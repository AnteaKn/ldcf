#include "ldcf.hpp"

LDCF::LDCF(std::size_t base_buckets) {
    // Početna razina
    levels_.emplace_back(base_buckets);
}

bool LDCF::contains(std::uint64_t x) const {
    // Provjeri sve razine
    for (const auto& lvl : levels_) {
        if (lvl.contains(x)) return true;
    }
    return false;
}

bool LDCF::insert(std::uint64_t x) {
    // Pokušaj umetanja u postojeće razine
    for (auto& lvl : levels_) {
        if (lvl.insert(x)) {
            return true;
        }
    }

    // Ako ne stane — dodaj novu razinu
    maybe_grow();
    return levels_.back().insert(x);
}

bool LDCF::remove(std::uint64_t x) {
    for (auto& lvl : levels_) {
        if (lvl.remove(x)) return true;
    }
    return false;
}

void LDCF::maybe_grow() {
    // Nova razina je 2× veća od zadnje
    std::size_t new_size = levels_.back().size() * 2;
    levels_.emplace_back(new_size);
}
