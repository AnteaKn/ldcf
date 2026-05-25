// Author: Antea Knezović
// Description:
//      Hash utilities used by the Cuckoo Filter and LDCF implementation.
//      These functions implement standard hashing techniques 
//      commonly used in probabilistic data structures.
#pragma once
#include <cstdint>
#include <cstddef>

std::uint64_t hash64(std::uint64_t x, std::uint64_t seed = 0xcbf29ce484222325ULL);

// Used as the stored indentifier in Cuckoo Filter buckets.
std::uint16_t fingerprint(std::uint64_t x);

std::size_t index_from_hash(std::uint64_t h, std::size_t capacity);