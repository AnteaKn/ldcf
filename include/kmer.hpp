#pragma once
#include <string>
#include <vector>
#include <cstdint>

std::vector<std::uint64_t> extract_kmers(const std::string& seq, std::size_t k);
