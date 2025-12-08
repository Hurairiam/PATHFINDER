#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NODES 100
#define MAX_EDGES 1000
#define MAX_NAME_LEN 30

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
    strcpy(g->nodes[g->nodeCount].name, name);
    g->nodes[g->nodeCount].id = g->nodeCount;
    g->nodeCount++;
}

// Add edge
void addEdge(Graph *g, int from, int to, const char *pathName, int weight, bool bidirectional) {
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

int main() {
    Graph g;
    initializeGraph(&g);

    // Hardcoded nodes
    addNode(&g, "A");
    addNode(&g, "B");
    addNode(&g, "C");
    addNode(&g, "D");

    // Hardcoded edges
    addEdge(&g, 0, 1, "Path1", 5, true);
    addEdge(&g, 0, 2, "Path2", 3, false);
    addEdge(&g, 1, 3, "Path3", 2, true);
    addEdge(&g, 2, 3, "Path4", 4, true);

    displayGraph(&g);

    return 0;
}
