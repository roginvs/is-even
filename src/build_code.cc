#include "build_code.h"
#include "get_module_filename.h"
#include <iostream>

#if defined(__x86_64__) || defined(_M_X64)

// https://defuse.ca/online-x86-assembler.htm#disassembly

/*
mov rax, 0
*/
static unsigned char preamble[] = {0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00};

/*
cmp edi, 0xaabbccdd
jnz L1
mov al, 0xab
ret
L1:
*/
static unsigned char iteration[] = {0x81, 0xFF, 0xDD, 0xCC, 0xBB, 0xAA,
                                    0x75, 0x03, 0xB0, 0xAB, 0xC3};

/*
mov rax, 0xffffffff
ret
*/
static unsigned char epilogue[] = {0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF,
                                   0x00, 0x00, 0x00, 0x00, 0xC3};

#elif defined(__aarch64__) || defined(_M_ARM64)

/*
mov     x0, #0
*/

static unsigned char preamble[] = {0x00, 0x00, 0x80, 0xD2};
/*

0000:  A1 9B 99 52    // movz  w1, #0xccdd
0004:  61 57 B5 72    // movk  w1, #0xaabb, lsl #16
0008:  1F 00 01 6B    // cmp   w0, w1        (subs wzr,w0,w1)
000C:  41 00 00 54    // b.ne  L1            (to 0x18; imm19 = 2)
0010:  20 00 80 52    // mov   w0, #1        (alias of movz w0,#1)
0014:  C0 03 5F D6    // ret
0018:  ...            // L1:

L1:
*/

static unsigned char iteration[] = {};
/*
  mov     w0, #0xffffffff
ret
*/
static unsigned char epilogue[] = {}

#else
#error "Unsupported architecture"
#endif

int build_code()
{
    auto fileName = get_module_filename();

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        printf("Generating file %s...\n", fileName.c_str());
    }

    FILE *fp = fopen(fileName.c_str(), "wb");
    if (!fp)
    {
        perror("fopen");
        return -1;
    }

    fwrite(preamble, sizeof(preamble), 1, fp);

    long int max_i = std::getenv("IS_EVEN_LIGHT") ? 0xFFFF : 0xFFFFFFFF;

    for (long int i = 0; i <= max_i; i++)
    {
        if (i % 0x100000 == 0)
        {
            if (std::getenv("IS_EVEN_DEBUG"))
            {
                printf("Processing: %lx, %li%%\n", i, (i * 100) / max_i);
            }
        }
        __uint32_t val = i;
        (*(__uint32_t *)&iteration[2]) = val;
        iteration[9] = i % 2 == 0 ? 1 : 0;

        fwrite(iteration, sizeof(iteration), 1, fp);
    }

    fwrite(epilogue, sizeof(epilogue), 1, fp);
    fclose(fp);

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        printf("Done\n");
    }

    return 0;
}