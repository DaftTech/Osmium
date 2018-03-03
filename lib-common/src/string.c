#include "string.h"
#include "stdlib.h"

int strcmp(const char* str1, const char* str2) {
    int l1 = strlen(str1);
    int l2 = strlen(str2);
    l1 = l1 < l2 ? l1 : l2;

    return memcmp(str1, str2, (l1 + 1) * sizeof(char));
}

unsigned int strlen(const char* str) {
    if (str == 0)
        return 0;
    int i;
    for (i = 0; str[i] != '\0'; i++)
        ;
    return i;
}

char* strcpy(char* dest, const char* src) {
    memcpy(dest, src, strlen(src) * sizeof(char) + 1);
    return dest;
}

char* strclone(const char* str) {
    char* ret = malloc(sizeof(char) * strlen(str) + 1);
    strcpy(ret, str);

    return ret;
}

char* sp = NULL; /* the start position of the string */
void* fr = NULL;

char* strtok(char* str, const char* delimiters) {
    if(str != 0) {
        if(fr != 0) {
            free(fr);
        }
        fr = strclone(str);
        return strtoknc(fr, delimiters);
    }
    return strtoknc(0, delimiters);
}

char* strtoknc(char* str, const char* delimiters) {
    int i = 0;
    int len = strlen(delimiters);

    /* check in the delimiters */
    if (len == 0)
        return 0;

    /* if the original string has nothing left */
    if (!str && !sp)
        return 0;

    /* initialize the sp during the first call */
    if (str) {
        sp = str;
    }

    /* find the start of the substring, skip delimiters */
    char* p_start = sp;
    while (1) {
        for (i = 0; i < len; i++) {
            if (*p_start == delimiters[i]) {
                p_start++;
                break;
            }
        }

        if (i == len) {
            sp = p_start;
            break;
        }
    }

    /* return NULL if nothing left */
    if (*sp == '\0') {
        sp = NULL;
        return sp;
    }

    /* find the end of the substring, and replace the delimiter with null*/
    while (*sp != '\0') {
        for (i = 0; i < len; i++) {
            if (*sp == delimiters[i]) {
                *sp = '\0';
                break;
            }
        }

        sp++;
        if (i < len)
            break;
    }

    return p_start;
}
