#pragma once
#include <cstdint>
#include <cstddef>

// jednostavan 64-bitni hash
std::uint64_t hash64(std::uint64_t x, std::uint64_t seed = 0xcbf29ce484222325ULL);

// 16-bitni fingerprint
std::uint16_t fingerprint(std::uint64_t x);

// izracun indeksa bucketa
std::size_t index_from_hash(std::uint64_t h, std::size_t capacity);