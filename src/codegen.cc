#include <iostream>

#include "codegen.h"

#include "codegen.file.cc"
#include "module.getFileName.h"

int generate_code_file()
{
    auto fileName = get_module_filename();

    auto codeGen = create_code_generator(std::getenv("IS_EVEN_LIGHT"), std::getenv("IS_EVEN_DEBUG"));

    return codeGen.generate(fileName.c_str());
}