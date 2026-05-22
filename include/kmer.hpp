// Author: Antea Knezović
// Description:
//      Provides k-mer extraction utilities used in the LDCF pipeline.
//      Implements the standard sliding-window k-mer generation technique commonly used in bioinformatics.
#pragma once
#include <string>
#include <vector>
#include <cstdint>

// Extracts all k-mers from the input DNA sequence 'seq'.
// Returns a vector of encoded k-mers in left-to-right order.
std::vector<std::uint64_t> extract_kmers(const std::string& seq, std::size_t k);
