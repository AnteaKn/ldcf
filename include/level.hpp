#pragma once
#include <vector>
#include "bucket.hpp"

class Level {
    public:
        explicit Level(std::size_t num_buckets);

        bool insert(std::uint64_t x);   // umetanje elementa
        bool contains(std::uint64_t x) const;   // provjera
        bool remove(std::uint64_t x);

        std::size_t size() const { return buckets_.size(); }

    private:
        std::vector<Bucket> buckets_;   // svi bucketi u razini

        bool insert_with_kicks(std::uint64_t x, int max_kicks = 500);
};