#include "process.h"

extern int main(void* args);

void _start(void* args) {
  int result = main(args);

  exit(result);
}

void exit(int returncode) {
    struct regstate state = {
        .eax = 1,
        .ebx = returncode,
        .ecx = 0,
        .edx = 0,
        .esi = 0,
        .edi = 0
    };

    syscall(&state);

    while(1) {
    }
}
