// Author: Antea Knezović
// Updated by: Ivan Džankić
#include "ldcf.hpp"

namespace {
// Rounds up to the next power of two (minimum 1).
std::size_t next_power_of_two(std::size_t value) {
    if (value == 0) return 1;
    value--;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    if (sizeof(std::size_t) == 8) {
        value |= value >> 32;
    }
    return value + 1;
}
}  // namespace

// Constructs the LDCF with a single initial level of size `base_buckets`.
LDCF::LDCF(std::size_t base_buckets) {
    std::size_t normalized = next_power_of_two(base_buckets);
    levels_.emplace_back(normalized); // create level 0
}

// Inserts an element into the LDCF.
// Tries all existing levels first; if insertion fails everywhere,
// a new level is created and insertion is retired there.
bool LDCF::insert(std::uint64_t x) {
    for (auto& lvl : levels_) {
        if (lvl.insert(x)) {
            return true;    // inserted successfully
        }
    }

    maybe_grow();
    return levels_.back().insert(x);
}

// Checks whether the element exist in any level.
// Returns true if found in at least one level.
bool LDCF::contains(std::uint64_t x) const {
    for (const auto& lvl : levels_) {
        if (lvl.contains(x)) return true;
    }
    return false;
}

// Removes the element from the LDCF if present.
// Returns true if removal succeeds in any level.
bool LDCF::remove(std::uint64_t x) {
    for (auto& lvl : levels_) {
        if (lvl.remove(x)) return true;
    }
    return false;
}

// Adds a new level with double the capacity of the previous one.
// Implements the logarithmic growth rule of LDCF.
void LDCF::maybe_grow() {
    // Nova razina je 2× veća od zadnje
    std::size_t new_size = levels_.back().size() * 2;
    levels_.emplace_back(new_size);
}

// Returns total number of buckets across all levels.
std::size_t LDCF::total_buckets() const {
    std::size_t total = 0;
    for (const auto& level : levels_) {
        total += level.size();
    }
    return total;
}

// Returns an estimated memory footprint in bytes.
std::size_t LDCF::estimated_memory_bytes() const {
    return total_buckets() * sizeof(Bucket);
}
