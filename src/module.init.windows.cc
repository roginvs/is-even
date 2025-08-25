#include <cstdio>
#include <cstdint>
#include <cstdlib> // std::getenv
#include <string>
#include <windows.h>

#include "module.init.h"
#include "module.getFileName.h"
#include "module.init.printhex.h"

static void print_last_error(const char *where)
{
    DWORD err = GetLastError();
    LPSTR msg = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, err, 0, reinterpret_cast<LPSTR>(&msg), 0, nullptr);
    if (msg)
    {
        std::fprintf(stderr, "%s failed (error %lu): %s", where, err, msg);
        LocalFree(msg);
    }
    else
    {
        std::fprintf(stderr, "%s failed (error %lu)\n", where, err);
    }
}

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

    HANDLE binFile = CreateFileA(
        fileName.c_str(),
        GENERIC_READ | GENERIC_EXECUTE, FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (binFile == INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();
        printf("CreateFile failed, error code = %lu\n", err);
        perror("CreateFile failed");
        return -1;
    }

    LARGE_INTEGER codeSize;
    if (!GetFileSizeEx(binFile, &codeSize))
    {
        print_last_error("GetFileSizeEx");
        CloseHandle(binFile);
        return -1;
    }
    if (codeSize.QuadPart == 0)
    {
        std::fprintf(stderr, "File is empty: %s\n", fileName.c_str());
        CloseHandle(binFile);
        return -1;
    }

    HANDLE mapping = CreateFileMapping(
        binFile,
        NULL,
        PAGE_EXECUTE_READ,
        0,
        0,
        NULL);
    if (!mapping)
    {
        print_last_error("CreateFileMappingA");
        CloseHandle(binFile);
        return -1;
    }

    CloseHandle(binFile);
    binFile = INVALID_HANDLE_VALUE;

    LPVOID addr = MapViewOfFile(
        mapping, FILE_MAP_EXECUTE | FILE_MAP_READ,
        0,
        0,
        codeSize.QuadPart);
    if (!addr)
    {
        print_last_error("MapViewOfFile");
        CloseHandle(mapping);
        return -1;
    }

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        printf("Memory mapped at address: %p\n", addr);
    }

    CloseHandle(mapping);
    mapping = nullptr;

    is_even = (IsEven *)addr;

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        print_func_hex();
    }

    return 0;
}
