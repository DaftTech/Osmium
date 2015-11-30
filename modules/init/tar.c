#include "tar.h"
#include "memory.h"
#include "driver.h"
#include "stdlib.h"
#include "stdint.h"
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

void tar_extract(void* tarball, uint32_t** files, int driverID) {
    void* address = tarball;

    uint32_t i;

    for (i = 0; ; i++)
    {
        struct tar_header *header = (struct tar_header*)address;

        if (header->name[0] == '\0')
            break;

        uint32_t size = tar_parse_number(header->size);
        address += 512;

        if(size != 0) {
            kprintf("[initrfs] Extracting %s (%d bytes)...\n", header->name, size);

            files[i] = malloc(size + 4);
            files[i][0] = size;
            memcpy(&files[i][1], address, size);

            register_path(header->name, driverID, i);
        }

        address += (size / 512) * 512;

        if (size % 512)
            address += 512;
    }

    return;
}
