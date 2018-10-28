#pragma once

#include <string>
#include <array>

// #define NDEBUG // uncomment to disable assert
#include <cassert>

using u8 = u_int8_t;
using u32 = u_int32_t;

namespace rb {
    template<size_t N>
    constexpr size_t length(char const (&)[N]){
        return N-1;
    }

    void print_hex(const u8 *buf, size_t len);

    template<typename It>
    void from_hex(const std::string& str, const It& rbegin, const It& rend);

    template<typename Arr, typename Str>
    auto from_hex(const Str& str);

    template< class T >
    std::string to_hex(const T& xs);

    template<class T, size_t N>
    std::array<T,N> to_array(const std::string& s);

    template<typename T, size_t N>
    inline std::array<T,N> to_array(const std::string& s) {
        assert(s.size() == N);
        std::array<T, N> d;
        std::copy(s.begin(), s.end(), d.begin()); // this is the recommended way
        return d;
    }

    template< class T >
    inline std::string to_hex(const T& xs) {
        static const char *digits = "0123456789abcdef";
        std::string str(xs.size() * 2, '0');
        int i = 0;
        for(auto it = str.begin(); it != str.end(); it+=2, i++) {
            *it = digits[ (xs[i] >> 4) & 0xf ];
            *(it+1) = digits[ xs[i] % 16 ];
        }
        return str;
    }
}

namespace io {
    int get_y_n();
}
