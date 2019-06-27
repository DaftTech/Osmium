#include "level0/console.h"
#include "level0/gdt.h"
#include "level0/pmm.h"
#include "level0/vmm.h"
#include "level0/idt.h"
#include "level0/ports.h"
#include "level0/catofdeath.h"
#include "level1/scheduler.h"
#include "level1/events.hpp"
#include "level1/tar.h"
#include "level1/elf.h"
#include "stdlib.h"
#include "multiboot.h"

void* globTarball;

extern "C" void clevel_entry(MultibootInfo* mb_info) {
  clrscr();
  setclr(COLOR(SCLR_BLACK, SCLR_CYAN));
  kprintf("LEVEL0 ENTRY\n");

  setclr(C_DEFAULT);
  kprintf("GDT INIT ");
  initGDT();
  setclr(C_SUCCESS);
  kprintf("DONE!\n");
  setclr(C_DEFAULT);

  kprintf("IDT INIT ");
  initIDT();
  setclr(C_SUCCESS);
  kprintf("DONE!\n");
  setclr(C_DEFAULT);

  kprintf("PMM INIT ");
  pmmInit(mb_info);
  setclr(C_SUCCESS);
  kprintf("DONE!\n");
  setclr(C_DEFAULT);

  setclr(COLOR(SCLR_BLACK, SCLR_YELLOW));
  pmmPrintStats();
  setclr(C_DEFAULT);

  kprintf("VMM/PAGING INIT ");
  PADDR root = vmmInit();
  setclr(C_SUCCESS);
  kprintf("DONE! (root=%x, getRoot=%x)\n", root, vmmGetActivePhysical());
  setclr(C_DEFAULT);

  setclr(COLOR(SCLR_BLACK, SCLR_CYAN));
  kprintf("LEVEL1 ENTRY\n");
  setclr(C_DEFAULT);

  kprintf("Creating root environment...\n");
  Environment* rootEnv = createEnvironment(root);

  kprintf("Mapping multiboot...\n");

  vmmMapAddress(mb_info, (uint32_t) mb_info, 0);
  vmmMapAddress(mb_info->mi_mods_addr, (uint32_t) mb_info->mi_mods_addr, 0);

  if (!(mb_info->mi_flags & MULTIBOOT_INFO_HAS_MODS)) {
    showCOD(0, "No multiboot module (initrfs?) available.\n");
  }

  void* initrfs = mb_info->mi_mods_addr[0].start;
  size_t initrfsSize = (size_t)mb_info->mi_mods_addr[0].end - (size_t)mb_info->mi_mods_addr[0].start;

  vmmMapRange(initrfs, (uint32_t) initrfs, initrfsSize,  0);

  kprintf("Assuming mbmod[0] is a tarball (%d bytes)... \n", initrfsSize);

  uint32_t* initELF = (uint32_t*) tarExtract(mb_info->mi_mods_addr[0].start, "init");
  globTarball = (void*)mb_info->mi_mods_addr[0].start;

  if(initELF == (void*)0) {
    showCOD(0, "initrfs damaged or didn't contain \"/init\".\n");
  }

  kprintf("Unpacking ELF...\n");
  ADDRESS entryPoint = unpackELF(&initELF[1]);

  if(entryPoint == 0) {
    showCOD(0, "init has no valid entry point...\n");
  }

  kprintf("Registering init module...\n");
  registerModule(rootEnv, entryPoint);

  kprintf("Setting PIT interval...\n");

  int counter = 1193182 / 100;
  outb(0x43, 0x34);
  outb(0x40,counter & 0xFF);
  outb(0x40,counter >> 8);

  kprintf("Registering kernel events...\n");
  registerKernelEvents();

  kprintf("Enabling scheduler...\n");
  enableScheduling();

  while(1) {
  }
}
