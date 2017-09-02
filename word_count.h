#ifndef TRAMPOLINE_WORD_COUNT_H
#define TRAMPOLINE_WORD_COUNT_H

#include <cstddef>
#include <string>
#include <xmmintrin.h>
#include <x86intrin.h>

size_t word_count_naive(const char* str, size_t size) {
    bool ws = true;
    size_t c = 0;
    for (size_t i = 0; i < size; i++) {
        if (ws && str[i] != ' ') {
            c++;
        }
        ws = (str[i] == ' ');
    }
    return c;
}

size_t word_count_intrinsics(const char* str, size_t size) {
    if (size < 64) {
        return word_count_naive(str, size);
    }
    size_t pos = 0;
    size_t word_counter = (str[0] == ' ') ? 0 : 1;
    bool ws = false;

    while ((size_t)(str + pos) % 16 != 0) {
        char cur = *(str + pos);
        if (ws && cur != ' ') {
            word_counter++;
        }
        ws = (cur == ' ');
        pos++;
    }
    if (ws && *(str + pos) != ' ' && pos != 0) word_counter++;
    size_t tail = size - (size - pos) % 16 - 16;

    //ws_mask - 16 whitespaces
    const __m128i ws_mask = _mm_set1_epi8(' ');
    __m128i acc = _mm_set1_epi8(0);
    //take first 16 byte piece of the string
    //wss_i = (a_i == b_i) ? 0xFF : 0x00
    __m128i wss = _mm_cmpeq_epi8(_mm_load_si128((__m128i*)str + pos), ws_mask);

    for (size_t i = pos; i < tail; i += 16) {
        __m128i wss_prev = wss;
        //take the next piece of string
        wss = _mm_cmpeq_epi8(_mm_load_si128((__m128i*)(str + i + 16)), ws_mask);
        //check for sequential whitespaces
        //(wss << 128 | wss_prev) >> 8
        __m128i shifted_wss = _mm_alignr_epi8(wss, wss_prev, 1);
        __m128i andnot = _mm_andnot_si128(shifted_wss, wss_prev);
        acc = _mm_adds_epu8(_mm_and_si128(_mm_set1_epi8(1), andnot), acc);

        if (_mm_movemask_epi8(acc) != 0 || i + 16 >= tail) {
            acc = _mm_sad_epu8(_mm_set1_epi8(0), acc);
            word_counter += _mm_cvtsi128_si32(acc);
            acc = _mm_srli_si128(acc, 8);
            word_counter += _mm_cvtsi128_si32(acc);
            acc = _mm_set1_epi8(0);
        }
    }
    pos = tail;
    if (*(str + pos - 1) == ' ' && *(str + pos) != ' ') {
        word_counter--;
    }
    ws = (*(str + pos - 1) == ' ');
    for (size_t i = pos; i < size; i++) {
        char cur = *(str + i);
        if (ws && cur != ' ') {
            word_counter++;
        }
        ws = (cur == ' ');
    }
    return word_counter;
}

#endif //TRAMPOLINE_WORD_COUNT_H
