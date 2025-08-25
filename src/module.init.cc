#include "module.init.h"
#include "module.getFileName.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

IsEven *is_even = nullptr;

int init_module_if_needed()
{
    if (is_even != nullptr)
    {
        return 0; // Module already initialized
    }

    auto fileName = get_module_filename();

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        printf("Binary file is %s\n", fileName.c_str());
    }

    int fd = open(fileName.c_str(), O_RDONLY);
    if (fd == -1)
    {
        perror("fopen");
        return -1;
    }

    struct stat statbuf;

    if (fstat(fd, &statbuf) < 0)
    {
        perror("fstat error");
        return -1;
    }

    void *addr = mmap((void *)0x10000000000ULL, statbuf.st_size,
                      PROT_EXEC | PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return 1;
    }
    close(fd);

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        printf("Memory mapped at address: %p\n", addr);
    }

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        unsigned char *p = (unsigned char *)addr;

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

    is_even = (IsEven *)addr;

    return 0;
}
