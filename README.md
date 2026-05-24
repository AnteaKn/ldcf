# LDCF

Command-line runner for the Logarithmic Dynamic Cuckoo Filter (LDCF) used in the Bioinformatics 1 project.

## Build

```
cmake -S . -B build
cmake --build build
```

On multi-config generators (Visual Studio), add a configuration:

```
cmake --build build --config Release
```

## Run

The program always writes results to a CSV file.

### Benchmark mode (default)

```
./build/ldcf --input data/ecoli.fasta --mode bench --output results.csv
```

### Functional mode

```
./build/ldcf --generate 200000 --mode functional --k 10,20,50 --output functional.csv
```

### Common options

- `--k 10,20,50,100,200` sets the list of k-mer sizes.
- `--base-buckets 1024` sets the base bucket count (rounded up to a power of two).
- `--max-kmers 1000000` limits how many k-mers are inserted.
- `--queries 10000` and `--neg-queries 10000` control query counts.
- `--seed 1` sets the RNG seed.
- `--append` appends to an existing CSV instead of overwriting it.

## Output CSV fields

Each run produces one row per k value with the following columns:

- `mode`, `dataset`, `k`, `sequence_length`, `total_kmers`, `used_kmers`
- `insert_failed`, `missing_inserted`
- `extract_ms`, `insert_ms`, `query_ms`
- `positive_queries`, `positive_hits`, `negative_queries`, `negative_hits`
- `levels`, `total_buckets`, `estimated_bytes`, `base_buckets`, `seed`

## Experiments

See docs/EXPERIMENTS.md for suggested functional and performance runs.

## Notes

- Negative queries are random k-mers and may overlap with inserted k-mers.
- For `k > 32`, k-mers are represented by the lower 64 bits of the encoding.
