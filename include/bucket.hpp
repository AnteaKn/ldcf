// Author: Antea Knezović
// Updated by: Ivan Džankić
// Class: Bucket
// Description:
//      Based on the standard Cuckoo Filter algorithm (Fan et al., 2014).

#pragma once
#include <array>
#include <cstdint>

// Number of slots per bucket
constexpr std::size_t BUCKET_SIZE = 4;  

// Represents one bucket in the Cuckoo Filter
// Stores up to kBucketSIze 16-bit fingerprints.
struct Bucket {
    std::array<uint16_t, BUCKET_SIZE> slots{};  // Stored fingerprints

    // Implements basic operations
    bool insert(uint16_t fp);   
    bool contains(uint16_t fp) const;   
    bool remove(uint16_t fp);   
    // Swaps a victim fingerprint with the provided one.
    bool kickout(uint16_t& victim_fp);  
};