#include "level.hpp"
#include "hash.hpp"
#include <cstdlib>

Level::Level(std::size_t num_buckets)
    : buckets_(num_buckets) {}

bool Level::contains(std::uint64_t x) const {
    auto fp = fingerprint(x);

    // dva moguca indeksa
    auto h1 = hash64(x);
    auto i1 = index_from_hash(h1, buckets_.size());

    auto h2 = h1 ^ hash64(fp);
    auto i2 = index_from_hash(h2, buckets_.size());

    return buckets_[i1].contains(fp) || buckets_[i2].contains(fp);
}

bool Level::insert(std::uint64_t x) {
    return insert_with_kicks(x);
}

bool Level::insert_with_kicks(std::uint64_t x, int max_kicks) {
    auto fp = fingerprint(x);

    auto h1 = hash64(x);
    auto i1 = index_from_hash(h1, buckets_.size());

    auto h2 = h1 ^ hash64(fp);
    auto i2 = index_from_hash(h2, buckets_.size());

    // pokusaj direktnog umetanja
    if (buckets_[i1].insert(fp) || buckets_[i2].insert(fp)) return true;

    // inače cuckoo kick
    std::size_t idx = (rand() % 2 == 0) ? i1 : i2;
    uint16_t cur_fp = fp;

    for (int k = 0; k < max_kicks; ++k) {
        if (!buckets_[idx].kickout(cur_fp)) return false;
        if (buckets_[idx].insert(cur_fp)) return true;

        auto h = hash64(cur_fp);
        idx = index_from_hash(h, buckets_.size());
    }
    return false;
}

bool Level::remove(std::uint64_t x) {
    auto fp = fingerprint(x);

    auto h1 = hash64(x);
    auto i1 = index_from_hash(h1, buckets_.size());

    auto h2 = h1 ^ hash64(fp);
    auto i2 = index_from_hash(h2, buckets_.size());

    return buckets_[i1].remove(fp) || buckets_[i2].remove(fp);
}