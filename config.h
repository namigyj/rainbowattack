#pragma once
#include <cstddef>
#include <cstdint>

namespace def {
    static const std::string filename = "foo.bin";
    static const uint th_n = 4;
    static const size_t hd_l = 8;  // password length
    static const size_t tl_l = 32; // sha255 length
    static const uint row_n = 1000000;
    static const uint col_n = 50000;
    static const char charset[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "-_";
} // namespace defaults
