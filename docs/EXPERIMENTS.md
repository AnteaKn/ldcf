# Experiments

This note lists suggested command-line runs for functional checks and performance measurements.

## Functional checks

Run on a small synthetic dataset to verify insertions succeed and that inserted k-mers are found:

```
./build/ldcf --generate 200000 --mode functional --k 10,20,50,100,200 --output functional.csv
```

## Performance benchmarks (synthetic)

Use multiple sequence lengths (example: 1e3 to 1e7). Store one CSV per size or append into one file.

```
./build/ldcf --generate 1000 --mode bench --k 10,20,50,100,200 --output bench_synthetic.csv
./build/ldcf --generate 10000 --mode bench --k 10,20,50,100,200 --output bench_synthetic.csv --append
./build/ldcf --generate 100000 --mode bench --k 10,20,50,100,200 --output bench_synthetic.csv --append
./build/ldcf --generate 1000000 --mode bench --k 10,20,50,100,200 --output bench_synthetic.csv --append
./build/ldcf --generate 10000000 --mode bench --k 10,20,50,100,200 --output bench_synthetic.csv --append
```

## Performance benchmarks (real data)

Use an E. coli FASTA file and store results in a separate CSV:

```
./build/ldcf --input data/ecoli.fasta --mode bench --k 10,20,50,100,200 --output bench_ecoli.csv
```

## Notes

- Use `--max-kmers` to limit memory usage on large datasets.
- For consistent results, keep the same `--seed` across runs.
