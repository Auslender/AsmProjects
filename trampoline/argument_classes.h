#ifndef TRAMPOLINE_ARGUMENT_CLASSES_H
#define TRAMPOLINE_ARGUMENT_CLASSES_H

#include <mmintrin.h>

/**
    INTEGER class consists of integral types that fit into one of the general purpose registers (%rdi, %rsi, %rdx, %rcx, %r8 and %r9)
    SSE consists of types that fit into a vector register (%xmm0...%xmm7)
 **/

template <typename ... Args>
struct argument_classes;

template <>
struct argument_classes<> {
    static constexpr int INTEGER = 0;
    static constexpr int SSE = 0;
};

template <typename ...Args>
struct argument_classes<float, Args ...> {
    static constexpr int INTEGER = argument_classes<Args...>::INTEGER;
    static constexpr int SSE = argument_classes<Args...>::SSE + 1;
};

template <typename ...Args>
struct argument_classes<double, Args ...> {
    static constexpr int INTEGER = argument_classes<Args...>::INTEGER;
    static constexpr int SSE = argument_classes<Args...>::SSE + 1;
};

template <typename ...Args>
struct argument_classes<__m64, Args ...> {
    static constexpr int INTEGER = argument_classes<Args...>::INTEGER;
    static constexpr int SSE = argument_classes<Args...>::SSE + 1;
};

template <typename T, typename ...Args>
struct argument_classes<T, Args ...> {
    static constexpr int INTEGER = argument_classes<Args...>::INTEGER + 1;
    static constexpr int SSE = argument_classes<Args...>::SSE;
};

#endif //TRAMPOLINE_ARGUMENT_CLASSES_H
