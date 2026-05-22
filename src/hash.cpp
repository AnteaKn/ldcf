// Author: Antea Knezović
// Implementation of hashing utilities used by the Cuckoo Filter and LDCF.
#include "hash.hpp"

// Computes a simple 64-bit FNV-like hash.
// The seed defaults to the FNV offset basis.
// Returns a 64-bit hashed value.
std::uint64_t hash64(std::uint64_t x, std::uint64_t seed) {
    std::uint64_t h = seed;
    h ^= x; // XOR-mix input with seed
    h *= 0x100000001b3ULL;  // FNV prime multiplication
    return h;
}

// Extracts a 16-bit fingerprint from the 64-bit hash of x.
// Ensures the fingerprint is never zero (0 is reserved as "empty slot").
// Returns a non-zero 16-bit fingerprint.
std::uint16_t fingerprint(std::uint64_t x) {
    auto h = hash64(x);
    std::uint16_t fp = static_cast<std::uint16_t>(h& 0xFFFFu);  // lower 16 bits
    if (fp == 0) fp = 1;    // fingerprint must not be zero
    return fp;
}

// Computes the bucket index from a hash value h.
// The index is in the range [0, capacity).
// Uses modulo operation to map hash to bucket range.
std::size_t index_from_hash(std::uint64_t h, std::uint64_t capacity) {
    return static_cast<std::size_t>(h % capacity);
}