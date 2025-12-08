PATHFINDER – Advanced Graph Pathfinding System

Description:
This repository contains multiple iterations of PATHFINDER, an advanced C-based graph pathfinding system developed as an open-ended lab project for the CSE 1302: Data Structures Lab course (Fall 2025, Section 02) at the University of Liberal Arts Bangladesh (ULAB). PATHFINDER demonstrates practical applications of graph data structures, dynamic node and edge management, and pathfinding algorithms such as BFS (for unweighted graphs) and Dijkstra (for weighted graphs). The repository includes all C source files for versions 1 through 6 of the project and showcases the evolution from static, hardcoded graphs to fully dynamic, user-editable city networks with human-readable navigation instructions.

Table of Contents

Project Overview

Features

Data Structures

Algorithms

Implementation

Usage

Future Improvements

References

Project Overview

PATHFINDER is designed to simulate a city-like graph where nodes represent blocks or waypoints and edges represent streets or paths connecting them. The system supports:

Dynamic addition and removal of nodes and edges.

Both weighted and unweighted modes.

Bidirectional and unidirectional edges.

Step-by-step human-readable navigation instructions.

ASCII-based visualization for user convenience.

The project evolves across six iterations, from a basic 3×3 hardcoded grid to a fully dynamic, scalable navigation system.

Features

Dynamic Graph Management: Add, delete, and modify nodes and edges at runtime.

Optimal Pathfinding: BFS for shortest paths (unweighted) and Dijkstra for minimum-cost paths (weighted).

Human-Readable Navigation: Stores street names and distances for step-by-step guidance.

ASCII Visualization: Provides a quick text-based overview of the graph for verification.

Menu-Driven Interface: Fully designed to close menus after each selection for smooth user experience.

Data Structures

Node: Represents a city block or waypoint. Contains name and active status.

Edge: Represents a connection between nodes. Contains source, destination, weight, bidirectional flag, and unit.

Graph: Contains arrays of nodes and edges, adjacency matrix for weights, adjacency path for street names, and metadata such as node count, edge count, and weighted/unweighted mode.

Justification:
Adjacency matrices were chosen for simplicity, O(1) edge lookup, and ease of BFS/Dijkstra implementation, while Node and Edge arrays support dynamic insertion and deletion.

Algorithms

Breadth-First Search (BFS) – For unweighted graphs. Guarantees the shortest path in terms of number of edges.

Dijkstra’s Algorithm – For weighted graphs. Ensures minimum-cost paths for any positive edge weights.

Both algorithms utilize a parent array for reconstructing human-readable paths.

Implementation

All versions are implemented in C.

Key functionality includes graph initialization, node/edge management, pathfinding setup, and ASCII visualization.

Screenshots of execution can be found in the Input/Output Screenshots
 section of the repository.

Usage

Clone the repository:

git clone <repository-url>


Open the .c files in your preferred C IDE (Code::Blocks recommended).

Compile and run to explore dynamic graph pathfinding and visualization.

Future Improvements

GUI or interactive visualization to handle larger graphs.

Incorporation of heuristic-based algorithms for faster navigation in spatial graphs.

Enhanced user interaction and error handling.

References

Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C., Introduction to Algorithms, 3rd Edition, MIT Press, 2009.

Weiss, M. A., Data Structures and Algorithm Analysis in C, 4th Edition, Pearson, 2013.

Lab manual, CSE 1302: Data Structures Lab, University of Liberal Arts Bangladesh, Fall 2025.
