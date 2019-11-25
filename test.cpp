#include <array>
#include <cassert>
#include <iostream>
#include <thread>
#include <atomic>

#include <cryptopp/sha.h>

#include "config.h"
#include "utils.hpp"


template <typename T, size_t N>
auto& reduce(T& o, std::array<u8, N> digest, int n) {
    size_t r = n, k = 0, l = rb::length(charset);
    size_t i = 0;
    for(auto it = o.begin(); it != o.end(); it++) {
        /* r := n + d[i+0] + d[i+8] + d[i+16] + ...
         * but constrainted to the size of digest
         */
        for(size_t j = 0; j < (N/8)+1; j++) {
            k = i + 8*j;
            r += digest[k%N];
        }
        *it = charset[r % l];
        i++;
    }
    return o;
}

template<typename T>
auto& reduce(T& o, std::array<u8, 32> digest, int n) {
    size_t index;
    for(size_t i = 0; i < o.size(); i++) {
        index = digest[i] + digest[i+8] + digest[i+16]
            + digest[i+24] + n;
        o[i] = charset[index%64];
    }
    return o;
}

void test_reduction_dynN() {
    std::string s = "password";
    std::string s1(8, '0');
    std::string s2(8, '0');
    std::array<u8, 32> d1, d2;

    CryptoPP::SHA256().CalculateDigest(d1.data(), (u8 *)s1.data(), s.size());
    CryptoPP::SHA256().CalculateDigest(d2.data(), (u8 *)s2.data(), s.size());

    assert(reduce(s1, d1, 1) == reduce(s2, d2, 1));
}

const int max = 1000000;

void th_work(std::atomic<size_t> *s) {
    for (int i = 0; i < max; i++)
        (*s)++;
}

int main() {
    /* too lazy to use test framework */
    // test_reduction_dynN();
}
