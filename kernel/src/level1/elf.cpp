#include "level1/elf.h"
#include "level0/catofdeath.h"
#include "level0/console.h"
#include "level0/vmm.h"
#include "level1/scheduler.h"

char* unpackELFSymbolTable(void* elf, uint32_t eip) {
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

	uint32_t maxDiff = 0xFFFFFFFF;
	char* ret = "n/a";

  for(int i = 0; i < symbolCount; i++) {
		if(symbols[i].st_value < eip && symbols[i].st_value + symbols[i].st_size > eip) return str_raw + symbols[i].st_name;
  }

	return ret;
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

				uint32_t pageStart = (ph->virt_addr & 0xFFFFF000);
				uint32_t pageEnd = ((ph->virt_addr + ph->mem_size) & 0xFFFFF000);

        /* Nur Program Header vom Typ LOAD laden */
        if (ph->type != 1) {
            continue;
        }

        kprintf("ELF loading section %x - %x\n", ph->virt_addr, ph->virt_addr + ph->mem_size);

        for (uint32_t map = pageStart; map <= pageEnd; map += 0x1000) {
					kprintf("vmmResolve %x\n", map);
					if(!vmmResolve((void*)map)) {
						kprintf("vmmAllocate %x\n", map);
          	vmmAllocateAddress((void*)map, 0);
					}
        }

        memcpy(dest, src, ph->file_size);
    }

    return elf_mod_entry;
}
