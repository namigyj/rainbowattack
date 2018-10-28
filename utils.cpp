#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <limits>

// #define NDEBUG // uncomment to disable assert
#include <cassert>

#include "utils.hpp"

namespace rb {

void print_hex(const u8 *buf, size_t len) {
    for(size_t i = 0; i < len; i++)
        printf("%02hhx", buf[i]);
}

auto pval(char c) {
    return c > '9' ? (c|0x20) - 'a' : c - '0';
}
template<typename It>
void from_hex(const std::string& str, const It& rbegin, const It& rend) {
    /* TODO : How to write nice implementors code in sepples */
    auto it = rbegin;
    int i = 0; // counter to know which semi-byte we are in.
    for(auto is = str.rbegin(); is != str.rend() && it != rend; is++) {
        if(i&1) {// odd char = Most significat hex
            *it |= pval(*is) << 4;
            it++;
        } else {
            *it = pval(*is);
        }
        i++;
    }
    /* if odd number of chars, we'd overwrite (instead of or-ing) the last byte */
    if(i&1)
        it++;
    for(; it < rend; it++) {
        *it = 0;
    }
}

template<typename Arr, typename Str>
auto from_hex(const Str& str) {
    /* >tfw don't know how to write readable sepples */
    auto s = str.size();
    auto ns = s / 2 + (s & 1);
    auto a = Arr(ns);
    u32 i = 0;
    auto it = a.begin();
    if(s & 1)
        *(it++) = pval(str[i++]);
    for(; it != a.end(); it++, i+=2) {
        *it = (pval(str[i]) << 4) | pval(str[i+1]);
    }
    return a;
}
}

namespace io {
    int get_y_n() {
        /* this is pretty shit, but no idea how to do this in a simple way */
        char c;
        std::cin >> c;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (std::cin.gcount() > 1)
            return -1;
        else if (c == 'y' || c == 'Y')
            return 1;
        else if (c == 'n' || c == 'N')
            return 0;
        else
            return -1;
    }
}
