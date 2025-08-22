#pragma once

typedef int IsEven(int);

extern IsEven *is_even;

int init_module_if_needed();