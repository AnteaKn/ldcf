// Author: Antea Knezović
#include "genome.hpp"
#include <fstream>
#include <sstream>

// Loads a genome sequence from a FASTA file at the given path.
// Skips all header lines and concatenates the remaining 
// sequence lines into a single continuous string.
std::string load_genome_fasta(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        return "";
    }
    std::stringstream ss;
    std::string line;

    // Skip FASTA header lines and accumulate sequence lines.
    while (std::getline(in, line)) {
        if (!line.empty() && line[0] == '>') continue;  // header line
        ss << line; // append sequence
    }
    return ss.str();
}
