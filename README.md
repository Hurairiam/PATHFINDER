# PATHFINDER – Advanced Graph Pathfinding System

**Description:**  
This repository contains multiple iterations of **PATHFINDER**, an advanced C-based graph pathfinding system developed as an open-ended lab project for the **CSE 1302: Data Structures Lab** course (Fall 2025, Section 02) at the University of Liberal Arts Bangladesh (ULAB). PATHFINDER demonstrates practical applications of graph data structures, dynamic node and edge management, and pathfinding algorithms. The repository includes all C source files for versions 1 through 6 of the project and showcases the evolution from static, hardcoded graphs to fully dynamic, user-editable city networks with human-readable navigation instructions.

---

## Table of Contents
1. [Project Overview](#project-overview)  
2. [Features](#features)  
3. [Data Structures](#data-structures)  
4. [Algorithms](#algorithms)  
5. [Implementation](#implementation)  
6. [Usage](#usage)  
7. [Future Improvements](#future-improvements)  
8. [Version History](#version-history)  
9. [References](#references)  

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

## Usage
1. Clone the repository:  
   ```bash
   git clone <repository-url>
Open the project in any C IDE (Code::Blocks recommended).

Compile and run the main C file.

Follow the menu to add nodes, edges, and perform pathfinding operations.

Future Improvements
Add a graphical user interface (GUI) to enhance visualization of large or complex graphs.

Integrate interactive controls for dynamic exploration of the network.

Implement heuristic-based algorithms for faster performance on spatially distributed networks.

Version History
v1–v3: Static or hardcoded graph examples.

v4: Partial dynamic implementation with user input for nodes.

v5: Full dynamic graph with weighted/unweighted edges.

v6: Complete system with bidirectional/unidirectional edges, menu interface, ASCII visualization, and robust pathfinding.

References
Mark Allen Weiss, Data Structures and Algorithm Analysis in C, 4th Edition, Pearson, 2014.

Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest, Clifford Stein, Introduction to Algorithms, 3rd Edition, MIT Press, 2009.

ULAB CSE-1302 Lab Manual, Fall 2025.

Online resources for graph traversal algorithms and adjacency matrix representations.

pgsql
Copy code

I can also create a **version with collapsible sections and better visual Markdown formatting for GitHub** so it looks very professional.  

Do you want me to do that next?
