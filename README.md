# distributed-database-system

This project creates a server protocol and basic server that stores a simple key-value database implemented in C. It also includes a client library and a client that can connect to one or multiple servers with a replication parameter for distributing data across servers.

The key-value library has benchmarks for read and write times (currently broken). The client/server model is also benchmarked for read/writes on replicated data. The benchmarking analysis is implemented in python.

## Directory Structure

```plaintext
distributed-database-system/
├── benchmark/
│   ├── benchmark.c
│   ├── graph-benchmarks.py
│   └── Makefile
├── lib/
│   └── kvstore/
│       ├── benchmark/
│       |   ├── benchmark.c
│       |   └── graph-benchmark-data.py
│       ├── src/
│       |   ├── kvstore.c
│       |   └── kvstore.h
│       ├── Makefile
│       └── README.md
├── log/
├── src/
│   ├── client/
│   |   ├── client_lib.c
│   |   ├── client_lib.h
│   |   ├── client.c
│   |   └── Makefile
│   └── server/
│       ├── Makefile
│       ├── protocol.c
│       ├── protocol.h
│       └── server.c
├── Makefile
└── README.md
```

## Getting Started

### Prerequisites
- GCC (GNU Compiler Collection)
- Make utility
- xxhash (installed via homebrew on Mac into /opt/homebrew/Cellar/xxhash/0.8.2/)

### Building the Project
1. Navigate to the root directory
2. Run the following command: `make`

### Running the Server
After building the project:
1. Navigate to the root directory
2. Run the executable: `./bin/server <ip> <port>`

### Running the Client
With the server running:
1. Navigate to the root directory
2. Run the executable: `./bin/client <replication_factor> <server_ip1> <server_port1> [<server_ip2> <server_port2> ...]`

### Cleaning the Project
To remove all executables:
1. Navigate to the root directory
2. Run the following command: `make clean`

## Running the Benchmarks

### Prerequisites
- Python 3 with pip
- matplotlib: `pip install matplotlib`

With the server running:
1. Navigate to the root directory
2. Run the executable: `./bin/benchmark <replication_factor> <server_ip1> <server_port1> [<server_ip2> <server_port2> ...]`
3. Upon completion, run `python benchmark/graph-benchmarks.py`

Note: You may have to make some tweaks to the benchmark/stats.json file. Verify that it exists, and the content is formatted like:
```
[
    {
        "num_servers": ...
    },
    {
        "num_servers": ...
    }
]
```

### Benchmark Results

![read/write time vs. number of servers (full replication)](benchmark/graph-benchmarks.png)