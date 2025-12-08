#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_NODES 100
#define MAX_EDGES 5000
#define MAX_NAME_LEN 30
#define INF INT_MAX
#define MAX_GRID_SIZE 9

typedef struct {
    char name[MAX_NAME_LEN];
    int id;
} Node;

typedef struct {
    int from;
    int to;
    char pathName[MAX_NAME_LEN];
    int weight;
    bool bidirectional;
} Edge;

typedef struct {
    Node nodes[MAX_NODES];
    Edge edges[MAX_EDGES];
    int adjMatrix[MAX_NODES][MAX_NODES];
    char adjPath[MAX_NODES][MAX_NODES][MAX_NAME_LEN];
    bool adjBidirectional[MAX_NODES][MAX_NODES];
    int nodeCount;
    int edgeCount;
    bool weighted;
    int gridSize;
} Graph;

// Initialize graph
void initializeGraph(Graph *g) {
    g->nodeCount = 0;
    g->edgeCount = 0;
    g->weighted = true;
    g->gridSize = 0;

    for (int i = 0; i < MAX_NODES; i++) {
        g->nodes[i].name[0] = '\0';
        g->nodes[i].id = 0;
        for (int j = 0; j < MAX_NODES; j++) {
            g->adjMatrix[i][j] = 0;
            g->adjPath[i][j][0] = '\0';
            g->adjBidirectional[i][j] = false;
        }
    }
}

// Add node
void addNode(Graph *g, const char *name) {
    if (g->nodeCount >= MAX_NODES) {
        printf("Error: Maximum number of nodes reached!\n");
        return;
    }
    strcpy(g->nodes[g->nodeCount].name, name);
    g->nodes[g->nodeCount].id = g->nodeCount;
    g->nodeCount++;
}

// Find node index by name
int findNodeIndex(Graph *g, const char *name) {
    for (int i = 0; i < g->nodeCount; i++) {
        if (strcmp(g->nodes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Add edge
void addEdge(Graph *g, int from, int to, const char *pathName, int weight, bool bidirectional) {
    if (g->edgeCount >= MAX_EDGES) {
        printf("Error: Maximum number of edges reached!\n");
        return;
    }
    if (from < 0 || from >= g->nodeCount || to < 0 || to >= g->nodeCount) {
        printf("Error: Invalid node indices!\n");
        return;
    }
    
    g->edges[g->edgeCount].from = from;
    g->edges[g->edgeCount].to = to;
    strcpy(g->edges[g->edgeCount].pathName, pathName);
    g->edges[g->edgeCount].weight = weight;
    g->edges[g->edgeCount].bidirectional = bidirectional;
    g->edgeCount++;

    g->adjMatrix[from][to] = weight;
    strcpy(g->adjPath[from][to], pathName);
    g->adjBidirectional[from][to] = bidirectional;

    if (bidirectional) {
        g->adjMatrix[to][from] = weight;
        strcpy(g->adjPath[to][from], pathName);
        g->adjBidirectional[to][from] = true;
    }
}

// Dijkstra's algorithm to find shortest path
void findShortestPath(Graph *g, int start, int end) {
    if (start < 0 || start >= g->nodeCount || end < 0 || end >= g->nodeCount) {
        printf("Error: Invalid node indices!\n");
        return;
    }
    
    if (start == end) {
        printf("Start and end nodes are the same!\n");
        return;
    }
    
    int distance[MAX_NODES];
    bool visited[MAX_NODES];
    int previous[MAX_NODES];
    
    for (int i = 0; i < g->nodeCount; i++) {
        distance[i] = INF;
        visited[i] = false;
        previous[i] = -1;
    }
    
    distance[start] = 0;
    
    for (int count = 0; count < g->nodeCount - 1; count++) {
        int minDist = INF;
        int minIndex = -1;
        
        for (int v = 0; v < g->nodeCount; v++) {
            if (!visited[v] && distance[v] < minDist) {
                minDist = distance[v];
                minIndex = v;
            }
        }
        
        if (minIndex == -1) break;
        
        visited[minIndex] = true;
        
        for (int v = 0; v < g->nodeCount; v++) {
            if (!visited[v] && g->adjMatrix[minIndex][v] != 0) {
                int edgeWeight = g->adjMatrix[minIndex][v];
                if (distance[minIndex] != INF && 
                    distance[minIndex] + edgeWeight < distance[v]) {
                    distance[v] = distance[minIndex] + edgeWeight;
                    previous[v] = minIndex;
                }
            }
        }
    }
    
    if (distance[end] == INF) {
        printf("No path exists from %s to %s\n", 
               g->nodes[start].name, g->nodes[end].name);
        return;
    }
    
    int path[MAX_NODES];
    int pathLength = 0;
    int current = end;
    
    while (current != -1) {
        path[pathLength++] = current;
        current = previous[current];
    }
    
    for (int i = 0; i < pathLength / 2; i++) {
        int temp = path[i];
        path[i] = path[pathLength - 1 - i];
        path[pathLength - 1 - i] = temp;
    }
    
    printf("\nShortest path from %s to %s:\n", 
           g->nodes[start].name, g->nodes[end].name);
    printf("Total distance: %d\n", distance[end]);
    
    printf("Path: ");
    for (int i = 0; i < pathLength; i++) {
        printf("%s", g->nodes[path[i]].name);
        if (i < pathLength - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
    
    printf("Detailed route:\n");
    for (int i = 0; i < pathLength - 1; i++) {
        int from = path[i];
        int to = path[i + 1];
        printf("  %s to %s via %s (distance: %d)\n",
               g->nodes[from].name,
               g->nodes[to].name,
               g->adjPath[from][to],
               g->adjMatrix[from][to]);
    }
}

// Generate node name for grid position
void generateNodeName(int row, int col, int size, char *name) {
    char rowChar = 'A' + row;
    snprintf(name, MAX_NAME_LEN, "%c%d", rowChar, col + 1);
}

// NxN Grid Input System (up to 9x9)
void inputNxNGrid(Graph *g) {
    printf("\n=== NxN Grid Input System (Up to 9x9) ===\n");
    
    // Get grid size from user
    int size;
    do {
        printf("Enter grid size (N) where 2 <= N <= 9: ");
        scanf("%d", &size);
        if (size < 2 || size > MAX_GRID_SIZE) {
            printf("Invalid size! Please enter between 2 and %d.\n", MAX_GRID_SIZE);
        }
    } while (size < 2 || size > MAX_GRID_SIZE);
    
    g->gridSize = size;
    
    printf("\nCreating %dx%d grid with %d nodes...\n", size, size, size * size);
    
    // Create nodes for NxN grid
    printf("\nGrid nodes:\n");
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            char nodeName[MAX_NAME_LEN];
            generateNodeName(row, col, size, nodeName);
            addNode(g, nodeName);
            printf("%s ", nodeName);
        }
        printf("\n");
    }
    
    printf("\n=== Grid Connection Options ===\n");
    printf("1. Automatic random weights\n");
    printf("2. Manual input for all connections\n");
    printf("3. Default constant weights\n");
    
    int option;
    printf("Choose connection option (1-3): ");
    scanf("%d", &option);
    
    // Define all possible connections
    int connectionCount = 0;
    
    // Arrays to store all possible connections
    int fromNodes[size * size * 4]; // Max 4 connections per node
    int toNodes[size * size * 4];
    
    // Generate all possible horizontal connections
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size - 1; col++) {
            int from = row * size + col;
            int to = row * size + (col + 1);
            fromNodes[connectionCount] = from;
            toNodes[connectionCount] = to;
            connectionCount++;
        }
    }
    
    // Generate all possible vertical connections
    for (int col = 0; col < size; col++) {
        for (int row = 0; row < size - 1; row++) {
            int from = row * size + col;
            int to = (row + 1) * size + col;
            fromNodes[connectionCount] = from;
            toNodes[connectionCount] = to;
            connectionCount++;
        }
    }
    
    // Generate diagonal connections if requested
    char addDiagonals;
    printf("\nAdd diagonal connections? (y/n): ");
    scanf(" %c", &addDiagonals);
    
    if (addDiagonals == 'y' || addDiagonals == 'Y') {
        // Down-right diagonals
        for (int row = 0; row < size - 1; row++) {
            for (int col = 0; col < size - 1; col++) {
                int from = row * size + col;
                int to = (row + 1) * size + (col + 1);
                fromNodes[connectionCount] = from;
                toNodes[connectionCount] = to;
                connectionCount++;
            }
        }
        
        // Down-left diagonals
        for (int row = 0; row < size - 1; row++) {
            for (int col = 1; col < size; col++) {
                int from = row * size + col;
                int to = (row + 1) * size + (col - 1);
                fromNodes[connectionCount] = from;
                toNodes[connectionCount] = to;
                connectionCount++;
            }
        }
    }
    
    printf("\nCreating %d connections...\n", connectionCount);
    
    // Apply chosen weight option
    srand(time(NULL)); // Seed for random numbers
    
    for (int i = 0; i < connectionCount; i++) {
        int from = fromNodes[i];
        int to = toNodes[i];
        
        char pathName[50];
        char fromName[MAX_NAME_LEN], toName[MAX_NAME_LEN];
        generateNodeName(from / size, from % size, size, fromName);
        generateNodeName(to / size, to % size, size, toName);
        snprintf(pathName, sizeof(pathName), "%s-%s", fromName, toName);
        
        int weight;
        
        switch(option) {
            case 1: // Random weights
                weight = 1 + rand() % 20; // Random weight 1-20
                break;
            case 2: // Manual input
                printf("Enter weight for %s to %s: ", fromName, toName);
                scanf("%d", &weight);
                while (weight <= 0) {
                    printf("Weight must be positive. Enter again: ");
                    scanf("%d", &weight);
                }
                break;
            case 3: // Default constant
            default:
                weight = 1; // Default weight of 1
                break;
        }
        
        addEdge(g, from, to, pathName, weight, true);
    }
    
    printf("\n=== %dx%d Grid Created Successfully ===\n", size, size);
    printf("Total nodes: %d\n", g->nodeCount);
    printf("Total edges: %d\n", g->edgeCount);
}

// Display NxN grid visualization
void displayNxNGrid(Graph *g) {
    if (g->gridSize == 0) {
        printf("No grid has been created yet!\n");
        return;
    }
    
    int size = g->gridSize;
    printf("\n=== %dx%d Grid Visualization ===\n\n", size, size);
    
    // Generate grid representation
    for (int row = 0; row < size; row++) {
        // Print nodes and horizontal connections
        for (int col = 0; col < size; col++) {
            int nodeIndex = row * size + col;
            printf("%-4s", g->nodes[nodeIndex].name);
            
            // Print horizontal connection if it exists
            if (col < size - 1) {
                int rightNode = nodeIndex + 1;
                if (g->adjMatrix[nodeIndex][rightNode] > 0) {
                    printf("--%-2d--", g->adjMatrix[nodeIndex][rightNode]);
                } else {
                    printf("      ");
                }
            }
        }
        printf("\n");
        
        // Print vertical connections for this row
        if (row < size - 1) {
            for (int col = 0; col < size; col++) {
                int nodeIndex = row * size + col;
                int downNode = nodeIndex + size;
                
                if (g->adjMatrix[nodeIndex][downNode] > 0) {
                    printf("| %-2d     ", g->adjMatrix[nodeIndex][downNode]);
                } else {
                    printf("|        ");
                }
            }
            printf("\n");
            
            for (int col = 0; col < size; col++) {
                printf("|        ");
            }
            printf("\n");
        }
    }
    
    printf("\nGrid Coordinates:\n");
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            int nodeIndex = row * size + col;
            printf("%s(%2d)  ", g->nodes[nodeIndex].name, nodeIndex);
        }
        printf("\n");
    }
}

// Display all shortest paths from a source node
void displayAllShortestPaths(Graph *g, int start) {
    if (start < 0 || start >= g->nodeCount) {
        printf("Error: Invalid node index!\n");
        return;
    }
    
    int distance[MAX_NODES];
    bool visited[MAX_NODES];
    int previous[MAX_NODES];
    
    for (int i = 0; i < g->nodeCount; i++) {
        distance[i] = INF;
        visited[i] = false;
        previous[i] = -1;
    }
    
    distance[start] = 0;
    
    for (int count = 0; count < g->nodeCount - 1; count++) {
        int minDist = INF;
        int minIndex = -1;
        
        for (int v = 0; v < g->nodeCount; v++) {
            if (!visited[v] && distance[v] < minDist) {
                minDist = distance[v];
                minIndex = v;
            }
        }
        
        if (minIndex == -1) break;
        
        visited[minIndex] = true;
        
        for (int v = 0; v < g->nodeCount; v++) {
            if (!visited[v] && g->adjMatrix[minIndex][v] != 0) {
                int edgeWeight = g->adjMatrix[minIndex][v];
                if (distance[minIndex] != INF && 
                    distance[minIndex] + edgeWeight < distance[v]) {
                    distance[v] = distance[minIndex] + edgeWeight;
                    previous[v] = minIndex;
                }
            }
        }
    }
    
    printf("\nShortest paths from %s:\n", g->nodes[start].name);
    printf("%-10s %-15s %s\n", "To", "Distance", "Path");
    printf("------------------------------------------------\n");
    
    // Display in grid order
    if (g->gridSize > 0) {
        int size = g->gridSize;
        int pathsPerLine = 3; // Number of paths to show per line
        
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                int i = row * size + col;
                if (i == start) continue;
                
                if (distance[i] == INF) {
                    printf("%-10s %-15s %s\n", 
                           g->nodes[i].name, "∞", "No path");
                } else {
                    int path[MAX_NODES];
                    int pathLength = 0;
                    int current = i;
                    
                    while (current != -1) {
                        path[pathLength++] = current;
                        current = previous[current];
                    }
                    
                    for (int j = 0; j < pathLength / 2; j++) {
                        int temp = path[j];
                        path[j] = path[pathLength - 1 - j];
                        path[pathLength - 1 - j] = temp;
                    }
                    
                    printf("%-10s %-15d ", g->nodes[i].name, distance[i]);
                    
                    // Show only start and end if path is long
                    if (pathLength > 6) {
                        printf("%s->...->%s", 
                               g->nodes[path[0]].name,
                               g->nodes[path[pathLength-1]].name);
                    } else {
                        for (int j = 0; j < pathLength; j++) {
                            printf("%s", g->nodes[path[j]].name);
                            if (j < pathLength - 1) {
                                printf("->");
                            }
                        }
                    }
                    printf("\n");
                }
            }
        }
    } else {
        // Display for non-grid graphs
        for (int i = 0; i < g->nodeCount; i++) {
            if (i == start) continue;
            
            if (distance[i] == INF) {
                printf("%-10s %-15s %s\n", 
                       g->nodes[i].name, "∞", "No path");
            } else {
                int path[MAX_NODES];
                int pathLength = 0;
                int current = i;
                
                while (current != -1) {
                    path[pathLength++] = current;
                    current = previous[current];
                }
                
                for (int j = 0; j < pathLength / 2; j++) {
                    int temp = path[j];
                    path[j] = path[pathLength - 1 - j];
                    path[pathLength - 1 - j] = temp;
                }
                
                printf("%-10s %-15d ", g->nodes[i].name, distance[i]);
                for (int j = 0; j < pathLength; j++) {
                    printf("%s", g->nodes[path[j]].name);
                    if (j < pathLength - 1) {
                        printf("->");
                    }
                }
                printf("\n");
            }
        }
    }
}

// Display graph
void displayGraph(Graph *g) {
    printf("\n=== Graph Information ===\n");
    printf("Grid Size: %dx%d\n", g->gridSize, g->gridSize);
    printf("Total Nodes: %d\n", g->nodeCount);
    printf("Total Edges: %d\n", g->edgeCount);
    
    printf("\nNodes:\n");
    if (g->gridSize > 0) {
        int size = g->gridSize;
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                int i = row * size + col;
                printf("%s(%d) ", g->nodes[i].name, i);
            }
            printf("\n");
        }
    } else {
        for (int i = 0; i < g->nodeCount; i++) {
            printf("  %d: %s\n", i, g->nodes[i].name);
        }
    }

    printf("\nEdge Summary:\n");
    printf("Total bidirectional connections: %d\n", g->edgeCount);
    
    // Show sample of edges for large graphs
    int maxEdgesToShow = 20;
    if (g->edgeCount > maxEdgesToShow) {
        printf("Showing first %d edges (out of %d):\n", maxEdgesToShow, g->edgeCount);
        for (int i = 0; i < maxEdgesToShow && i < g->edgeCount; i++) {
            printf("  %s ↔ %s (weight=%d)\n",
                   g->nodes[g->edges[i].from].name,
                   g->nodes[g->edges[i].to].name,
                   g->edges[i].weight);
        }
        printf("  ... and %d more edges\n", g->edgeCount - maxEdgesToShow);
    } else {
        for (int i = 0; i < g->edgeCount; i++) {
            printf("  %s ↔ %s (weight=%d)\n",
                   g->nodes[g->edges[i].from].name,
                   g->nodes[g->edges[i].to].name,
                   g->edges[i].weight);
        }
    }
}

// NxN Grid Pathfinder Menu
void gridPathfinderMenu(Graph *g) {
    int choice;
    char startNode[10], endNode[10];
    
    do {
        printf("\n=== NxN Grid Pathfinder (Up to 9x9) ===\n");
        printf("1. Display Grid Visualization\n");
        printf("2. Find Shortest Path between Two Nodes\n");
        printf("3. Display All Shortest Paths from a Node\n");
        printf("4. Show Complete Graph Details\n");
        printf("5. Create New Grid\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                displayNxNGrid(g);
                break;
                
            case 2:
                printf("Enter start node (e.g., A1, B2, C3): ");
                scanf("%s", startNode);
                printf("Enter end node (e.g., A1, B2, C3): ");
                scanf("%s", endNode);
                
                // Convert to uppercase for consistency
                for(int i = 0; startNode[i]; i++) {
                    startNode[i] = toupper(startNode[i]);
                }
                for(int i = 0; endNode[i]; i++) {
                    endNode[i] = toupper(endNode[i]);
                }
                
                int start = findNodeIndex(g, startNode);
                int end = findNodeIndex(g, endNode);
                
                if (start == -1) {
                    printf("Error: Start node '%s' not found!\n", startNode);
                } else if (end == -1) {
                    printf("Error: End node '%s' not found!\n", endNode);
                } else {
                    findShortestPath(g, start, end);
                }
                break;
                
            case 3:
                printf("Enter source node (e.g., A1, B2, C3): ");
                scanf("%s", startNode);
                
                for(int i = 0; startNode[i]; i++) {
                    startNode[i] = toupper(startNode[i]);
                }
                
                start = findNodeIndex(g, startNode);
                if (start == -1) {
                    printf("Error: Node '%s' not found!\n", startNode);
                } else {
                    displayAllShortestPaths(g, start);
                }
                break;
                
            case 4:
                displayGraph(g);
                break;
                
            case 5:
                printf("Creating new grid...\n");
                initializeGraph(g);
                inputNxNGrid(g);
                break;
                
            case 6:
                printf("Exiting Grid Pathfinder...\n");
                break;
                
            default:
                printf("Invalid choice! Please enter 1-6.\n");
        }
    } while (choice != 6);
}

// Performance test for large grids
void runPerformanceTest(Graph *g) {
    if (g->gridSize < 4) {
        printf("Grid is too small for performance test.\n");
        return;
    }
    
    printf("\n=== Performance Test ===\n");
    printf("Testing shortest path on %dx%d grid...\n", g->gridSize, g->gridSize);
    
    // Test corner to corner (longest possible path)
    int start = 0; // Top-left corner
    int end = g->nodeCount - 1; // Bottom-right corner
    
    printf("Finding path from %s to %s...\n", 
           g->nodes[start].name, g->nodes[end].name);
    
    clock_t begin = clock();
    findShortestPath(g, start, end);
    clock_t endTime = clock();
    
    double timeSpent = (double)(endTime - begin) / CLOCKS_PER_SEC;
    printf("Execution time: %.6f seconds\n", timeSpent);
    printf("Grid size: %dx%d\n", g->gridSize, g->gridSize);
    printf("Total nodes: %d\n", g->nodeCount);
    printf("Total edges: %d\n", g->edgeCount);
}

// Main function
int main() {
    Graph g;
    initializeGraph(&g);
    
    printf("========================================\n");
    printf("      PATHFINDER v4.0 - NxN GRID\n");
    printf("        (Supports up to 9x9)\n");
    printf("========================================\n");
    
    // Create NxN grid based on user input
    inputNxNGrid(&g);
    
    // Run the grid pathfinder menu
    gridPathfinderMenu(&g);
    
    // Run performance test for large grids
    if (g.gridSize >= 4) {
        char runTest;
        printf("\nRun performance test on %dx%d grid? (y/n): ", g.gridSize, g.gridSize);
        scanf(" %c", &runTest);
        if (runTest == 'y' || runTest == 'Y') {
            runPerformanceTest(&g);
        }
    }
    
    // Demo some paths
    printf("\n=== Sample Paths ===\n");
    if (g.nodeCount >= 2) {
        findShortestPath(&g, 0, g.nodeCount - 1);  // First to last node
        if (g.nodeCount >= 4) {
            int mid1 = g.nodeCount / 4;
            int mid2 = g.nodeCount * 3 / 4;
            findShortestPath(&g, mid1, mid2);  // Mid to mid
        }
    }
    
    printf("\nThank you for using Pathfinder v4.0!\n");
    return 0;
}