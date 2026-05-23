// Author: Antea Knezović
// Updated by: Ivan Džankić
// Class: Level
// Description:
//      Represents a single level of the LDCF.
//      Each level is an indepent Cuckoo Filter consisting of multiple buckets.
#pragma once
#include <vector>
#include "bucket.hpp"

class Level {
    public:
        explicit Level(std::size_t num_buckets);

        // Inserts an element into this level.
        // Returns true if insertion succeeds (directly or via cuckoo kicks).
        bool insert(std::uint64_t x);   
        // Returns true if found in either candidate bucket.
        bool contains(std::uint64_t x) const;   
        bool remove(std::uint64_t x);

        // Returns the number of buckets in this level.
        std::size_t size() const { return buckets_.size(); }

    private:
        std::vector<Bucket> buckets_;   // All buckets in this level

        // max_kicks limits the number of allowed evictions.
        bool insert_with_kicks(std::uint64_t x, int max_kicks = 500);

        // Computes the alternate bucket index for a fingerprint.
        std::size_t alt_index(std::size_t index, std::uint16_t fp) const;
};