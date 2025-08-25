#include "module.getFileName.h"

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

std::string get_module_filename()
{
    return get_module_path() + "/is_even.bin";
}