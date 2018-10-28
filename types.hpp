#include <array>
//#include <iostream>
#include "utils.hpp"

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
}
