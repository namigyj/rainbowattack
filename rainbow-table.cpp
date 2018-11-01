#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <random>

// #define NDEBUG
#include <cassert>

#include "types.hpp"
#include "utils.hpp"
#include "config.h"

#include "concurrentqueue.h"

using u8 = unsigned char;

auto compute_line(const std::array<u8, hd_l>& head, int max_col) {
    auto r = std::array<u8, tl_l>();
    auto h = head;
    r = rb::hash(r, h);
    int i = 0;
    while (i < max_col) {
        r = rb::hash(r, rb::reduce(h, r, i));
        i++;
    }
    return std::make_unique<rb::RowPair<hd_l, tl_l>>(head, r);
}


std::mutex display_g;
template< typename Q, typename F >
void th_work(Q& chan, F& next, int max_lines, int max_col) {
    std::unique_ptr<rb::RowPair<hd_l, tl_l>> res;
    auto id = std::this_thread::get_id();

    for(int i = 0; i < max_lines; i++) {
        if(max_lines > 1000 && !(i % 1000)) {
            display_g.lock();
            std::cout << id << ": " << i << std::endl;
            display_g.unlock();
        }
        res = compute_line(next(), max_col);
        chan.enqueue(std::move(res));
        /* note: we could also allocate res in here and
         * pass the reference to results
         */
    }

    display_g.lock();
    std::cout << id << ": finished work" << std::endl;
    display_g.unlock();
}

template< typename Q>
void th_writer(Q& chan, std::ofstream& f, int max_lines) {
    using microseconds = std::chrono::microseconds;
    std::unique_ptr<rb::RowPair<hd_l, tl_l>> r;
    int i = 0;

    do {
        if(!chan.try_dequeue(r)) {
            std::this_thread::sleep_for(microseconds(50));
            continue;
        }

        f.write((char *) r.get()->data(), r.get()->size());
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

int helper(const int i, const int n, const int c) {
    /* refactor */
    return std::floor(n / std::pow(c, i));
}

std::mutex next_g;
template<size_t N>
auto mk_next_chars() {
    const size_t cslen = rb::length(charset);
    auto n_ = std::make_unique<size_t>();

    return [n{move(n_)}] {
               size_t m;
               next_g.lock();
               m = (* n.get())++;
               next_g.unlock();
               /* this is shit but I don't have any better ideas */
               std::array <u8, N> r;
               for(size_t i = 0; i < N; i++) {
                   int j = helper(i, m, cslen) % cslen;
                   r[i] = charset[j % cslen];
               }
               return r;
           };
}

template<size_t N>
auto mk_next_rand() {
    std::random_device rd;
    auto gen = std::make_unique<std::mt19937>(rd());
    auto distr = std::make_unique<std::uniform_int_distribution<int>>(
        0,
        rb::length(charset));

    return [g{move(gen)}, d{move(distr)}](){
               /* TODO : buid array form charset */
               //(* d.get())(* g.get());
               return std::array<u8, N>();
           };
}

int main() {
    /* TODO : filename as argument */
    /* TODO : number of threads as argument, [lines/columns too ?] */
    file_exit("foo.bin");

    auto thcount = th_n;
    auto q = moodycamel::ConcurrentQueue<std::unique_ptr<rb::RowPair<hd_l, tl_l>>>();
    auto n = mk_next_chars<hd_l>();
    using chan_t = decltype(q);

    auto worker1= std::thread(th_work<chan_t, decltype(n)>,
                              std::ref(q), std::ref(n), row_n/2, col_n);
    auto worker2= std::thread(th_work<chan_t, decltype(n)>,
                              std::ref(q), std::ref(n), row_n/2, col_n);
    auto o = std::ofstream("foo.bin", std::ios::binary|std::ios::app);

    auto writer = std::thread(th_writer<chan_t>, std::ref(q), std::ref(o), row_n);
    worker1.join();
    worker2.join();
    writer.join();
}
