#include "build_code.h"
#include "get_module_filename.h"
#include <iostream>
#include "codegen.cc"

int build_code()
{
    auto fileName = get_module_filename();

    auto codeGen = create_code_generator(std::getenv("IS_EVEN_LIGHT"), std::getenv("IS_EVEN_DEBUG"));

    return codeGen.generate(fileName.c_str());
}