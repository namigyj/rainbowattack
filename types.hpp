#include <array>

#include <cryptopp/sha.h>

#include "utils.hpp"
#include "config.h"

namespace std {
    // see: https://stackoverflow.com/a/8026914
    template<typename T, size_t N>
    struct hash<array<T, N> > {
        typedef array<T, N> argument_type;
        typedef size_t result_type;

        result_type operator()(const argument_type& a) const {
            hash<T> hasher;
            result_type h = 0;
            for (result_type i = 0; i < N; ++i) {
                h = h * 31 + hasher(a[i]);
            }
            return h;
        }
    };
}

using hash_t = std::array<u8, def::tl_l>;
using pass_t = std::array<u8, def::hd_l>;

namespace rb {
    using namespace def;
    using u8 = unsigned char;

    template<size_t Hd, size_t Tl>
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

        RowPair(const std::array<u8, Hd>& hd
               ,const std::array<u8, Tl>& tl);

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

    template <typename T, size_t M, size_t N>
    inline std::array<T,M>& reduce(std::array<T, M>& o, const std::array<u8, N>& digest, const size_t& round) {
        size_t r = round;
        size_t k = 0, l = rb::length(charset);
        for(size_t i = 0; i < M; i++) {
            /* r := n + d[i+0] + d[i+8] + d[i+16] + ...
             * but constrainted to the size of digest
             */
            r = 0;
            for(size_t j = 0; j < (N/8)+1; j+=8) {
                k = i + 8*j;
                r += digest[k % N];
            }
            o[i] = charset[r % l];
        }
        return o;
    }

}
