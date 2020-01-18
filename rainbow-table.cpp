#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <thread>
#include <chrono>    // for queue polling
#include <mutex>     // display lock
#include <algorithm> // std::max
#include <string>    // std::string, std::atoi

// #define NDEBUG
#include <cassert>

#include "types.hpp"
#include "utils.hpp"
#include "config.h"

#include "concurrentqueue.h"

// @SPEEDUP since we only change one thing at a time, in code grey-style, there are max 2 bytes in the
//   array we need to change at the time -> pass array, compute mask (?) and change those?
//   this would save us on the loop
//   ..  but then, it's only called once per line...
auto next_head(uint n) {
    using namespace def;
    pass_t r;
    const size_t cslen = rb::length(charset);
    for(size_t i = 0; i < hd_l; i++) {
        // @SPEEDUP get rid of the cast and the pow? possible using just bitmasks?
        uint j = (uint) (n / std::pow(cslen, i)) % cslen;
        r[i] = charset[j & cslen];
    }
    return r;
}

auto compute_line(const pass_t& head, uint max_col) {
    using namespace def;
    auto tl = hash_t();
    auto h = head;
    rb::hash(tl, h);
    uint i = 0;
    // /!\ tight loop /!\
    // @TODO short-circuit if hash doesn't change?
    do {
        rb::hash(tl, rb::reduce(h, tl, i));
    } while (++i < max_col);
    return std::make_unique<rb::RowPair<hd_l, tl_l>>(head, tl);
}

std::mutex display_g;

template< typename Q>
void th_work(Q& chan, uint start, uint max_lines, uint max_col) {
    using namespace def;
    auto id = std::this_thread::get_id();
    const int div = 500;

    for(size_t i = 0; i < max_lines; i++) {
        if(max_lines > div && i % div == 0){
            display_g.lock();
            std::cout << id << ": " << (((float)i) / 1000) << "/" << (max_lines/1000) << "K" << std::endl;
            display_g.unlock();
        }
        auto res = compute_line(next_head(start++), max_col);
        chan.enqueue(std::move(res));
    }

    display_g.lock();
    std::cout << id << ": finished work" << std::endl;
    display_g.unlock();
}

template< typename Q>
void th_writer(Q& chan, std::ofstream& f, uint max_lines) {
    using namespace def;
    using microseconds = std::chrono::microseconds;
    std::unique_ptr<rb::RowPair<hd_l, tl_l>> r;
    uint i = 0;

    /* FIXME : this will loop forever if one thread crashes */
    do {
        if(!chan.try_dequeue(r)) {
            std::this_thread::sleep_for(microseconds(50));
            continue;
        }

        f.write((char *) r->data(), r->size());
    } while( ++i < max_lines);
}


void file_exist(const std::string& filepath) {
    using namespace def;
    if(std::filesystem::exists(filepath)) {
        int r = -1;
        do {
            std::cout << "file " << filepath << " already exists. Overwrite it (y/n)? ";
            r = io::get_y_n();
        } while (r < 0);

        if (!r) {
            std::cout << "existing";
            exit(EXIT_FAILURE);
        }

        auto o = std::ofstream(filepath, std::ios::binary);
        o.close();
        std::cout << "file deleted" << std::endl;
    }
}

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& o, const std::array<T,N>& arr){
    for(auto it : arr)
        o << it;
    return o;
}

int main(int argc, char **argv) {
    auto a = rb::parse_args(argc, argv);

    std::cout << "filename " << a.fname << '\n';
    std::cout << "jobs " << a.th_n << '\n';

    file_exist(a.fname); // default = foo.bin

    auto q = moodycamel::ConcurrentQueue<std::unique_ptr<rb::RowPair<def::hd_l, def::tl_l>>>();
    using chan_t = decltype(q); // lol

    uint lines = def::row_n / a.th_n;
    uint offset = 0; // first thread takes does remainder
    // @TODO use a thread_pool instead
    auto th_vec = std::vector<std::thread>(a.th_n);
    for(auto it = th_vec.begin(); it != th_vec.end(); it++) {
        uint lines = (def::row_n / a.th_n) + (offset == 0 ? 0 : def::row_n % a.th_n);
        (* it) = std::thread(th_work<chan_t>,
                             std::ref(q), offset, lines, def::col_n);
        offset += lines;
    }

    auto o = std::ofstream(a.fname, std::ios::binary | std::ios::app);
    auto writer = std::thread(th_writer<chan_t>, std::ref(q), std::ref(o), def::row_n);

    for(auto& th : th_vec) {
        th.join();
    }
    writer.join();
    return EXIT_SUCCESS;
}
