#include <stddef.h>

const size_t hd_l = 8;
const size_t tl_l = 32; // sha256 length

static const char charset[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "-_";
