#include "../../lib-common/include/cstring.h"
#include "process.h"
#include "console.h"
#include "memory.h"
#include "rpc.h"
#include "stdint.h"

#define INITRFS_MAX_FILE_COUNT 65536

struct file {
  uint32_t size;
  uint8_t content;
};

uint32_t evt_tarAvailable = 0;

int processEvent(uint32_t arg0, void* data, uint32_t size) {
  switch(arg0) {
  case 0x00: //Module Initialized
    kprintf("initialized\n");
    evt_tarAvailable = registerToEventByName("tarAvailable");
    throwEventByName("unpackInitrfs");
    break;

  case 0x1D7E: //IDLE
    break;

  default:
    if(arg0 == evt_tarAvailable) {
      kprintf("tarAvailable: %x\n", arg0);

      char* dataUI = (char*)data;

      for(int i = 0; i < 10; i++) {
        kputc(dataUI[i]);
      }
      kprintf("\n");

    }
    break;
  }

  return 0;
}
