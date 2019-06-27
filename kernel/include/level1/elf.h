#ifndef ELF_H_
#define ELF_H_

#include "stdint.h"
#include "stddef.h"

#define ELF_MAGIC 0x464C457F

struct LoadedELF {
  ADDRESS entry;
  char* stringTable;
};

struct ELFHeader {
    uint32_t magic;
    uint32_t version;
    uint64_t reserved;
    uint64_t version2;
    uint32_t entry;
    uint32_t ph_offset;
    uint32_t sh_offset;
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_entry_size;
    uint16_t ph_entry_count;
    uint16_t sh_entry_size;
    uint16_t sh_entry_count;
    uint16_t sh_str_table_index;
}__attribute__((packed));

struct ELFProgramHeader {
    uint32_t type;
    uint32_t offset;
    uint32_t virt_addr;
    uint32_t phys_addr;
    uint32_t file_size;
    uint32_t mem_size;
    uint32_t flags;
    uint32_t alignment;
}__attribute__((packed));

struct ELFSectionHeader {
  uint32_t sh_name;
  uint32_t sh_type;
  uint32_t sh_flags;
  uint32_t sh_addr;
  uint32_t sh_offset;
  uint32_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint32_t sh_addralign;
  uint32_t sh_entsize;
}__attribute__((packed));

struct ELFSymbol {
  uint32_t st_name;
  uint32_t st_value;
  uint32_t st_size;
  uint8_t st_info;
  uint8_t st_other;
  uint16_t st_shndx;
}__attribute__((packed));

void unpackELFSymbolTable(ELFSectionHeader* sh, uint32_t num);
ADDRESS unpackELF(void* elf);

#endif
