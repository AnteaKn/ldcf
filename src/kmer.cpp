// Author: Antea Knezović
// Implementation of k-mer extraction utilities.
#include "kmer.hpp"

// Encodes a DNA base into 2 bits.
// Unknown characters default to 0 (treated as 'A').
static inline std::uint64_t encode_base(char c) {
    switch (c) {
        case 'A': case 'a': return 0;
        case 'C': case 'c': return 1;
        case 'G': case 'g': return 2;
        case 'T': case 't': return 3;
        default: return 0;
    }
}

// Extracts all k-mers from the sequence using a rolling 2-bit encoding.
// Produces one encoded k-mer for every position i > k-1.
std::vector<std::uint64_t> extract_kmers(const std::string& seq, std::size_t k) {
    std::vector<std::uint64_t> kmers;
    if (seq.size() < k) return kmers;   // no k-mers possible

    // Mask to keep only the last 2*k bits of the rolling hash.
    // For k >= 32, 2*k exceeds 64 bits -> use full mask.
    std::uint64_t mask = (k >= 32) ? ~0ULL : ((1ULL << (2 * k)) - 1);
    std::uint64_t cur = 0;  // rolling k-mer value

    for (std::size_t i = 0; i < seq.size(); ++i) {
        // Shift left by 2 bits and append encoded base.
        cur = ((cur << 2) | encode_base(seq[i])) & mask;
        // Once we have processed at least k characters, emit a k-mer.
        if (i + 1 >= k) {
            kmers.push_back(cur);
        }
    }
    return kmers;
}
