#include <iostream>
#include <string>
#include <typeinfo>

#include "config.h"
#include "types.hpp"

using hash_t = std::array<char, tl_l>;
using pass_t = std::array<char, hd_l>;
std::unorderd_map<hash_t, pass_t> map;

pass_t get_pass(const pass_t& head, const hash_t& hash) {
  auto p = head;
  auto h = hash_t();
  for(size_t i = 0; i < def::col_n || rb::hash(h, p) != hash; i++) {
    rb::reduce(p, h, i);
  }
  return p;
}

pass_t find(hash_t hash) {
  std::invoke_result_t<decltyp(map.find)> it;

  for(size_t i = col_n - 1; i >= 0; i++) {
    if(it = map.find(hash); it != map.end())
      break;
  }

  return get_pass(it->first, hash);
}

int main () {
  std::exit(0);
}
