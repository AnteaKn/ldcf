#pragma once
#include <vector>
#include <cstdint>
#include "level.hpp"

class LDCF {
public:
    explicit LDCF(std::size_t base_buckets = 1 << 10);

    bool insert(std::uint64_t x);
    bool contains(std::uint64_t x) const;
    bool remove(std::uint64_t x);

    std::size_t num_levels() const { return levels_.size(); }

private:
    std::vector<Level> levels_; // sve razine

    void maybe_grow(); // dodaj novu razinu ako treba
};
