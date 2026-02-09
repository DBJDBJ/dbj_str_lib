#pragma once


// Gustedt's defer macro
#define defer __DEFER(__COUNTER__)
#define __DEFER(N) __DEFER_(N)
#define __DEFER_(N) __DEFER__(__DEFER_FUNCTION_##N, __DEFER_VARIABLE_##N)

#define __DEFER__(F, V)        \
    auto void F(int*);         \
    [[gnu::cleanup(F)]] int V; \
    auto void F(int*)
