#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

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
    
    // Initialize distance, visited, and previous arrays
    int distance[MAX_NODES];
    bool visited[MAX_NODES];
    int previous[MAX_NODES];
    
    for (int i = 0; i < g->nodeCount; i++) {
        distance[i] = INF;
        visited[i] = false;
        previous[i] = -1;
    }
    
    distance[start] = 0;
    
    // Main Dijkstra's algorithm loop
    for (int count = 0; count < g->nodeCount - 1; count++) {
        // Find the unvisited node with minimum distance
        int minDist = INF;
        int minIndex = -1;
        
        for (int v = 0; v < g->nodeCount; v++) {
            if (!visited[v] && distance[v] < minDist) {
                minDist = distance[v];
                minIndex = v;
            }
        }
        
        if (minIndex == -1) break; // No reachable nodes left
        
        visited[minIndex] = true;
        
        // Update distances of adjacent nodes
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
    
    // Check if path exists
    if (distance[end] == INF) {
        printf("No path exists from %s to %s\n", 
               g->nodes[start].name, g->nodes[end].name);
        return;
    }
    
    // Reconstruct path
    int path[MAX_NODES];
    int pathLength = 0;
    int current = end;
    
    while (current != -1) {
        path[pathLength++] = current;
        current = previous[current];
    }
    
    // Reverse the path
    for (int i = 0; i < pathLength / 2; i++) {
        int temp = path[i];
        path[i] = path[pathLength - 1 - i];
        path[pathLength - 1 - i] = temp;
    }
    
    // Display results
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

// Display all shortest paths from a source node
void displayAllShortestPaths(Graph *g, int start) {
    if (start < 0 || start >= g->nodeCount) {
        printf("Error: Invalid node index!\n");
        return;
    }
    
    // Run Dijkstra's algorithm
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
            // Reconstruct path for this node
            int path[MAX_NODES];
            int pathLength = 0;
            int current = i;
            
            while (current != -1) {
                path[pathLength++] = current;
                current = previous[current];
            }
            
            // Reverse the path
            for (int j = 0; j < pathLength / 2; j++) {
                int temp = path[j];
                path[j] = path[pathLength - 1 - j];
                path[pathLength - 1 - j] = temp;
            }
            
            // Display path
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

// Interactive menu for testing
void interactiveMenu(Graph *g) {
    int choice;
    char nodeName1[MAX_NAME_LEN], nodeName2[MAX_NAME_LEN];
    char pathName[MAX_NAME_LEN];
    int weight;
    char bidirectional;
    
    do {
        printf("\n=== Graph Shortest Path Menu ===\n");
        printf("1. Display Graph\n");
        printf("2. Find Shortest Path\n");
        printf("3. Display All Shortest Paths from Source\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                displayGraph(g);
                break;
                
            case 2:
                printf("Enter start node name: ");
                scanf("%s", nodeName1);
                printf("Enter end node name: ");
                scanf("%s", nodeName2);
                
                int start = findNodeIndex(g, nodeName1);
                int end = findNodeIndex(g, nodeName2);
                
                if (start == -1) {
                    printf("Error: Start node '%s' not found!\n", nodeName1);
                } else if (end == -1) {
                    printf("Error: End node '%s' not found!\n", nodeName2);
                } else {
                    findShortestPath(g, start, end);
                }
                break;
                
            case 3:
                printf("Enter source node name: ");
                scanf("%s", nodeName1);
                
                start = findNodeIndex(g, nodeName1);
                if (start == -1) {
                    printf("Error: Node '%s' not found!\n", nodeName1);
                } else {
                    displayAllShortestPaths(g, start);
                }
                break;
                
            case 4:
                printf("Exiting...\n");
                break;
                
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 4);
}

int main() {
    Graph g;
    initializeGraph(&g);

    // Add nodes
    addNode(&g, "A");
    addNode(&g, "B");
    addNode(&g, "C");
    addNode(&g, "D");
    addNode(&g, "E");
    addNode(&g, "F");

    // Add edges
    addEdge(&g, 0, 1, "Path1", 4, true);
    addEdge(&g, 0, 2, "Path2", 2, true);
    addEdge(&g, 1, 2, "Path3", 1, true);
    addEdge(&g, 1, 3, "Path4", 5, true);
    addEdge(&g, 2, 3, "Path5", 8, true);
    addEdge(&g, 2, 4, "Path6", 10, true);
    addEdge(&g, 3, 4, "Path7", 2, true);
    addEdge(&g, 3, 5, "Path8", 6, true);
    addEdge(&g, 4, 5, "Path9", 3, true);
    
    // Test shortest path
    printf("=== Graph Information ===\n");
    displayGraph(&g);
    
    // Run interactive menu
    interactiveMenu(&g);
    
    // Test some specific paths
    printf("\n=== Pre-defined Path Tests ===\n");
    findShortestPath(&g, 0, 5);  // A to F
    findShortestPath(&g, 1, 4);  // B to E
    findShortestPath(&g, 0, 3);  // A to D

    return 0;
}