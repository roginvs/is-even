#include "module.init.h"
#include "module.getFileName.h"

#include <stdio.h>

IsEven *is_even = nullptr;

#if defined(__linux__) || defined(__APPLE__)

#include "module.init.posix.cc"

#elif defined(_WIN64)

#include "module.init.windows.cc"

#else
#error "Unknown platform"
#endif
