#include "level1/tar.h"

#include "../../../lib-common/include/cstring.h"
#include "level0/console.h"
#include "level0/vmm.h"
#include "level1/events.hpp"
#include "stdlib.h"

static uint32_t tarParseNumber(const char *in)
{
    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1;

    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);

    return size;
}

void tarThrowEvents() {
  uint8_t* address = (uint8_t*) globTarball;

  uint32_t i;

  kprintf("Extracting tarball on unpack event %x...\n", address);

  for (i = 0; ; i++)
  {
    TARHeader *header = (TARHeader*)address;

    if (header->name[0] == '\0')
      break;

    uint32_t size = tarParseNumber(header->size);
    address += 512;

    if(size != 0) {
      kprintf("Extracting %s (%d bytes)\n", header->name, size);

      uint32_t* dest = (uint32_t*) malloc(size);
      memcpy(dest, address, size);

      getEventByName("tarAvailable")->callListeners(dest, size);
    }

    address += (size / 512) * 512;

    if (size % 512)
      address += 512;
  }

  kprintf("Finished...\n");
  return;
}

void* tarExtract(void* tarball, const char* path) {
  uint8_t* address = (uint8_t*) tarball;

  uint32_t i;

  kprintf("Searching for %s in tarball %x\n", path, address);

  for (i = 0; ; i++)
  {
    TARHeader *header = (TARHeader*)address;

    if (header->name[0] == '\0')
      break;

    uint32_t size = tarParseNumber(header->size);
    address += 512;

    if(size != 0 && !strcmp(header->name, path)) {
      kprintf("FOUND! Extracting %s (%d bytes)\n", header->name, size);

      uint32_t* dest = (uint32_t*) malloc(size + 4);
      dest[0] = size;
      memcpy(&dest[1], address, size);
      return dest;
    }

    address += (size / 512) * 512;

    if (size % 512)
      address += 512;
  }

  kprintf("Not found...\n");
  return 0;
}
