//
// Created by maria on 31.03.17.
//
#include <cstdio>
#include <memory>
#include <iostream>
#include <emmintrin.h>
#include <ctime>

const uint64_t N = 1024 * 1024 * 1024;

//byte by byte
void* memcpy_naive (void* dest, const void* src, std:: size_t count) {
    for (size_t i = 0; i < count; i++) {
        *((char*) dest + i) = *((const char*) src + i);
    }
    return dest;
}

//by 8 bytes, general register
void* memcpy_8(void* dest, const void* src, std:: size_t count) {
    for (size_t i = 0; i < count; i+= 8) {
        size_t buffer = 0;
        __asm__ volatile("mov %1, %0\n"
                "mov %0, %2"
                 :
                 : "r"(buffer), "r"((const char*)src + i), "r" ((char*)dest + i));
    }
    return dest;
}

//movdqu - Move Unaligned Double Quadword
//(moves a double quadword from the source operand (second operand) to the destination operand (first operand))

//movntdq (Store Double Quadword Using Non-Temporal Hint)
//(moves the packed integers in the source operand (second operand) to the destination operand (first operand) using a non-temporal hint to prevent caching of the data during the write to memory)

void* my_memcpy(void* dest, void const* src, size_t size) {
    size_t pos = 0;
    while (((size_t) dest + pos) % 16 != 0) {
        *((char*)dest + pos) = *((char*)src + pos);
        pos++;
    }
    size_t tail = (size - pos) % 16;

    for (size_t i = pos; i < size - tail; i += 16) {
        __m128i reg;
        __asm__ volatile ("movdqu (%1), %0\n"
                 "movntdq %0, (%2)\n"
                 : "=x" (reg)
                 : "r"((const char*) src + i), "r"((char*)dest + i)
                 : "memory");
    }
    for (size_t i = (size - tail); i < size; i++) {
        *((char*)dest + i) = *((char*)src + i);
    }
    _mm_sfence();
    return dest;
}


int main() {
    char* src = new char[N];
    char* dest = new char[N];

//    std::clock_t start1 = std::clock();
//    memcpy_naive(dest, src, N);
//    std::clock_t end1 = std::clock();
//
//    memcpy_8(dest, src, N);
    std::clock_t end2 = std::clock();

    my_memcpy(dest, src, N);
    std::clock_t end3 = std::clock();


    //std::cout << "Memcpy without assembler inlines: " << end1 - start1 << "ms" <<std::endl;
    //std::cout << "Memcpy by 8 bytes: " << end2 - end1 << "ms" <<std::endl;
    std::cout << "Memcpy optimized (by 16 bytes): " << end3 - end2 << "ms" <<std::endl;
}