#include "genome.hpp"
#include <fstream>
#include <sstream>

std::string load_genome_fasta(const std::string& path) {
    std::ifstream in(path);
    std::stringstream ss;
    std::string line;

    // Preskoči FASTA header linije
    while (std::getline(in, line)) {
        if (!line.empty() && line[0] == '>') continue;
        ss << line;
    }
    return ss.str();
}
