#include "build_code.h"
#include "get_module_filename.h"
#include <iostream>
#include "codegen.cc"

#if defined(__x86_64__) || defined(_M_X64)

#elif defined(__aarch64__) || defined(_M_ARM64)

/*
mov     x0, #0
*/

static unsigned char preamble[] = {0x00, 0x00, 0x80, 0xD2};
/*

0000:  A1 9B 99 52    // movz  w1, #0xccdd
0004:  61 57 B5 72    // movk  w1, #0xaabb, lsl #16
0008:  1F 00 01 6B    // cmp   w0, w1        (subs wzr,w0,w1)
000C:  41 00 00 54    // b.ne  L1            (to 0x18; imm19 = 2)
0010:  20 00 80 52    // mov   w0, #1        (alias of movz w0,#1)
0014:  C0 03 5F D6    // ret
0018:  ...            // L1:

L1:
*/

static unsigned char iteration[] = {};
/*
  mov     w0, #0xffffffff
ret
*/
static unsigned char epilogue[] = {}

#else
#error "Unsupported architecture"
#endif

int build_code()
{
    auto fileName = get_module_filename();

    auto codeGen = create_code_generator(std::getenv("IS_EVEN_LIGHT"), std::getenv("IS_EVEN_DEBUG"));

    return codeGen.generate(fileName.c_str());
}