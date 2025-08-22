#include "get_module_filename.h"

#include <dlfcn.h>
#include <string.h>
#include <libgen.h>
#include <exception>
#include <iostream>

std::string get_module_path()
{
    Dl_info info;
    if (dladdr((void *)get_module_path, &info))
    {
        char *dir = strdup(info.dli_fname);

        dirname(dir); // Get directory of build module

        std::string s{dir};

        free(dir);

        return s;
    }
    else
    {
        std::cerr << "Failed to get module path" << std::endl;
        std::terminate();
    }
}

std::string get_module_filename()
{
    return get_module_path() + "/is_even.bin";
}