#include "module.getFileName.h"

#include <exception>
#include <iostream>

#if defined(__linux__) || defined(__APPLE__)

#include <dlfcn.h>
#include <string.h>
#include <libgen.h>

std::string get_module_path()
{
    Dl_info info;
    if (dladdr((void *)get_module_path, &info))
    {
        char *fileName = strdup(info.dli_fname);

        char *dir = dirname(fileName); // Get directory of build module

        std::string s{dir};

        free(fileName);

        return s;
    }
    else
    {
        std::cerr << "Failed to get module path" << std::endl;
        std::terminate();
    }
}

#elif defined(_WIN64)

#include <windows.h>
#include <vector>

static void die_win32(const char *where)
{
    DWORD err = ::GetLastError();
    LPSTR msg = nullptr;
    ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                     nullptr, err, 0, reinterpret_cast<LPSTR>(&msg), 0, nullptr);
    if (msg)
    {
        std::cerr << where << " failed (" << err << "): " << msg;
        ::LocalFree(msg);
    }
    else
    {
        std::cerr << where << " failed (" << err << ")\n";
    }
    std::terminate();
}

std::string get_module_path()
{
    // Get HMODULE that contains this function
    HMODULE hmod = nullptr;
    if (!::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              reinterpret_cast<LPCSTR>(&get_module_path),
                              &hmod))
    {
        die_win32("GetModuleHandleExA");
    }

    // Get full path to that module (may be longer than MAX_PATH if long paths enabled)
    std::vector<char> buf(260); // start with MAX_PATH-like, grow if needed
    DWORD len = 0;
    for (;;)
    {
        len = ::GetModuleFileNameA(hmod, buf.data(), static_cast<DWORD>(buf.size()));
        if (len == 0)
            die_win32("GetModuleFileNameA");
        if (len < buf.size() - 1)
            break; // success, not truncated
        buf.resize(buf.size() * 2);
    }

    std::string full(buf.data(), len);

    // Strip the filename, keep the directory
    // Works with both '\' and '/' just in case
    const auto pos = full.find_last_of("\\/");
    if (pos == std::string::npos)
    {
        // Shouldn't happen, but fall back to current directory
        return ".";
    }
    return full.substr(0, pos);
}

#else
#error "Unknown platform"
#endif

std::string get_module_filename()
{
    return get_module_path() + "/is_even.bin";
}
