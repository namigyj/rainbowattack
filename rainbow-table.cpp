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
    const int div = 500;

    for(int i = 0; i < max_lines; i++) {
        if(max_lines > div && i % div == 0){
            display_g.lock();
            std::cout << id << ": " << (((float)i) / 1000) << "/" << (max_lines/1000) << "K" << std::endl;
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

    /* FIXME : this will loop forever if one thread crashed */
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
    file_exit(filename);

    auto q = moodycamel::ConcurrentQueue<std::unique_ptr<rb::RowPair<hd_l, tl_l>>>();
    auto n = mk_next_chars<hd_l>();
    using chan_t = decltype(q);


    auto th_vec = std::vector<std::thread>(th_n);
    for(auto it = th_vec.begin(); it != th_vec.end(); it++) {
        int r = row_n / th_n;
        if(it == th_vec.begin())
            r += row_n % th_n;

        (* it) = std::thread(th_work<chan_t, decltype(n)>,
                             std::ref(q), std::ref(n), r,  col_n);
    }

    auto o = std::ofstream(filename, std::ios::binary | std::ios::app);
    auto writer = std::thread(th_writer<chan_t>, std::ref(q), std::ref(o), row_n);

    for(auto& th : th_vec) {
        th.join();
    }
    writer.join();
}
