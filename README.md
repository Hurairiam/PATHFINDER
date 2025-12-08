# PATHFINDER – Advanced Graph Pathfinding System
 
This repository contains multiple iterations of **PATHFINDER**, an advanced C-based graph pathfinding system developed as an open-ended lab project for the **CSE 1302: Data Structures Lab** course (Fall 2025, Section 02) at the University of Liberal Arts Bangladesh (ULAB). PATHFINDER demonstrates practical applications of graph data structures, dynamic node and edge management, and pathfinding algorithms. The repository includes all C source files for versions 1 through 6 of the project and showcases the evolution from static, hardcoded graphs to fully dynamic, user-editable city networks with human-readable navigation instructions.

---

## Project Overview
PATHFINDER simulates a city-like graph where nodes represent blocks or waypoints and edges represent streets or paths connecting them. Users can dynamically add, remove, and modify nodes and edges. Both weighted and unweighted paths are supported, including bidirectional and unidirectional connections. The system also provides human-readable navigation instructions and optional ASCII-based visualization for verification.

---

## Features
- **Dynamic Graph Management:** Add, delete, and modify nodes and edges at runtime.  
- **Optimal Pathfinding:** BFS for shortest paths (unweighted) and Dijkstra for minimum-cost paths (weighted).  
- **Human-Readable Navigation:** Step-by-step street names and distance units.  
- **ASCII Visualization:** Quick text-based overview of the graph for verification, though not required.  
- **Menu-Driven Interface:** Menus automatically close after each selection for smooth user experience.  

---

## Data Structures
- **Node:** Represents a city block or waypoint. Stores the name and active status.  
- **Edge:** Represents a connection between nodes. Stores source, destination, weight, bidirectional flag, and unit.  
- **Graph:** Contains arrays of nodes and edges, adjacency matrix for edge weights, adjacency path array for street names, and metadata such as node count, edge count, and weighted/unweighted mode.  

**Justification:**  
Adjacency matrices were chosen for simplicity, O(1) edge lookup, and ease of implementing BFS and Dijkstra. Node and Edge arrays support dynamic insertion and deletion, balancing performance, usability, and clarity.

---

## Algorithms
PATHFINDER uses two main algorithms for pathfinding:

1. **Breadth-First Search (BFS) – Unweighted Graphs:** Guarantees shortest paths by the number of edges and is efficient for uniform-cost layouts.  
2. **Dijkstra’s Algorithm – Weighted Graphs:** Ensures minimum total distance or cost and supports dynamic weighted edges.  

These algorithms were selected after evaluating alternatives such as DFS, Greedy Best-First Search, Floyd–Warshall, and hybrid heuristic methods, all of which were unsuitable for fully dynamic, arbitrary node-based graphs.

---

## Implementation
- Graph initialization prepares all nodes and edges as inactive.  
- Nodes and edges are dynamically added, deleted, and updated.  
- Parent arrays track paths for reconstruction in both BFS and Dijkstra modes.  
- ASCII visualization provides a console-based overview of nodes and edges.  
- The menu-driven interface allows smooth interaction and ensures each menu closes automatically after use.  

All design, logic, and interface ideas were developed by the author, with AI tools used only for clarification and code refinement.

---

