#include "kmer.hpp"

static inline std::uint64_t encode_base(char c) {
    switch (c) {
        case 'A': case 'a': return 0;
        case 'C': case 'c': return 1;
        case 'G': case 'g': return 2;
        case 'T': case 't': return 3;
        default: return 0;
    }
}

std::vector<std::uint64_t> extract_kmers(const std::string& seq, std::size_t k) {
    std::vector<std::uint64_t> kmers;
    if (seq.size() < k) return kmers;

    // maska za 2*k bitova
    std::uint64_t mask = (k >= 32) ? ~0ULL : ((1ULL << (2 * k)) - 1);
    std::uint64_t cur = 0;

    for (std::size_t i = 0; i < seq.size(); ++i) {
        cur = ((cur << 2) | encode_base(seq[i])) & mask;
        if (i + 1 >= k) {
            kmers.push_back(cur);
        }
    }
    return kmers;
}
