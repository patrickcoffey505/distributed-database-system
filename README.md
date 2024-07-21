# distributed-database-system

This library includes a simple key-value store library implemented in C, along with a benchmarking tool to measure read and write performance. It also provides a Python script to visualize the benchmark results.

Benchmark results can be found [here](analysis/README.md#benchmark-results)

## Directory Structure

```plaintext
distributed-database-system/
├── analysis/
│   ├── graph-benchmark-data.py
|   └── README.md
├── benchmark/
│   └── main.c
├── lib/
│   └── kvstore/
│       ├── src/
│       |   ├── kvstore.c
│       |   └── kvstore.h
│       └── Makefile
├── Makefile
└── README.md
```

## Getting Started

### Prerequisites
- GCC (GNU Compiler Collection)
- Make utility

### Building the Project
1. Navigate to the root directory
2. Run the following command: `make`

### Running the Benchmarks
After building the project:
1. Navigate to the root directory
2. Run the executable: `./bin/benchmark`

### Cleaning the Project
To remove all executables:
1. Navigate to the root directory
2. Run the following command: `make clean`