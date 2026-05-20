#include "bucket.hpp"

bool Bucket::insert(uint16_t fp) {
    // umetni fingerprint u prvi slobodan slot
    for (auto& s : slots) {
        if (s == 0) {
            s = fp;
            return true;
        }
    }
    return false;   // bucket pun
}

bool Bucket::contains(uint16_t fp) const {
    for (auto s : slots) {
        if (s == fp) {
            return true;
        }
    }
    return false;   
}


bool Bucket::remove(uint16_t fp) {
    // ukloni fingerprint ako postoji
    for (auto& s : slots) {
        if (s == fp) {
            s = 0;
            return true;
        }
    }
    return false;   
}

bool Bucket::kickout(uint16_t& victim_fp) {
    // izbaci prvi fingerprint
    victim_fp = slots[0];
    return victim_fp != 0;
}