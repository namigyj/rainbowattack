#include "types.hpp"
#include "utils.hpp"
#include "config.h"

namespace rb {
    using namespace def;
    template<size_t Hd, size_t Tl>
    struct RowPair<Hd,Tl>::row_pair* RowPair<Hd,Tl>::data() noexcept const {
        return &rp;
    }
    template<size_t Hd, size_t Tl>
    std::array<u8, Hd>& RowPair<Hd, Tl>::head() {
        return rp.head;
    }

    template<size_t Hd, size_t Tl>
    std::array<u8, Tl>& RowPair<Hd, Tl>::tail() {
        return rp.tail;
    }

    template<size_t Hd, size_t Tl>
    RowPair<Hd,Tl>::RowPair(const std::string& hd) {
        rp.head = rb::to_array<u8, Hd>(hd);
    }

    template<size_t Hd, size_t Tl>
    RowPair<Hd,Tl>::RowPair(const std::array<u8, Hd>& hd) {
        rp.head = hd;
    }

    template<size_t Hd, size_t Tl>
    RowPair<Hd,Tl>::RowPair(const std::array<u8, Hd>& hd
                           ,const std::array<u8, Tl>& tl) {
        rp.head = hd;
        rp.tail = tl;
    }

    template<size_t Hd, size_t Tl>
    size_t RowPair<Hd,Tl>::size() noexcept const {
        return Hd+Tl;
    }

    /* https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl */
    template class RowPair<hd_l, tl_l>;
}
