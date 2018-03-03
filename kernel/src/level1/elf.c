#include "level1/elf.h"
#include "level0/catofdeath.h"
#include "level0/console.h"
#include "level0/vmm.h"

ADDRESS unpackELF(void* elf) {
	struct elf_header* header = elf;
    struct elf_program_header* ph;

    /* Ist es ueberhaupt eine ELF-Datei? */
    if (header->magic != ELF_MAGIC) {
    	showCOD(0, "No ELF");
        return 0;
    }

    ADDRESS elf_mod_entry = header->entry;

    ph = (struct elf_program_header*) (((char*) header) + header->ph_offset);

    for (uint32_t n = 0; n < header->ph_entry_count; n++, ph++) {
        void* dest = (void*) ph->virt_addr;
        void* src = ((char*) header) + ph->offset;

        /* Nur Program Header vom Typ LOAD laden */
        if (ph->type != 1) {
            continue;
        }

        for (uint32_t offset = 0; offset < ph->mem_size; offset += 0x1000) {
            vmmFree(dest + offset); //FIXME: ELF kann Kernel Memory unloaden.
            vmmAllocateAddress(dest + offset, 0);
        }

        memcpy(dest, src, ph->file_size);
    }

    return elf_mod_entry;
}
