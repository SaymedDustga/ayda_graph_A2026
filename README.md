# Graph Algorithms & Data Structures - Assignment Base

Welcome to the Graph Algorithms practical assignment!

In this module, you will move beyond theory and implement some of the most important graph algorithms. We have provided a production-ready Graph data structure for you. Your goal is to bring it to life by implementing the missing algorithms.

## The Goal

You do not need to build a graph from scratch. The core architecture—including nodes, arcs, dynamic weights, and graph builders—are already provided.

Your task is to open the **graph_algorithms** file for your assigned language and replace all the `TODO` markers (which currently throw "Not Implemented" errors) with working code.

---

## Repository Structure

This repository supports three languages. Choose the one you prefer:

```text
├── cpp/                   # C++17 Implementation
│   ├── Makefile           # Build configuration
│   ├── main_tests.cpp     # Simple tests
│   ├── examples.cpp       # Graph usage tutorial
│   └── lib/
│       ├── graph.hpp          # Core data structures
│       ├── graph_builders.hpp # Topology builders
│       └── graph_algorithms.hpp ⬅️ YOUR WORK GOES HERE
│
├── python/                # Python 3.8+ Implementation
│   ├── graph.py           # Core data structures
│   ├── graph_builders.py  # Topology builders
│   ├── main_tests.py      # Simple tests
│   ├── examples.py        # Graph usage tutorial
│   └── graph_algorithms.py   ⬅️ YOUR WORK GOES HERE
│
└── rust/                  # Rust 2024 Implementation
    ├── Cargo.toml         # Dependencies
    └── src/
        ├── graph.rs           # Core data structures
        ├── graph_builders.rs  # Topology builders
        ├── main_tests.rs      # Simple tests
        └── graph_algorithms.rs ⬅️ YOUR WORK GOES HERE