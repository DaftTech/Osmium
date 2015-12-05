#include "process.h"
#include "console.h"
#include "memory.h"
#include "stdlib.h"
#include "rpc.h"
#include "driver.h"
#include "string.h"
#include "stdint.h"

struct driver_data* dd;

void send_command(uint8_t command)
{
    // Warten bis die Tastatur bereit ist, und der Befehlspuffer leer ist
    while ((inb(0x64) & 0x2)) {}
    outb(0x60, command);
}

static uint8_t sc_to_kc[][128] = {
    // Normale Scancodes
    {
          0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
         10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
         20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
         30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
         40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
         50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
         60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
         70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
         80,  81,  82,  84,  00,  00,  86,  87,  88,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00
    },

    // Extended0-Scancodes (werden mit e0 eingeleitet)
    {
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  96,  97,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  99,  00,  00, 100,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00, 102, 103, 104,  00, 105,  00, 106,  00, 107,
        108, 109, 110, 111,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00
    },
};

uint8_t translate_scancode(int set, uint16_t scancode)
{
    uint8_t keycode = 0;

    switch (set) {
        // Normale scancodes
        case 0:
            keycode = sc_to_kc[0][scancode];
            break;

        // e0-Scancodes
        case 1:
            keycode = sc_to_kc[1][scancode];
            break;

        // e1-Scancodes
        case 2:
            // TODO: Hier waere eigentlich eine Tabelle auch schoen
            switch (scancode) {
                // Pause
                case 0x451D:
                    keycode = 119;
                    break;

                default:
                    keycode = 0x0;
            };
            break;
    }

    if (keycode == 0) {
        kprintf("kbc: Unbekannter Scancode: 0x%x (%d)\n", scancode, set);
    }

    return keycode;
}

FUTURE irqFuture = 0;

int irq(int irq, void* data) {
	uint8_t scancode;
	uint8_t keycode = 0;
	int break_code = 0;

	// Status-Variablen fuer das Behandeln von e0- und e1-Scancodes
	static int     e0_code = 0;
	// Wird auf 1 gesetzt, sobald e1 gelesen wurde, und auf 2, sobald das erste
	// Datenbyte gelesen wurde
	static int      e1_code = 0;
	static uint16_t  e1_prev = 0;

	scancode = inb(0x60);

	// Um einen Breakcode handelt es sich, wenn das oberste Bit gesetzt ist und
	// es kein e0 oder e1 fuer einen Extended-scancode ist
	if ((scancode & 0x80) &&
		(e1_code || (scancode != 0xE1)) &&
		(e0_code || (scancode != 0xE0)))
	{
		break_code = 1;
		scancode &= ~0x80;
	}

	if (e0_code) {
		// Fake shift abfangen und ignorieren
		if ((scancode == 0x2A) || (scancode == 0x36)) {
			e0_code = 0;
			kprintf("E0 ret\n");
			return 0;
		}

		keycode = translate_scancode(1, scancode);
		e0_code = 0;
	} else if (e1_code == 2) {
		// Fertiger e1-Scancode
		// Zweiten Scancode in hoeherwertiges Byte packen
		e1_prev |= ((uint16_t) scancode << 8);
		keycode = translate_scancode(2, e1_prev);
		e1_code = 0;
	} else if (e1_code == 1) {
		// Erstes Byte fuer e1-Scancode
		e1_prev = scancode;
		e1_code++;
	} else if (scancode == 0xE0) {
		// Anfang eines e0-Codes
		e0_code = 1;
	} else if (scancode == 0xE1) {
		// Anfang eines e1-Codes
		e1_code = 1;
	} else {
		// Normaler Scancode
		keycode = translate_scancode(0, scancode);
	}

	if(keycode != 0) {
		dd->data[0] = keycode;
		dd->data[1] = break_code;
		dd->data[2] = 0;
		dd->length = 2;

		while(rpc_check_future(irqFuture)); //Check future before issuing write, cause we reuse same dd page
		irqFuture = fWrite("/dev/kbdRaw", dd);

		kprintf(" >%x:%x< ", keycode, break_code);
		return 1;
	}

	return 0;
}


int rmain(void* initrfsPtr) {
	dd = palloc();

    // Tastaturpuffer leeren
    while (inb(0x64) & 0x1) {
        inb(0x60);
    }

    // IRQ-Handler fuer Tastatur-IRQ(1) registrieren
	int rpcID = rpc_register_handler(irq);
    register_irq_rpc(0x21, rpcID);

	strcpy(dd->data, "/dev/kbdRaw");

	FUTURE fut = fCall("fifo", CALL_CREATE, dd);
	while(rpc_check_future(fut));

    // Tastatur aktivieren
    send_command(0xF4);

	fCall("initrfs", 0, 0); //TELL init we are finished here
	while(1) {
	}

	return 0;
}
