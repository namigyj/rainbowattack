#pragma once

#define NDEBUG // uncomment to disable assert
#include <cassert>

#include <string>
#include <array>
#include <thread>
#include <iostream>

#include "config.h"

using u8 = u_int8_t;
using u32 = u_int32_t;
using uint = unsigned int;

namespace rb {
  struct args {
    uint th_n;
    std::string fname;

    args(uint th_n = def::th_n, std::string fname = def::filename)
      : th_n(th_n), fname(fname) {}
  };

  void usage(const char * exename);

  args parse_args(int argc, char **argv);

  template<size_t N>
  constexpr size_t length(char const (&)[N]){
    return N-1;
  }

  template<typename T, size_t N>
  std::array<T,N> to_array(const std::string& s);

  template<typename T, size_t N>
  inline std::array<T,N> to_array(const std::string& s) {
    assert(s.size() == N);
    std::array<T, N> d;
    std::copy(s.begin(), s.end(), d.begin()); // this is the recommended way
    return d;
  }

  void print_hex(const u8 *buf, size_t len);

  template<typename T >
  std::string to_hex(const T& xs);

  inline u8 pval(u8 c) noexcept {
    if ('a' <= c && c <= 'f')
      return 10 + (c - 'a');
    else if ('0' <= c && c <= '9')
      return c - '0';

    std::cerr << "not a hex character: out of range";
    std::exit(EXIT_FAILURE);
  }

  template<typename rev_it>
  void from_hex(const std::string& str, const rev_it& rbegin, const rev_it& rend) noexcept {
    auto it = rbegin;
    int i = 0; /* counter to know which nibble are in. */
    for(auto is = str.rbegin(); is != str.rend() && it != rend; is++) {
      if(i&1) { /* odd char = Most significat hex */
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


  template<typename T>
  T from_hex(const std::string& str) {
    T r;
    from_hex(str, std::rbegin(r), std::rend(r));
    return r;
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
