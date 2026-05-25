// Author: Antea Knezović
// Updated by: Ivan Džankić
// Class: LDCF 
// Description:
//      Implements the Logarithmic Dynamic Cuckoo Filter, a multi-level
//      probabilistic structure that grows dynamically as more elements are inserted.
//      Each level is a standalone Cuckoo Filter with increasing capacity.
//      Based on the algorithm described by Zhang et al., 2021.
#pragma once
#include <vector>
#include <cstdint>
#include "level.hpp"

class LDCF {
public:
    // Constructs and LCDF with an initial base level size (in buckets).
    // Default: 2 ^10 buckets.
    explicit LDCF(std::size_t base_buckets = 1 << 10);

    // Inserts a 64-bit element into the LDCF.
    bool insert(std::uint64_t x);
    bool contains(std::uint64_t x) const;
    bool remove(std::uint64_t x);

    // Returns the number of active levels.
    std::size_t num_levels() const { return levels_.size(); }

    // Returns total number of buckets across all levels.
    std::size_t total_buckets() const;

    // Returns an estimated memory footprint in bytes.
    std::size_t estimated_memory_bytes() const;

private:
    std::vector<Level> levels_; // All levels of the LDCF (level 0 = newest)

    // Adds a new level if the current highest level becomes too full.
    void maybe_grow(); 
};
