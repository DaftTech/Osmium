#include "level1/tar.h"
#include "level0/console.h"
#include "level0/vmm.h"
#include "stdlib.h"
#include "string.h"

static uint32_t tar_parse_number(const char *in)
{
    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1;

    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);

    return size;
}

void* tar_extract(void* tarball, const char* path) {
    void* address = tarball;

    uint32_t i;

    kprintf("Searching for %s in tarball %x\n", path, address);

    for (i = 0; ; i++)
    {

        struct tar_header *header = (struct tar_header*)address;

        if (header->name[0] == '\0')
            break;

        uint32_t size = tar_parse_number(header->size);
        address += 512;

        if(size != 0 && !strcmp(header->name, path)) {
            kprintf("FOUND! Extracting %s (%d bytes)\n", header->name, size);

            uint32_t* dest = malloc(size + 4);
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
