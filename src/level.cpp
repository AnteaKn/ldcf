// Author: Antea Knezović
// Updated by: Ivan Džankić
// Each level is a standalone Cuckoo Filter with two candidate bukets
// per element and standard cuckoo-kick insertion.
#include "level.hpp"
#include "hash.hpp"
#include <cstdlib>

// Constructs a level with the given number of buckets.
Level::Level(std::size_t num_buckets)
    : buckets_(num_buckets) {}

// Inserts an element using cuckoo insertion.
// Delegates to insert_with_kicks().
bool Level::insert(std::uint64_t x) {
    return insert_with_kicks(x);
}

// Checks whether the element exists in this level.
// Computes two candidate bucket indices and checks both.
bool Level::contains(std::uint64_t x) const {
    auto fp = fingerprint(x);

    // Two possible bucket indices
    auto h1 = hash64(x);
    auto i1 = index_from_hash(h1, buckets_.size());

    auto i2 = alt_index(i1, fp);

    return buckets_[i1].contains(fp) || buckets_[i2].contains(fp);
}

// Attempts insertion with cuckoo kicks.
// Tries direct insertion into either cadndidate bucket.
// If both are full, repeatedly evicts a victim fingerprint.
bool Level::insert_with_kicks(std::uint64_t x, int max_kicks) {
    auto fp = fingerprint(x);

    // Compute two candidate bucket indices
    auto h1 = hash64(x);
    auto i1 = index_from_hash(h1, buckets_.size());

    auto i2 = alt_index(i1, fp);

    // Try direct insertion
    if (buckets_[i1].insert(fp) || buckets_[i2].insert(fp)) return true;

    // Otherwise perform cuckoo kick
    std::size_t idx = (std::rand() % 2 == 0) ? i1 : i2;
    uint16_t cur_fp = fp;

    for (int k = 0; k < max_kicks; ++k) {
        // Swap the current fingerprint with a victim in this bucket.
        buckets_[idx].kickout(cur_fp);
        // Move the victim to its alternate bucket.
        idx = alt_index(idx, cur_fp);
        if (buckets_[idx].insert(cur_fp)) return true;
    }
    return false;   // insertion failed after max_kicks
}

// Removes the element from this level if present.
// Checks both candidate buckets
bool Level::remove(std::uint64_t x) {
    auto fp = fingerprint(x);

    auto h1 = hash64(x);
    auto i1 = index_from_hash(h1, buckets_.size());

    auto i2 = alt_index(i1, fp);

    return buckets_[i1].remove(fp) || buckets_[i2].remove(fp);
}

// Computes the alternate bucket index for the given fingerprint.
std::size_t Level::alt_index(std::size_t index, std::uint16_t fp) const {
    auto fp_hash = hash64(fp);
    auto fp_index = index_from_hash(fp_hash, buckets_.size());
    return index ^ fp_index;
}