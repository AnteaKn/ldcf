// Author: Antea Knezović
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

    auto h2 = h1 ^ hash64(fp);
    auto i2 = index_from_hash(h2, buckets_.size());

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

    auto h2 = h1 ^ hash64(fp);
    auto i2 = index_from_hash(h2, buckets_.size());

    // Try direct insertion
    if (buckets_[i1].insert(fp) || buckets_[i2].insert(fp)) return true;

    // Otherwise perform cuckoo kick
    std::size_t idx = (rand() % 2 == 0) ? i1 : i2;
    uint16_t cur_fp = fp;

    for (int k = 0; k < max_kicks; ++k) {
        // Evict a victim fingerprint
        if (!buckets_[idx].kickout(cur_fp)) return false;
        // Try inserting the evicted fingerprint
        if (buckets_[idx].insert(cur_fp)) return true;

        // Recompute its alternate bucket
        auto h = hash64(cur_fp);
        idx = index_from_hash(h, buckets_.size());
    }
    return false;   // insertion failed after max_kicks
}

// Removes the element from this level if present.
// Checks both candidate buckets
bool Level::remove(std::uint64_t x) {
    auto fp = fingerprint(x);

    auto h1 = hash64(x);
    auto i1 = index_from_hash(h1, buckets_.size());

    auto h2 = h1 ^ hash64(fp);
    auto i2 = index_from_hash(h2, buckets_.size());

    return buckets_[i1].remove(fp) || buckets_[i2].remove(fp);
}