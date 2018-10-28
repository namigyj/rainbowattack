#include <array>
#include <cxxtest/TestSuite.h>
#include "utils.hpp"

// cxxtestgen --error-printer -o runner.cpp test_utils.h
// g++ -o runner -I$CXXTEST runner.cpp

class Test_utils : public CxxTest::TestSuite {
public:
    void test_hexdecode_even(void){
        std::array<u8, 3> a {1, 2, 3};
        auto b = hex_decode("010203");
        TS_ASSRT_EQUALS(a, b);
    }
    void test_hexdecode_odd(void){
        std::array<u8, 3> a {1, 2, 3};
        auto b = hex_decode("10203");
        TS_ASSRT_EQUALS(a, b);
    }
}
