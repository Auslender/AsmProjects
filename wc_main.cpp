//
// Created by maria on 15.08.17.
//
#include "word_count.h"
#include "iostream"

int main() {
    const char* str = "In the scalar version the explicit loading and unloading of data to and from memory to temporary";
    std::cout << word_count_naive(str, 97);
    std::cout << word_count_intrinsics(str, 97);
}
