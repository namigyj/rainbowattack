#pragma once
#include <stddef.h>

static std::string filename("foo.bin");
static int th_n = 4;

const size_t hd_l = 8;  // string length
const size_t tl_l = 32; // sha256 length

const int row_n = 1000000;
const int col_n = 50000;

static const char charset[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "-_";
