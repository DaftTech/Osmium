#include "level1/tar.h"
#include "level0/console.h"
#include "level0/vmm.h"
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

size_t tar_extract_to_userspace_bottom(void* tarball, const char* path) {
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

            void* dest = vmm_alloc_ucont(((size - 1) / 0x1000) + 1); //FIXME: Assumption that vmm_alloc_ucont will begin at USERSPACE_BOTTOM
            memcpy(dest, address, size);
            return size;
        }

        address += (size / 512) * 512;

        if (size % 512)
            address += 512;
    }

    kprintf("Not found...\n");
    return 0;
}
