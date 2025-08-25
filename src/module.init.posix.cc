#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "module.init.h"
#include "module.getFileName.h"
#include "module.init.printhex.h"

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

    is_even = (IsEven *)addr;

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        print_func_hex();
    }

    return 0;
}
