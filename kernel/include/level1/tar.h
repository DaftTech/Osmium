#ifndef TAR_H_
#define TAR_H_

#include "stddef.h"

struct tar_header
{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
};

void* tar_extract(void* tarball, const char* name);

#endif /* TAR_H_ */
