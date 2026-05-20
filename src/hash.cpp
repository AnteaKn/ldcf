#include "hash.hpp"

std::uint64_t hash64(std::uint64_t x, std::uint64_t seed) {
    // FNV-like hash
    std::uint64_t h = seed;
    h ^= x;
    h *= 0x100000001b3ULL;
    return h;
}

std::uint16_t fingerprint(std::uint64_t x) {
    // uzimamo donjih 16 bita
    auto h = hash64(x);
    std::uint16_t fp = static_cast<std::uint16_t>(h& 0xFFFFu);
    if (fp == 0) fp = 1;    // fingerprint ne smije biti 0
    return fp;
}

std::size_t index_from_hash(std::uint64_t h, std::uint64_t capacity) {
    return static_cast<std::size_t>(h % capacity);
}