#include <iostream>
#include <string>
#include <map>

#include "types.hpp"


/* @FUCK how to parallellize the lookup?
 * obvious idea would be to chunk the reduction functions.
 * but that's not possible, because the hash in the table 
 * is computed with the last reduction funciton...
 */

std::unordered_map<hash_t, pass_t> map;

pass_t get_pass(const pass_t& head, const hash_t& hash) {
  auto p = head;
  auto h = hash_t();
  for(size_t i = 0; i < def::col_n || rb::hash(h, p) != hash; i++) {
    rb::reduce(p, h, i);
  }
  return p;
}

pass_t find(hash_t hash) {
  std::unordered_map<hash_t, pass_t> map;
  std::unordered_map<hash_t, pass_t>::iterator it;
  pass_t p;

  for(size_t i = def::col_n - 1; i >= 0; i--) {
    if(it = map.find(hash); it != map.end())
      return get_pass(it->second, hash);
    rb::hash(hash, rb::reduce(p, hash, i));
  }

}

void usage(char *name) {
    std::cout << "USAGE: \n";
    std::cout << name << " [FILENAME] HASH\n";
    std::cout << "\nHASH: hexadecimal\n";
    std::cout << "default FILENAME: 'foo.bin'\n";
    std::exit(EXIT_FAILURE);
}

hash_t parse_hash(const std::string& hash) {
  return rb::from_hex<hash_t>(hash);
}

std::string to_string(const pass_t& pass) {
  return std::string(pass.begin(), pass.end());
}

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& o, const std::array<T, N>& arr) {
  for (auto it : arr)
    o << std::hex << (int)it;
  return o;
}

int main (int argc, char **argv) {
  std::string filename = def::filename;
  std::string hash;
  if (argc == 3) {
    filename = std::string(argv[1]);
    hash = std::string(argv[2]);
  } else if (argc == 2) {
    hash = std::string(argv[1]);
  } else {
    usage(argv[0]);
  }

  // @TODO fill map

  std::cout << to_string(find(parse_hash(hash))) << std::endl;
  return EXIT_SUCCESS;
}
