#pragma once

#include "module.init.h"
#include <cstddef>
#include <cstdio>
#include <ctype.h>

static inline void print_func_hex()
{
    unsigned char *p = (unsigned char *)is_even;

    size_t len = 16 * 16;

    for (size_t i = 0; i < len; i += 16)
    {
        if (false)
        {
            // Print offset
            printf("%08zx  ", i);
        }

        // Print hex bytes
        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < len)
                printf("%02x ", p[i + j]);
            else
                printf("   "); // padding
        }

        // Print ASCII
        if (false)
        {
            printf(" |");
            for (size_t j = 0; j < 16 && i + j < len; j++)
            {
                unsigned char c = p[i + j];
                printf("%c", isprint(c) ? c : '.');
            }
            printf("|");
        }
        printf("\n");
    }
    printf("\n");
}
