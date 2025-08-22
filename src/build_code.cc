#include "build_code.h"
#include "get_module_filename.h"
#include <iostream>

// https://defuse.ca/online-x86-assembler.htm#disassembly

/*
mov rax, 0xffffffff
ret
*/
static unsigned char fabula[] = {0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF,
                                 0x00, 0x00, 0x00, 0x00, 0xC3};

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

int build_code()
{
    auto fileName = get_module_filename();

    printf("Generating file %s...\n", fileName.c_str());

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
            printf("Processing: %lx, %li%%\n", i, (i * 100) / max_i);
        }
        __uint32_t val = i;
        (*(__uint32_t *)&iteration[2]) = val;
        iteration[9] = i % 2 == 0 ? 1 : 0;

        fwrite(iteration, sizeof(iteration), 1, fp);
    }

    fwrite(fabula, sizeof(fabula), 1, fp);
    fclose(fp);

    printf("Done\n");

    return 0;
}