#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "codegen.h"
#include "codegen.file.cc"
#include "module.getFileName.h"

/**
 * @brief Generate the code file for the module if needed
 *
 * Returns 0 if file exists
 * Returns 1 if file was created
 * Returns -1 on error
 *
 */
int generate_code_file()
{
    auto is_debug = std::getenv("IS_EVEN_DEBUG");

    auto fileName = get_module_filename();

    if (access(fileName.c_str(), F_OK) == 0)
    {
        if (is_debug)
        {
            printf("Re-using file %s...\n", fileName.c_str());
        }
        return 0;
    }

    auto codeGen = create_code_generator(std::getenv("IS_EVEN_LIGHT"), is_debug);

    auto tempFile = fileName + ".tmp";
    const int result = codeGen.generate(tempFile.c_str());
    if (result != 0)
    {
        perror("generate");
        return -1;
    }

    if (rename(tempFile.c_str(), fileName.c_str()) != 0)
    {
        perror("rename");
        return -1;
    }

    return 1;
}