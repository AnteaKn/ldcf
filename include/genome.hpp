// Author: Antea Knezović
// Description:
//      Loads a DNA sequence from a FASTA file. Header lines starting with '>'
//      are skipped, and all sequence lines are concatenated into a single string.
#pragma once    // Prevents multiple inclusion
#include <string>

// Returns a continuous DNA string with all header lines removed.
std::string load_genome_fasta(const std::string& path);
