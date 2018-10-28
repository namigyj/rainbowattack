#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>

#include <cryptopp/sha.h>

// #define NDEBUG
#include "types.hpp"
#include "utils.hpp"
#include "config.h"

#include "concurrentqueue.h"

using u8 = unsigned char;

const int th_n = 1;
const int row_n = 1;
const int col_n = 5;

template <typename Digest, typename T>
auto& hash(Digest& o, T pt) {
    CryptoPP::SHA256().CalculateDigest(o.data(), pt.data(), pt.size());
    return o;
}

template <typename T, size_t N>
auto& reduce(T& o, std::array<u8, N> digest, int n) {
    size_t r = n, k = 0, l = rb::length(charset);
    size_t i = 0;
    for(auto it = o.begin(); it != o.end(); it++) {
        /* r := n + d[i+0] + d[i+8] + d[i+16] + ...
         * but constrainted to the size of digest
         */
        for(size_t j = 0; j < (N/8)+1; j++) {
            k = i + 8*j;
            r += digest[k%N];
        }
        *it = charset[r % l];
        i++;
    }
    return o;
}

auto compute_line(const std::array<u8, hd_l>& head, int max_col) {
    auto r = std::array<u8, tl_l>();
    auto h = head;
    r = hash(r, h);
    int i = 0;
    while (i < max_col) {
        r = hash(r, reduce(h, r, i));
        i++;
    }
    return std::make_unique<rb::RowPair<hd_l, tl_l>>(head, r);
}

std::mutex display_g;

template< typename Q, typename F >
void th_work(Q& chan, F next, int max_lines, int max_col) {
    int i = 0;
    std::unique_ptr<rb::RowPair<hd_l, tl_l>> res;
    while(i < max_lines) {
        res = compute_line(next(), max_col);
        chan.enqueue(std::move(res));
        /* note: we could also allocate res in here and
         * pass the reference to results
         */
    }
    auto id = std::this_thread::get_id();
    display_g.lock();
    std::cout << id << ": finished work" << std::endl;
    display_g.unlock();
}

template< typename Q>
void th_writer(Q& chan, std::ofstream& f, int max_lines) {
    using microseconds = std::chrono::microseconds;
    rb::RowPair<hd_l, tl_l> r;
    int i = 1;

    do {
        if(!chan.try_dequeue(r)) {
            std::this_thread::sleep_for(microseconds(50));
            continue;
        }

        f.write((char *) r.data(), r.size());
        i++;
    } while( i < max_lines);
}


void file_exit(const std::string& filepath) {
    if(std::filesystem::exists(filepath)) {
        int r = -1;
        do {
            std::cout << "Detected existing file. Overwrite it (y/n)? ";
            r = io::get_y_n();
        } while (r < 0);

        if(!r)
            exit(0);

        auto o = std::ofstream(filepath, std::ios::binary);
        o.close();
        std::cout << "file deleted" << std::endl;
    }
}

int main() {
    /* TODO : filename as argument */
    /* TODO : number of threads as argument */
    std::cout << 1 + 2 * 3 << std::endl;

    file_exit("foo.bin");

    using chan_t = moodycamel::ConcurrentQueue<rb::RowPair<hd_l, tl_l>>;
    auto q = chan_t();
    auto o = std::ofstream("foo.bin", std::ios::binary|std::ios::app);
    //auto writer = std::thread(th_writer<chan_t>, std::ref(q), std::ref(o), 4);
}
