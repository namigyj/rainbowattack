#include <array>
//#include <iostream>

#include <cryptopp/sha.h>

#include "utils.hpp"
#include "config.h"

namespace rb {
    using u8 = unsigned char;

    template<size_t Hd, std::size_t Tl>
    class RowPair {
        struct row_pair {
            std::array<u8, Hd> head;
            std::array<u8, Tl> tail;
        };

        struct row_pair rp;

    public:
        RowPair() {};

        RowPair(const std::string& hd);

        //RowPair(std::string hd);

        RowPair(const std::array<u8, Hd>& hd);

        RowPair(const std::array<u8, Hd>& hd,
                const std::array<u8, Tl>& tl);

        struct row_pair * data();

        size_t size();

        std::array<u8, Hd>& head();

        std::array<u8, Tl>& tail();
    };

    template <typename Digest, typename T>
    inline Digest& hash(Digest& o, T pt) {
        CryptoPP::SHA256().CalculateDigest(o.data(), pt.data(), pt.size());
        return o;
    }

    template <typename T, size_t N>
    T& reduce(T& o, std::array<u8, N> digest, int n) {
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

}
