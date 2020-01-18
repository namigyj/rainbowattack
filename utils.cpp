#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <limits>

#define NDEBUG // comment to enable assert
#include <cassert>

#include "utils.hpp"

namespace rb {
  void usage(const char * exename) {
    std::cout << "USAGE: \n";
    std::cout << exename << " [-j<number>] [FILE]\n";
    std::cout << "\ndefault: -j4 foo.bin\n";
    std::exit(EXIT_FAILURE);
  }

  args parse_args(int argc, char **argv) {
    args a;
    for(auto i = 1; i < argc; i++) {
      // -j<...>
      if (argv[i][0] == '-' && argv[i][1] == 'j') {
        // -j
        if (!argv[i][2]) {
          a.th_n = std::thread::hardware_concurrency();
        } else {
          // -j<something>
          try {
            a.th_n = std::stoi(std::string(&argv[i][2]));
          } catch (...) {
            // -j[!($ | <number>)]
            std::cout << "ERROR: could not parse '" << &argv[i][2] << "'\n";
            usage(argv[0]);
          }
        }
      } else if (i + 1 == argc) {
        // last arg
        a.fname = std::string(argv[i]);
      } else {
        usage(argv[0]);
      }
    }
    return a;
  }

  template<typename It>
  void print_hex(const It& begin, const It& end) {
    for(auto it = begin; it != end; it++)
      printf("%02hhs", *it);
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
