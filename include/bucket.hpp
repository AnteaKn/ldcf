#pragma once
#include <array>
#include <cstdint>

constexpr std::size_t BUCKET_SIZE = 4;  // broj slotova u bucketu

// jedan bucket u cuckoo filteru
struct Bucket {
    std::array<uint16_t, BUCKET_SIZE> slots{};  // fingerprintovi

    bool insert(uint16_t fp);   // pokusaj umetanja fingerprinta
    bool contains(uint16_t fp) const;   // postoji li fingerprint
    bool remove(uint16_t fp);   
    bool kickout(uint16_t& victim_fp);  // izbacivanje (cuckoo kick)
};