#include "level1/elf.h"
#include "level0/catofdeath.h"
#include "level0/console.h"
#include "level0/vmm.h"
#include "level1/scheduler.h"

void unpackELFSymbolTable(void* elf) {
	ELFHeader* header = (ELFHeader*) elf;

  ELFSectionHeader* sh = (ELFSectionHeader*) (((char*) header) + header->sh_offset);

  char* str_raw = nullptr;
  ELFSymbol* symbols = nullptr;
  int symbolCount = 0;

  for(uint32_t n = 0; n < header->sh_entry_count; n++, sh++) {
    if(sh->sh_type == 0x03) {
      str_raw = (((char*) header) + sh->sh_offset);
      break;
    }
    if(sh->sh_type == 0x02) {
      symbols = (ELFSymbol*)(((char*) header) + sh->sh_offset);
      symbolCount = sh->sh_size / sh->sh_entsize;
    }
  }

  for(int i = 0; i < symbolCount; i++) {
    kprintf("%s\n", str_raw + symbols[i].st_name);
  }
}

ADDRESS unpackELF(void* elf) {
	ELFHeader* header = (ELFHeader*) elf;
    ELFProgramHeader* ph;
    ELFSectionHeader* sh;

    /* Ist es ueberhaupt eine ELF-Datei? */
    if (header->magic != ELF_MAGIC) {
    	showCOD(0, "No ELF");
        return 0;
    }

    ADDRESS elf_mod_entry = header->entry;

    ph = (ELFProgramHeader*) (((char*) header) + header->ph_offset);

    kprintf("ELF loading program headers...\n");

    for (uint32_t n = 0; n < header->ph_entry_count; n++, ph++) {
        uint8_t* dest = (uint8_t*) ph->virt_addr;
        uint8_t* src = ((uint8_t*) header) + ph->offset;

        /* Nur Program Header vom Typ LOAD laden */
        if (ph->type != 1) {
            continue;
        }

        kprintf("ELF loading section %x - %x\n", ph->virt_addr, ph->virt_addr + ph->mem_size);

        for (uint32_t offset = 0; offset < ph->mem_size; offset += 0x1000) {
            vmmFree(dest + offset); //FIXME: ELF kann Kernel Memory unloaden.
            vmmAllocateAddress(dest + offset, 0);
        }

        memcpy(dest, src, ph->file_size);
    }

    return elf_mod_entry;
}
