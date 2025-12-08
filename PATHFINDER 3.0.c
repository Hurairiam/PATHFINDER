#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

#define MAX_NODES 100
#define MAX_EDGES 1000
#define MAX_NAME_LEN 30
#define INF INT_MAX

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
} Graph;

// Initialize graph
void initializeGraph(Graph *g) {
    g->nodeCount = 0;
    g->edgeCount = 0;
    g->weighted = true;

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

// 3x3 Grid Input System
void input3x3Grid(Graph *g) {
    printf("\n=== 3x3 Grid Input System ===\n");
    printf("You will create a 3x3 grid of nodes (9 total).\n");
    printf("Each node will be connected to its adjacent nodes.\n");
    
    // Create 9 nodes (3x3 grid)
    char nodeNames[9][3] = {"A1", "A2", "A3", 
                           "B1", "B2", "B3", 
                           "C1", "C2", "C3"};
    
    printf("\nCreating 9 nodes for 3x3 grid:\n");
    for (int i = 0; i < 9; i++) {
        addNode(g, nodeNames[i]);
        printf("Created node: %s\n", nodeNames[i]);
    }
    
    printf("\nNow, enter connections between adjacent nodes:\n");
    printf("For each connection, enter the weight (distance).\n");
    printf("Connections will be bidirectional by default.\n\n");
    
    // Define all possible adjacent connections in a 3x3 grid
    int connections[][2] = {
        // Horizontal connections
        {0,1}, {1,2},  // Row 1: A1-A2, A2-A3
        {3,4}, {4,5},  // Row 2: B1-B2, B2-B3
        {6,7}, {7,8},  // Row 3: C1-C2, C2-C3
        
        // Vertical connections
        {0,3}, {3,6},  // Column 1: A1-B1, B1-C1
        {1,4}, {4,7},  // Column 2: A2-B2, B2-C2
        {2,5}, {5,8},  // Column 3: A3-B3, B3-C3
        
        // Diagonal connections (optional)
        {0,4}, {4,8},  // Main diagonal: A1-B2, B2-C3
        {2,4}, {4,6}   // Anti-diagonal: A3-B2, B2-C1
    };
    
    int numConnections = sizeof(connections) / sizeof(connections[0]);
    
    for (int i = 0; i < numConnections; i++) {
        int from = connections[i][0];
        int to = connections[i][1];
        
        char pathName[20];
        sprintf(pathName, "Edge_%s_%s", nodeNames[from], nodeNames[to]);
        
        int weight;
        printf("Enter weight for %s to %s: ", 
               nodeNames[from], nodeNames[to]);
        scanf("%d", &weight);
        
        // Input validation
        while (weight <= 0) {
            printf("Weight must be positive. Enter again: ");
            scanf("%d", &weight);
        }
        
        addEdge(g, from, to, pathName, weight, true);
        printf("  ✓ Added connection: %s ↔ %s (weight: %d)\n", 
               nodeNames[from], nodeNames[to], weight);
    }
    
    printf("\n=== 3x3 Grid Created Successfully ===\n");
    printf("Total nodes: %d\n", g->nodeCount);
    printf("Total edges: %d\n", g->edgeCount);
}

// Display 3x3 grid visualization
void display3x3Grid(Graph *g) {
    printf("\n=== 3x3 Grid Visualization ===\n\n");
    
    // Grid node indices
    // [0] [1] [2]
    // [3] [4] [5]
    // [6] [7] [8]
    
    // Print first row
    printf("          %s", g->nodes[0].name);
    if (g->adjMatrix[0][1] > 0) {
        printf(" --%d-- ", g->adjMatrix[0][1]);
    } else {
        printf("       ");
    }
    printf("%s", g->nodes[1].name);
    if (g->adjMatrix[1][2] > 0) {
        printf(" --%d-- ", g->adjMatrix[1][2]);
    } else {
        printf("       ");
    }
    printf("%s\n", g->nodes[2].name);
    
    // Vertical connections for first row
    printf("          |");
    printf("       |");
    printf("       |\n");
    
    if (g->adjMatrix[0][3] > 0) {
        printf("          %d", g->adjMatrix[0][3]);
    } else {
        printf("          ");
    }
    printf("       ");
    if (g->adjMatrix[1][4] > 0) {
        printf("%d", g->adjMatrix[1][4]);
    } else {
        printf(" ");
    }
    printf("       ");
    if (g->adjMatrix[2][5] > 0) {
        printf("%d", g->adjMatrix[2][5]);
    }
    printf("\n");
    
    printf("          |");
    printf("       |");
    printf("       |\n");
    
    // Print second row
    printf("          %s", g->nodes[3].name);
    if (g->adjMatrix[3][4] > 0) {
        printf(" --%d-- ", g->adjMatrix[3][4]);
    } else {
        printf("       ");
    }
    printf("%s", g->nodes[4].name);
    if (g->adjMatrix[4][5] > 0) {
        printf(" --%d-- ", g->adjMatrix[4][5]);
    } else {
        printf("       ");
    }
    printf("%s\n", g->nodes[5].name);
    
    // Vertical connections for second row
    printf("          |");
    printf("       |");
    printf("       |\n");
    
    if (g->adjMatrix[3][6] > 0) {
        printf("          %d", g->adjMatrix[3][6]);
    } else {
        printf("          ");
    }
    printf("       ");
    if (g->adjMatrix[4][7] > 0) {
        printf("%d", g->adjMatrix[4][7]);
    } else {
        printf(" ");
    }
    printf("       ");
    if (g->adjMatrix[5][8] > 0) {
        printf("%d", g->adjMatrix[5][8]);
    }
    printf("\n");
    
    printf("          |");
    printf("       |");
    printf("       |\n");
    
    // Print third row
    printf("          %s", g->nodes[6].name);
    if (g->adjMatrix[6][7] > 0) {
        printf(" --%d-- ", g->adjMatrix[6][7]);
    } else {
        printf("       ");
    }
    printf("%s", g->nodes[7].name);
    if (g->adjMatrix[7][8] > 0) {
        printf(" --%d-- ", g->adjMatrix[7][8]);
    } else {
        printf("       ");
    }
    printf("%s\n", g->nodes[8].name);
    
    printf("\nGrid Coordinates:\n");
    printf("┌─────┬─────┬─────┐\n");
    printf("│ A1  │ A2  │ A3  │\n");
    printf("│ (0) │ (1) │ (2) │\n");
    printf("├─────┼─────┼─────┤\n");
    printf("│ B1  │ B2  │ B3  │\n");
    printf("│ (3) │ (4) │ (5) │\n");
    printf("├─────┼─────┼─────┤\n");
    printf("│ C1  │ C2  │ C3  │\n");
    printf("│ (6) │ (7) │ (8) │\n");
    printf("└─────┴─────┴─────┘\n");
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
    printf("--------------------------------\n");
    
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

// Display graph
void displayGraph(Graph *g) {
    printf("\n=== Graph Information ===\n");
    printf("Nodes:\n");
    for (int i = 0; i < g->nodeCount; i++) {
        printf("  %d: %s\n", i, g->nodes[i].name);
    }

    printf("\nEdges:\n");
    for (int i = 0; i < g->edgeCount; i++) {
        printf("  %s -> %s via %s (weight=%d, %s)\n",
               g->nodes[g->edges[i].from].name,
               g->nodes[g->edges[i].to].name,
               g->edges[i].pathName,
               g->edges[i].weight,
               g->edges[i].bidirectional ? "bidirectional" : "one-way");
    }
}

// 3x3 Grid Pathfinder Menu
void gridPathfinderMenu(Graph *g) {
    int choice;
    char startNode[10], endNode[10];
    
    do {
        printf("\n=== 3x3 Grid Pathfinder ===\n");
        printf("1. Display Grid Visualization\n");
        printf("2. Find Shortest Path\n");
        printf("3. Display All Paths from a Node\n");
        printf("4. Show Complete Graph Details\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                display3x3Grid(g);
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
                    printf("Valid nodes are: ");
                    for (int i = 0; i < 9 && i < g->nodeCount; i++) {
                        printf("%s ", g->nodes[i].name);
                    }
                    printf("\n");
                } else if (end == -1) {
                    printf("Error: End node '%s' not found!\n", endNode);
                    printf("Valid nodes are: ");
                    for (int i = 0; i < 9 && i < g->nodeCount; i++) {
                        printf("%s ", g->nodes[i].name);
                    }
                    printf("\n");
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
                    printf("Valid nodes are: ");
                    for (int i = 0; i < 9 && i < g->nodeCount; i++) {
                        printf("%s ", g->nodes[i].name);
                    }
                    printf("\n");
                } else {
                    displayAllShortestPaths(g, start);
                }
                break;
                
            case 4:
                displayGraph(g);
                break;
                
            case 5:
                printf("Exiting 3x3 Grid Pathfinder...\n");
                break;
                
            default:
                printf("Invalid choice! Please enter 1-5.\n");
        }
    } while (choice != 5);
}

// Main function
int main() {
    Graph g;
    initializeGraph(&g);
    
    printf("========================================\n");
    printf("      PATHFINDER v3.0 - 3x3 GRID\n");
    printf("========================================\n");
    
    // Create 3x3 grid based on user input
    input3x3Grid(&g);
    
    // Run the grid pathfinder menu
    gridPathfinderMenu(&g);
    
    // Demo some paths
    printf("\n=== Demo Shortest Paths ===\n");
    findShortestPath(&g, 0, 8);  // A1 to C3
    findShortestPath(&g, 2, 6);  // A3 to C1
    findShortestPath(&g, 1, 7);  // A2 to C2
    
    printf("\nThank you for using Pathfinder v3.0!\n");
    return 0;
}