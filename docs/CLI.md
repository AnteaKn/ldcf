# CLI Reference

This file summarizes the command-line options supported by the LDCF runner.

## Basic usage

```
./build/ldcf --input data/ecoli.fasta --mode bench --output results.csv
```

## Options

- `--mode <bench|functional>` selects the run mode (default: `bench`).
- `--input <path>` loads a FASTA file.
- `--generate <length>` generates a random DNA sequence.
- `--k <list>` sets a comma-separated list of k values.
- `--base-buckets <n>` sets the base bucket count.
- `--max-kmers <n>` limits inserted k-mers.
- `--queries <n>` sets the positive query count.
- `--neg-queries <n>` sets the negative query count.
- `--seed <n>` sets the random seed.
- `--output <path>` writes results to a CSV file.
- `--append` appends to an existing CSV file.
- `-h, --help` prints the help message.
