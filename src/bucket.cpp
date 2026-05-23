// Author: Antea Knezović
// Updated by: Ivan Džankić
// Implementation of Bucket methods for the Cuckoo Filter

#include "bucket.hpp"
#include <cstdlib>
#include <utility>

// Attempts to insert a fingerprint into the first available empty slot.
// Returns true if insertion suceeds, false if the bucket is full.
bool Bucket::insert(uint16_t fp) {
    // umetni fingerprint u prvi slobodan slot
    for (auto& s : slots) {
        if (s == 0) {   // Empty slot found
            s = fp;
            return true;
        }
    }
    return false;   
}

// Checks whether the bucket contains the given fingerprint.
// Returns true if found.
bool Bucket::contains(uint16_t fp) const {
    for (auto s : slots) {
        if (s == fp) {
            return true;
        }
    }
    return false;   
}

// Removes the given fingerprint if present.
// Returns true if removal succeeds, false if the fingerprint was not found.
bool Bucket::remove(uint16_t fp) {
    for (auto& s : slots) {
        if (s == fp) {
            s = 0;  // Clear the slot
            return true;
        }
    }
    return false;   
}

// Selects a victim fingerprint for cuckoo eviction.
// Swaps the provided fingerprint with a random slot.
bool Bucket::kickout(uint16_t& victim_fp) {
    std::size_t victim_index = static_cast<std::size_t>(std::rand() % BUCKET_SIZE);
    std::swap(victim_fp, slots[victim_index]);
    return true;
}