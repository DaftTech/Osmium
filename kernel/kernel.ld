/*  Bei _start soll die Ausfuehrung losgehen */
ENTRY(_start)

/*
 * Hier wird festgelegt, in welcher Reihenfolge welche Sektionen in die Binary
 * geschrieben werden sollen
 */
SECTIONS
{
	/*
	 * . ist die aktuelle Position in der Datei. Wir wollen den Kernel wie gehabt
	 * an 1 MB laden, also muessen wir dort die erste Sektion hinlegen
	 */
	. = 0x100000;

	kernel_start = .;

	/*
	 * Der Multiboot-Header muss zuerst kommen (in den ersten 8 kB).
	 * Die Standardsektionen einfach hintereinander weg einbinden.
	 */
	.text : {
		*(multiboot)
		*(.text)
	}
	.data ALIGN(4096) : {
	    start_ctors = .;
	    KEEP(*( .init_array ));
	    end_ctors = .;
		*(.data)
	}
	.rodata ALIGN(4096) : {
		*(.rodata)
	}
	.bss ALIGN(4096) : {
		*(.bss)
	}

  	/DISCARD/ : { *(.fini_array*) *(.comment) }
  	
	. = ALIGN(4096);
	kernel_end = .;
}