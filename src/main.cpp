// Author: Antea Knezović
// Updated by: Ivan Džankić
// Description:
//      Command-line runner for building and testing the LDCF implementation.
//      Supports benchmarking and functional verification on FASTA or generated data.
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "genome.hpp"
#include "kmer.hpp"
#include "ldcf.hpp"

namespace {

struct RunConfig {
    std::string mode = "bench";
    std::string input_path;
    std::string output_path = "results.csv";
    std::size_t generate_length = 0;
    std::size_t base_buckets = 1 << 10;
    std::size_t max_kmers = 0;
    std::size_t query_count = 10000;
    std::size_t negative_query_count = 10000;
    std::uint64_t seed = 1;
    bool append = false;
    std::vector<std::size_t> k_values = {10, 20, 50, 100, 200};
};

struct RunMetrics {
    std::size_t k = 0;
    std::size_t sequence_length = 0;
    std::size_t total_kmers = 0;
    std::size_t used_kmers = 0;
    std::size_t insert_failed = 0;
    std::size_t missing_inserted = 0;
    std::size_t positive_queries = 0;
    std::size_t positive_hits = 0;
    std::size_t negative_queries = 0;
    std::size_t negative_hits = 0;
    std::size_t levels = 0;
    std::size_t total_buckets = 0;
    std::size_t estimated_bytes = 0;
    double extract_ms = 0.0;
    double insert_ms = 0.0;
    double query_ms = 0.0;
};

struct KmerSet {
    std::vector<std::uint64_t> kmers;
    std::size_t total_kmers = 0;
    double extract_ms = 0.0;
};

double to_ms(const std::chrono::steady_clock::duration& duration) {
    return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(duration).count();
}

void print_usage(const char* exe) {
    std::cout
        << "Usage: " << exe << " [options]\n"
        << "\nOptions:\n"
        << "  --mode <bench|functional>   Run mode (default: bench)\n"
        << "  --input <path>              FASTA input file\n"
        << "  --generate <length>         Generate random DNA sequence\n"
        << "  --k <list>                  Comma-separated k values (default: 10,20,50,100,200)\n"
        << "  --base-buckets <n>          Base bucket count (default: 1024)\n"
        << "  --max-kmers <n>             Limit number of k-mers inserted (default: 0 = all)\n"
        << "  --queries <n>               Positive query count (default: 10000)\n"
        << "  --neg-queries <n>           Negative query count (default: 10000)\n"
        << "  --seed <n>                  RNG seed (default: 1)\n"
        << "  --output <path>             Output CSV file (default: results.csv)\n"
        << "  --append                    Append to existing output file\n"
        << "  -h, --help                  Show this help message\n";
}

bool parse_size_t(const std::string& text, std::size_t& value) {
    try {
        std::size_t pos = 0;
        auto parsed = std::stoull(text, &pos, 10);
        if (pos != text.size()) return false;
        value = static_cast<std::size_t>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

bool parse_uint64(const std::string& text, std::uint64_t& value) {
    try {
        std::size_t pos = 0;
        auto parsed = std::stoull(text, &pos, 10);
        if (pos != text.size()) return false;
        value = static_cast<std::uint64_t>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

bool parse_k_list(const std::string& text, std::vector<std::size_t>& values) {
    std::vector<std::size_t> result;
    std::stringstream ss(text);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (item.empty()) return false;
        std::size_t value = 0;
        if (!parse_size_t(item, value) || value == 0) return false;
        result.push_back(value);
    }
    if (result.empty()) return false;
    values = result;
    return true;
}

bool parse_args(int argc, char** argv, RunConfig& cfg, bool& show_help, std::string& error) {
    show_help = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        auto require_value = [&](const std::string& opt) -> std::string {
            if (i + 1 >= argc) {
                error = "Missing value for " + opt;
                return {};
            }
            return std::string(argv[++i]);
        };

        if (arg == "-h" || arg == "--help") {
            show_help = true;
            return false;
        }
        if (arg == "--mode") {
            cfg.mode = require_value(arg);
            if (cfg.mode != "bench" && cfg.mode != "functional") {
                error = "Unsupported mode: " + cfg.mode;
                return false;
            }
            continue;
        }
        if (arg == "--input") {
            cfg.input_path = require_value(arg);
            continue;
        }
        if (arg == "--generate") {
            auto value = require_value(arg);
            if (!parse_size_t(value, cfg.generate_length) || cfg.generate_length == 0) {
                error = "Invalid --generate value: " + value;
                return false;
            }
            continue;
        }
        if (arg == "--k") {
            auto value = require_value(arg);
            if (!parse_k_list(value, cfg.k_values)) {
                error = "Invalid --k list: " + value;
                return false;
            }
            continue;
        }
        if (arg == "--base-buckets") {
            auto value = require_value(arg);
            if (!parse_size_t(value, cfg.base_buckets) || cfg.base_buckets == 0) {
                error = "Invalid --base-buckets value: " + value;
                return false;
            }
            continue;
        }
        if (arg == "--max-kmers") {
            auto value = require_value(arg);
            if (!parse_size_t(value, cfg.max_kmers)) {
                error = "Invalid --max-kmers value: " + value;
                return false;
            }
            continue;
        }
        if (arg == "--queries") {
            auto value = require_value(arg);
            if (!parse_size_t(value, cfg.query_count)) {
                error = "Invalid --queries value: " + value;
                return false;
            }
            continue;
        }
        if (arg == "--neg-queries") {
            auto value = require_value(arg);
            if (!parse_size_t(value, cfg.negative_query_count)) {
                error = "Invalid --neg-queries value: " + value;
                return false;
            }
            continue;
        }
        if (arg == "--seed") {
            auto value = require_value(arg);
            if (!parse_uint64(value, cfg.seed)) {
                error = "Invalid --seed value: " + value;
                return false;
            }
            continue;
        }
        if (arg == "--output") {
            cfg.output_path = require_value(arg);
            continue;
        }
        if (arg == "--append") {
            cfg.append = true;
            continue;
        }

        error = "Unknown option: " + arg;
        return false;
    }

    if (cfg.input_path.empty() && cfg.generate_length == 0) {
        error = "Either --input or --generate must be provided.";
        return false;
    }
    if (cfg.k_values.empty()) {
        error = "At least one k value must be provided.";
        return false;
    }
    return true;
}

std::string csv_escape(const std::string& value) {
    if (value.find_first_of(",\n\"") == std::string::npos) {
        return value;
    }
    std::string escaped = "\"";
    for (char c : value) {
        if (c == '\"') escaped += "\"\"";
        else escaped += c;
    }
    escaped += "\"";
    return escaped;
}

bool file_exists(const std::string& path) {
    std::ifstream in(path);
    return in.good();
}

std::string generate_random_dna(std::size_t length, std::mt19937_64& rng) {
    std::string seq;
    seq.reserve(length);
    std::uniform_int_distribution<int> dist(0, 3);
    static const char bases[] = {'A', 'C', 'G', 'T'};
    for (std::size_t i = 0; i < length; ++i) {
        seq.push_back(bases[dist(rng)]);
    }
    return seq;
}

std::uint64_t random_kmer_value(std::size_t k, std::mt19937_64& rng) {
    std::size_t effective_k = std::min<std::size_t>(k, 32);
    std::uint64_t value = 0;
    std::uniform_int_distribution<int> dist(0, 3);
    for (std::size_t i = 0; i < effective_k; ++i) {
        value = (value << 2) | static_cast<std::uint64_t>(dist(rng));
    }
    return value;
}

KmerSet prepare_kmers(const std::string& sequence, std::size_t k, const RunConfig& cfg) {
    KmerSet set;
    if (sequence.size() < k || k == 0) return set;

    auto start = std::chrono::steady_clock::now();
    set.kmers = extract_kmers(sequence, k);
    auto end = std::chrono::steady_clock::now();
    set.total_kmers = set.kmers.size();
    set.extract_ms = to_ms(end - start);

    if (cfg.max_kmers > 0 && set.kmers.size() > cfg.max_kmers) {
        set.kmers.resize(cfg.max_kmers);
    }
    return set;
}

RunMetrics run_benchmark(const std::string& sequence, std::size_t k, const RunConfig& cfg,
                         std::mt19937_64& rng) {
    RunMetrics metrics;
    metrics.k = k;
    metrics.sequence_length = sequence.size();

    KmerSet set = prepare_kmers(sequence, k, cfg);
    metrics.total_kmers = set.total_kmers;
    metrics.used_kmers = set.kmers.size();
    metrics.extract_ms = set.extract_ms;

    if (set.kmers.empty()) return metrics;

    LDCF filter(cfg.base_buckets);
    auto insert_start = std::chrono::steady_clock::now();
    for (auto value : set.kmers) {
        if (!filter.insert(value)) {
            metrics.insert_failed++;
        }
    }
    auto insert_end = std::chrono::steady_clock::now();
    metrics.insert_ms = to_ms(insert_end - insert_start);

    metrics.levels = filter.num_levels();
    metrics.total_buckets = filter.total_buckets();
    metrics.estimated_bytes = filter.estimated_memory_bytes();

    auto query_start = std::chrono::steady_clock::now();
    if (cfg.query_count > 0 && !set.kmers.empty()) {
        std::uniform_int_distribution<std::size_t> dist(0, set.kmers.size() - 1);
        std::size_t count = std::min(cfg.query_count, set.kmers.size());
        metrics.positive_queries = count;
        for (std::size_t i = 0; i < count; ++i) {
            auto value = set.kmers[dist(rng)];
            if (filter.contains(value)) {
                metrics.positive_hits++;
            }
        }
    }

    if (cfg.negative_query_count > 0) {
        metrics.negative_queries = cfg.negative_query_count;
        for (std::size_t i = 0; i < cfg.negative_query_count; ++i) {
            auto value = random_kmer_value(k, rng);
            if (filter.contains(value)) {
                metrics.negative_hits++;
            }
        }
    }
    auto query_end = std::chrono::steady_clock::now();
    metrics.query_ms = to_ms(query_end - query_start);

    return metrics;
}

RunMetrics run_functional(const std::string& sequence, std::size_t k, const RunConfig& cfg) {
    RunMetrics metrics;
    metrics.k = k;
    metrics.sequence_length = sequence.size();

    KmerSet set = prepare_kmers(sequence, k, cfg);
    metrics.total_kmers = set.total_kmers;
    metrics.used_kmers = set.kmers.size();
    metrics.extract_ms = set.extract_ms;

    if (set.kmers.empty()) return metrics;

    LDCF filter(cfg.base_buckets);
    auto insert_start = std::chrono::steady_clock::now();
    for (auto value : set.kmers) {
        if (!filter.insert(value)) {
            metrics.insert_failed++;
        }
    }
    auto insert_end = std::chrono::steady_clock::now();
    metrics.insert_ms = to_ms(insert_end - insert_start);

    for (auto value : set.kmers) {
        if (!filter.contains(value)) {
            metrics.missing_inserted++;
        }
    }

    metrics.levels = filter.num_levels();
    metrics.total_buckets = filter.total_buckets();
    metrics.estimated_bytes = filter.estimated_memory_bytes();

    return metrics;
}

void write_csv_header(std::ofstream& out) {
    out << "mode,dataset,k,sequence_length,total_kmers,used_kmers,insert_failed,missing_inserted,"
           "extract_ms,insert_ms,query_ms,positive_queries,positive_hits,negative_queries,"
           "negative_hits,levels,total_buckets,estimated_bytes,base_buckets,seed\n";
}

void write_csv_row(std::ofstream& out, const RunConfig& cfg, const RunMetrics& metrics,
                   const std::string& dataset_label) {
    out << cfg.mode << ','
        << csv_escape(dataset_label) << ','
        << metrics.k << ','
        << metrics.sequence_length << ','
        << metrics.total_kmers << ','
        << metrics.used_kmers << ','
        << metrics.insert_failed << ','
        << metrics.missing_inserted << ','
        << metrics.extract_ms << ','
        << metrics.insert_ms << ','
        << metrics.query_ms << ','
        << metrics.positive_queries << ','
        << metrics.positive_hits << ','
        << metrics.negative_queries << ','
        << metrics.negative_hits << ','
        << metrics.levels << ','
        << metrics.total_buckets << ','
        << metrics.estimated_bytes << ','
        << cfg.base_buckets << ','
        << cfg.seed << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    RunConfig cfg;
    bool show_help = false;
    std::string error;
    if (!parse_args(argc, argv, cfg, show_help, error)) {
        if (show_help) {
            print_usage(argv[0]);
            return 0;
        }
        std::cerr << "Error: " << error << "\n\n";
        print_usage(argv[0]);
        return 1;
    }

    std::srand(static_cast<unsigned>(cfg.seed));
    std::mt19937_64 rng(cfg.seed);

    std::string sequence;
    std::string dataset_label = cfg.input_path.empty() ? "generated" : cfg.input_path;
    if (!cfg.input_path.empty()) {
        sequence = load_genome_fasta(cfg.input_path);
    } else {
        sequence = generate_random_dna(cfg.generate_length, rng);
    }

    if (sequence.empty()) {
        std::cerr << "Error: input sequence is empty.\n";
        return 1;
    }

    bool has_existing = file_exists(cfg.output_path);
    std::ofstream out(cfg.output_path, cfg.append ? std::ios::app : std::ios::trunc);
    if (!out) {
        std::cerr << "Error: cannot open output file: " << cfg.output_path << "\n";
        return 1;
    }
    if (!cfg.append || !has_existing) {
        write_csv_header(out);
    }

    bool functional_ok = true;
    for (auto k : cfg.k_values) {
        RunMetrics metrics = (cfg.mode == "functional")
            ? run_functional(sequence, k, cfg)
            : run_benchmark(sequence, k, cfg, rng);
        write_csv_row(out, cfg, metrics, dataset_label);
        if (cfg.mode == "functional") {
            if (metrics.insert_failed > 0 || metrics.missing_inserted > 0) {
                functional_ok = false;
            }
        }
    }

    std::cout << "Wrote results to " << cfg.output_path << "\n";
    if (cfg.mode == "functional" && !functional_ok) {
        return 2;
    }
    return 0;
}