#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>

// ================== CONSTANTS ==================
#define MAX_NODES 100
#define MAX_EDGES 1000
#define MAX_NAME_LEN 30
#define MAX_ROWS 10
#define MAX_COLS 10
#define BACK_OPTION 8
#define MAIN_MENU_OPTION 9

// ================== DATA STRUCTURES ==================
typedef struct {
    char name[MAX_NAME_LEN];
    int id;
    bool active;
} Node;

typedef struct {
    int from;
    int to;
    char pathName[MAX_NAME_LEN];
    int weight;
    bool bidirectional;
    char unit[20];
    bool active;
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
    bool symmetricalGrid;
    int gridRows;
    int gridCols;
    char weightUnit[20];
} Graph;

// ================== GLOBAL VARIABLES ==================
Graph currentGraph;

// ================== UTILITY FUNCTIONS ==================
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printSeparator() {
    printf("============================================================\n");
}

void printDoubleSeparator() {
    printf("============================================================\n");
    printf("============================================================\n");
}

void printTitle(const char* title) {
    printf("\n");
    printDoubleSeparator();
    printf("                    %s\n", title);
    printDoubleSeparator();
}

void printNavigationInfo() {
    printf("\n[8] Back  |  [9] Main Menu\n");
}

void printSubtitle(const char* subtitle) {
    printf("\n%s\n", subtitle);
    for (int i = 0; i < strlen(subtitle); i++) printf("-");
    printf("\n");
}

void printAvailableNodes(Graph *g) {
    printf("Available nodes: ");
    int count = 0;
    for (int i = 0; i < g->nodeCount; i++) {
        if (g->nodes[i].active) {
            if (count > 0) printf(", ");
            printf("%s", g->nodes[i].name);
            count++;
        }
    }
    printf("\n");
}

// ================== GRAPH INITIALIZATION ==================
void initializeGraph(Graph *g) {
    g->nodeCount = 0;
    g->edgeCount = 0;
    g->weighted = false;
    g->symmetricalGrid = false;
    g->gridRows = 0;
    g->gridCols = 0;
    strcpy(g->weightUnit, "");

    for (int i = 0; i < MAX_NODES; i++) {
        g->nodes[i].name[0] = '\0';
        g->nodes[i].id = i;
        g->nodes[i].active = false;
        for (int j = 0; j < MAX_NODES; j++) {
            g->adjMatrix[i][j] = 0;
            g->adjPath[i][j][0] = '\0';
            g->adjBidirectional[i][j] = false;
        }
    }

    for (int i = 0; i < MAX_EDGES; i++) {
        g->edges[i].from = 0;
        g->edges[i].to = 0;
        g->edges[i].pathName[0] = '\0';
        g->edges[i].weight = 0;
        g->edges[i].bidirectional = false;
        strcpy(g->edges[i].unit, "");
        g->edges[i].active = false;
    }
}

// ================== NODE MANAGEMENT ==================
int findNodeIndex(Graph *g, const char *name) {
    for (int i = 0; i < g->nodeCount; i++) {
        if (g->nodes[i].active && strcmp(g->nodes[i].name, name) == 0)
            return i;
    }
    return -1;
}

int findFirstInactiveNode(Graph *g) {
    for (int i = 0; i < g->nodeCount; i++) {
        if (!g->nodes[i].active) {
            return i;
        }
    }
    return g->nodeCount;
}

bool addNode(Graph *g, const char *name) {
    if (g->nodeCount >= MAX_NODES) {
        printf("Error: Maximum nodes (%d) reached!\n", MAX_NODES);
        return false;
    }

    if (strlen(name) >= MAX_NAME_LEN) {
        printf("Error: Node name too long (max %d characters)\n", MAX_NAME_LEN-1);
        return false;
    }

    if (findNodeIndex(g, name) != -1) {
        printf("Error: Node '%s' already exists!\n", name);
        return false;
    }

    int index = findFirstInactiveNode(g);
    if (index == g->nodeCount) {
        index = g->nodeCount;
        g->nodeCount++;
    }

    strcpy(g->nodes[index].name, name);
    g->nodes[index].id = index;
    g->nodes[index].active = true;
    return true;
}

bool deleteNode(Graph *g, int nodeIndex) {
    if (nodeIndex < 0 || nodeIndex >= g->nodeCount || !g->nodes[nodeIndex].active) {
        printf("Error: Invalid node index!\n");
        return false;
    }

    g->nodes[nodeIndex].active = false;

    for (int i = 0; i < g->edgeCount; i++) {
        if (g->edges[i].active &&
            (g->edges[i].from == nodeIndex || g->edges[i].to == nodeIndex)) {
            g->edges[i].active = false;

            g->adjMatrix[g->edges[i].from][g->edges[i].to] = 0;
            g->adjPath[g->edges[i].from][g->edges[i].to][0] = '\0';
            g->adjBidirectional[g->edges[i].from][g->edges[i].to] = false;

            if (g->edges[i].bidirectional) {
                g->adjMatrix[g->edges[i].to][g->edges[i].from] = 0;
                g->adjPath[g->edges[i].to][g->edges[i].from][0] = '\0';
                g->adjBidirectional[g->edges[i].to][g->edges[i].from] = false;
            }
        }
    }

    return true;
}

void listNodes(Graph *g) {
    printf("\nCurrent Nodes:\n");
    printf("------------------------------------------------------------\n");
    bool hasNodes = false;

    for (int i = 0; i < g->nodeCount; i++) {
        if (g->nodes[i].active) {
            hasNodes = true;
            printf("%d. %s\n", i+1, g->nodes[i].name);
        }
    }

    if (!hasNodes) {
        printf("No nodes defined.\n");
    }
    printf("------------------------------------------------------------\n");
}

// ================== EDGE MANAGEMENT ==================
bool addEdge(Graph *g, int from, int to, const char *pathName, int weight, bool bidirectional, const char *unit) {
    if (from == to) {
        printf("Error: Cannot create edge from a node to itself.\n");
        return false;
    }

    if (strlen(pathName) >= MAX_NAME_LEN) {
        printf("Error: Path name too long (max %d characters)\n", MAX_NAME_LEN-1);
        return false;
    }

    if (g->weighted && weight < 0) {
        printf("Error: Weight cannot be negative.\n");
        return false;
    }

    for (int i = 0; i < g->edgeCount; i++) {
        if (g->edges[i].active && g->edges[i].from == from && g->edges[i].to == to) {
            printf("Warning: Overwriting existing edge from %s to %s\n",
                   g->nodes[from].name, g->nodes[to].name);
            g->edges[i].active = false;
        }
    }

    int edgeIndex = -1;
    for (int i = 0; i < g->edgeCount; i++) {
        if (!g->edges[i].active) {
            edgeIndex = i;
            break;
        }
    }

    if (edgeIndex == -1) {
        if (g->edgeCount >= MAX_EDGES) {
            printf("Error: Maximum edges (%d) reached!\n", MAX_EDGES);
            return false;
        }
        edgeIndex = g->edgeCount;
        g->edgeCount++;
    }

    g->edges[edgeIndex].from = from;
    g->edges[edgeIndex].to = to;
    strcpy(g->edges[edgeIndex].pathName, pathName);
    g->edges[edgeIndex].weight = weight;
    g->edges[edgeIndex].bidirectional = bidirectional;
    g->edges[edgeIndex].active = true;
    if (strlen(unit) > 0) {
        strcpy(g->edges[edgeIndex].unit, unit);
    } else {
        strcpy(g->edges[edgeIndex].unit, g->weightUnit);
    }

    int edgeValue = g->weighted ? weight : 1;
    g->adjMatrix[from][to] = edgeValue;
    strcpy(g->adjPath[from][to], pathName);
    g->adjBidirectional[from][to] = bidirectional;

    if (bidirectional) {
        g->adjMatrix[to][from] = edgeValue;
        strcpy(g->adjPath[to][from], pathName);
        g->adjBidirectional[to][from] = true;
    }

    return true;
}

bool deleteEdge(Graph *g, int from, int to) {
    bool found = false;

    for (int i = 0; i < g->edgeCount; i++) {
        if (g->edges[i].active && g->edges[i].from == from && g->edges[i].to == to) {
            g->edges[i].active = false;
            found = true;

            g->adjMatrix[from][to] = 0;
            g->adjPath[from][to][0] = '\0';
            g->adjBidirectional[from][to] = false;

            if (g->edges[i].bidirectional) {
                g->adjMatrix[to][from] = 0;
                g->adjPath[to][from][0] = '\0';
                g->adjBidirectional[to][from] = false;

                for (int j = 0; j < g->edgeCount; j++) {
                    if (g->edges[j].active && g->edges[j].from == to && g->edges[j].to == from) {
                        g->edges[j].active = false;
                        break;
                    }
                }
            }
            break;
        }
    }

    return found;
}

void listEdges(Graph *g) {
    printf("\nCurrent Edges:\n");
    printf("------------------------------------------------------------\n");
    bool hasEdges = false;

    for (int i = 0; i < g->edgeCount; i++) {
        if (g->edges[i].active) {
            hasEdges = true;
            Edge e = g->edges[i];
            printf("%d. %s -> %s", i+1, g->nodes[e.from].name, g->nodes[e.to].name);
            if (g->weighted) {
                printf(" (Weight: %d", e.weight);
                if (strlen(e.unit) > 0) {
                    printf(" %s", e.unit);
                }
                printf(")");
            }
            printf(" [%s]", e.pathName);
            if (e.bidirectional) {
                printf(" <->");
            } else {
                printf(" ->");
            }
            printf("\n");
        }
    }

    if (!hasEdges) {
        printf("No edges defined.\n");
    }
    printf("------------------------------------------------------------\n");
}

// ================== GRAPH VISUALIZATION ==================
int countActiveEdges(Graph *g) {
    int count = 0;
    for (int i = 0; i < g->edgeCount; i++) {
        if (g->edges[i].active) {
            count++;
        }
    }
    return count;
}

int countActiveNodes(Graph *g) {
    int count = 0;
    for (int i = 0; i < g->nodeCount; i++) {
        if (g->nodes[i].active) {
            count++;
        }
    }
    return count;
}

void printGraphASCII(Graph *g) {
    printTitle("GRAPH VISUALIZATION");
    printf("\n");

    if (g->symmetricalGrid) {
        printf("Grid Layout (%d x %d):\n\n", g->gridRows, g->gridCols);

        for (int r = 0; r < g->gridRows; r++) {
            for (int c = 0; c < g->gridCols; c++) {
                int idx = r * g->gridCols + c;
                if (g->nodes[idx].active) {
                    printf("[%-15s] ", g->nodes[idx].name);
                } else {
                    printf("[%-15s] ", "(deleted)");
                }
                if (c < g->gridCols - 1) {
                    printf("<-> ");
                }
            }
            printf("\n");

            if (r < g->gridRows - 1) {
                for (int c = 0; c < g->gridCols; c++) {
                    printf("     |         ");
                    if (c < g->gridCols - 1) {
                        printf("    ");
                    }
                }
                printf("\n");
                for (int c = 0; c < g->gridCols; c++) {
                    printf("     v         ");
                    if (c < g->gridCols - 1) {
                        printf("    ");
                    }
                }
                printf("\n");
            }
        }
    } else {
        int activeNodes = countActiveNodes(g);
        int activeEdges = countActiveEdges(g);

        printf("Active Nodes (%d total):\n", activeNodes);
        for (int i = 0; i < g->nodeCount; i++) {
            if (g->nodes[i].active) {
                printf("  %d. %s\n", i+1, g->nodes[i].name);
            }
        }

        printf("\nActive Edges (%d total):\n", activeEdges);
        if (activeEdges == 0) {
            printf("  No edges defined.\n");
        } else {
            for (int i = 0; i < g->edgeCount; i++) {
                if (g->edges[i].active) {
                    Edge e = g->edges[i];
                    printf("  %s -> %s", g->nodes[e.from].name, g->nodes[e.to].name);
                    if (g->weighted) {
                        printf(" (Weight: %d", e.weight);
                        if (strlen(e.unit) > 0) {
                            printf(" %s", e.unit);
                        }
                        printf(")");
                    }
                    printf(" [%s]", e.pathName);
                    if (e.bidirectional) {
                        printf(" <->");
                    } else {
                        printf(" ->");
                    }
                    printf("\n");
                }
            }
        }

        printf("\nConnection Diagram:\n");
        for (int i = 0; i < g->nodeCount; i++) {
            if (!g->nodes[i].active) continue;

            printf("%s\n", g->nodes[i].name);

            bool hasConnections = false;
            for (int j = 0; j < g->nodeCount; j++) {
                if (g->nodes[j].active && g->adjMatrix[i][j] != 0) {
                    if (!hasConnections) {
                        printf("  |\n");
                        hasConnections = true;
                    }
                    printf("  |--> %s", g->nodes[j].name);
                    if (g->weighted) {
                        printf(" (%d", g->adjMatrix[i][j]);
                        if (strlen(g->weightUnit) > 0) {
                            printf(" %s", g->weightUnit);
                        }
                        printf(")");
                    }
                    printf("\n");
                }
            }
            if (!hasConnections) {
                printf("  (No outgoing connections)\n");
            }
            if (i < g->nodeCount - 1) {
                printf("\n");
            }
        }
    }
    printf("\n");
    printSeparator();
}

// ================== PATHFINDING ALGORITHMS ==================
bool bfs(Graph *g, int start, int end, int parent[]) {
    bool visited[MAX_NODES] = {false};
    int queue[MAX_NODES];
    int front = 0, rear = 0;

    for (int i = 0; i < g->nodeCount; i++) {
        parent[i] = -1;
    }

    queue[rear++] = start;
    visited[start] = true;

    while (front < rear) {
        int u = queue[front++];

        if (u == end) {
            return true;
        }

        for (int v = 0; v < g->nodeCount; v++) {
            if (g->nodes[v].active && g->adjMatrix[u][v] != 0 && !visited[v]) {
                visited[v] = true;
                parent[v] = u;
                queue[rear++] = v;
            }
        }
    }

    return false;
}

bool dijkstra(Graph *g, int start, int end, int parent[], long long *totalWeight) {
    bool visited[MAX_NODES] = {false};
    long long dist[MAX_NODES];

    for (int i = 0; i < g->nodeCount; i++) {
        dist[i] = LLONG_MAX;
        parent[i] = -1;
    }
    dist[start] = 0;

    for (int count = 0; count < g->nodeCount; count++) {
        int u = -1;
        long long minDist = LLONG_MAX;
        for (int i = 0; i < g->nodeCount; i++) {
            if (g->nodes[i].active && !visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }

        if (u == -1 || dist[u] == LLONG_MAX) {
            break;
        }

        visited[u] = true;

        for (int v = 0; v < g->nodeCount; v++) {
            if (g->nodes[v].active && g->adjMatrix[u][v] != 0) {
                long long w = g->adjMatrix[u][v];
                if (!visited[v] && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                }
            }
        }
    }

    *totalWeight = (dist[end] == LLONG_MAX) ? -1 : dist[end];
    return dist[end] != LLONG_MAX;
}

// ================== ENHANCED SCREEN FUNCTIONS ==================
void showBackMenu() {
    printf("\n============================================================\n");
    printf("[8] Go Back  |  [9] Main Menu\n");
    printf("============================================================\n");
}

int showWeightSelectionScreen() {
    clearScreen();
    printTitle("GRAPH SETTINGS");
    printf("\n");

    int choice;
    while (1) {
        printf("Choose graph type:\n");
        printf("  1. Weighted Graph\n");
        printf("  2. Unweighted Graph\n");
        showBackMenu();
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter 1 or 2.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (choice == BACK_OPTION || choice == MAIN_MENU_OPTION) {
            return choice;
        }

        if (choice == 1 || choice == 2) {
            return choice;
        } else {
            printf("Invalid choice. Please enter 1 or 2.\n");
        }
    }
}

void showNodeManagement() {
    bool managingNodes = true;

    while (managingNodes) {
        clearScreen();
        printTitle("NODE MANAGEMENT");
        printf("\n");

        int activeNodes = countActiveNodes(&currentGraph);
        printf("Active Nodes: %d | Total Nodes: %d\n\n", activeNodes, currentGraph.nodeCount);

        listNodes(&currentGraph);

        printf("\nOptions:\n");
        printf("  1. Add New Node\n");
        printf("  2. Delete Node\n");
        printf("  3. View Graph\n");
        printf("  4. Return to Previous Menu\n");
        showBackMenu();

        int choice;
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (choice == BACK_OPTION) {
            managingNodes = false;
            return;
        }

        if (choice == MAIN_MENU_OPTION) {
            managingNodes = false;
            return;
        }

        switch(choice) {
            case 1: {
                printf("\n============================================================\n");
                printf("                      ADD NODE\n");
                printf("============================================================\n\n");

                char name[MAX_NAME_LEN];
                while (1) {
                    printf("Enter node name (or 8 to cancel): ");
                    fgets(name, MAX_NAME_LEN, stdin);
                    name[strcspn(name, "\n")] = 0;

                    if (strcmp(name, "8") == 0) break;
                    if (strcmp(name, "9") == 0) {
                        managingNodes = false;
                        return;
                    }

                    if (strlen(name) == 0) {
                        printf("Node name cannot be empty.\n");
                        continue;
                    }

                    if (addNode(&currentGraph, name)) {
                        printf("Node '%s' added successfully!\n", name);
                        break;
                    }
                }
                printf("\nPress Enter to continue...");
                clearInputBuffer();
                break;
            }

            case 2: {
                if (activeNodes == 0) {
                    printf("\nNo nodes to delete.\n");
                    printf("Press Enter to continue...");
                    clearInputBuffer();
                    break;
                }

                printf("\n============================================================\n");
                printf("                     DELETE NODE\n");
                printf("============================================================\n\n");

                listNodes(&currentGraph);

                int nodeNum;
                printf("\nEnter node number to delete (0 to cancel): ");
                if (scanf("%d", &nodeNum) != 1) {
                    printf("Invalid input.\n");
                    clearInputBuffer();
                    break;
                }
                clearInputBuffer();

                if (nodeNum == 0) break;

                int activeCount = 0;
                int nodeIndex = -1;

                for (int i = 0; i < currentGraph.nodeCount; i++) {
                    if (currentGraph.nodes[i].active) {
                        activeCount++;
                        if (activeCount == nodeNum) {
                            nodeIndex = i;
                            break;
                        }
                    }
                }

                if (nodeIndex != -1) {
                    printf("\nDelete node '%s' and all connected edges? (y/n): ",
                           currentGraph.nodes[nodeIndex].name);
                    char confirm;
                    scanf(" %c", &confirm);
                    clearInputBuffer();

                    if (confirm == 'y' || confirm == 'Y') {
                        if (deleteNode(&currentGraph, nodeIndex)) {
                            printf("Node deleted successfully.\n");
                        }
                    } else {
                        printf("Deletion cancelled.\n");
                    }
                } else {
                    printf("Invalid node number.\n");
                }

                printf("\nPress Enter to continue...");
                clearInputBuffer();
                break;
            }

            case 3:
                printGraphASCII(&currentGraph);
                printf("Press Enter to continue...");
                clearInputBuffer();
                break;

            case 4:
                managingNodes = false;
                break;

            default:
                printf("Invalid choice. Please enter 1-4, 8, or 9.\n");
                printf("Press Enter to continue...");
                clearInputBuffer();
        }
    }
}

void showSymmetricalGridCreation() {
    clearScreen();
    printTitle("SYMMETRICAL GRID CREATION");

    int rows, cols;

    while (1) {
        printf("\nEnter number of rows (1-%d, or 8 to cancel): ", MAX_ROWS);
        if (scanf("%d", &rows) != 1) {
            printf("Invalid input.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (rows == BACK_OPTION) return;
        if (rows == MAIN_MENU_OPTION) return;

        if (rows < 1 || rows > MAX_ROWS) {
            printf("Please enter a number between 1 and %d.\n", MAX_ROWS);
            continue;
        }
        break;
    }

    while (1) {
        printf("Enter number of columns (1-%d, or 8 to cancel): ", MAX_COLS);
        if (scanf("%d", &cols) != 1) {
            printf("Invalid input.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (cols == BACK_OPTION) return;
        if (cols == MAIN_MENU_OPTION) return;

        if (cols < 1 || cols > MAX_COLS) {
            printf("Please enter a number between 1 and %d.\n", MAX_COLS);
            continue;
        }
        break;
    }

    if (rows * cols > MAX_NODES) {
        printf("Error: Grid size (%d nodes) exceeds maximum (%d)\n", rows * cols, MAX_NODES);
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    currentGraph.gridRows = rows;
    currentGraph.gridCols = cols;

    if (currentGraph.weighted) {
        printf("\nEnter weight unit (e.g., 'miles', 'km', 'hours', 'lightyears', or leave empty): ");
        char unit[20];
        fgets(unit, 20, stdin);
        unit[strcspn(unit, "\n")] = 0;
        if (strlen(unit) > 0) {
            strcpy(currentGraph.weightUnit, unit);
        }
    }

    printf("\nEnter names for each node (enter 8 to cancel any time):\n");

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            char name[MAX_NAME_LEN];
            while (1) {
                printf("Node at row %d, column %d: ", r + 1, c + 1);
                fgets(name, MAX_NAME_LEN, stdin);
                name[strcspn(name, "\n")] = 0;

                if (strcmp(name, "8") == 0) return;
                if (strcmp(name, "9") == 0) return;

                if (strlen(name) == 0) {
                    printf("Node name cannot be empty.\n");
                    continue;
                }

                if (!addNode(&currentGraph, name)) {
                    printf("Please try a different name.\n");
                    continue;
                }
                break;
            }
        }
    }

    printf("\nCreating grid connections...\n");

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int current = r * cols + c;

            if (c < cols - 1) {
                int right = r * cols + (c + 1);
                char pathName[MAX_NAME_LEN];
                sprintf(pathName, "Horizontal_%d_%d", current + 1, right + 1);
                int weight = 1;

                if (currentGraph.weighted) {
                    printf("Enter weight for edge %s-%s (or -1 for default 1): ",
                           currentGraph.nodes[current].name, currentGraph.nodes[right].name);
                    if (scanf("%d", &weight) != 1 || weight == -1) {
                        weight = 1;
                    }
                    clearInputBuffer();
                }
                addEdge(&currentGraph, current, right, pathName, weight, true, currentGraph.weightUnit);
            }

            if (r < rows - 1) {
                int bottom = (r + 1) * cols + c;
                char pathName[MAX_NAME_LEN];
                sprintf(pathName, "Vertical_%d_%d", current + 1, bottom + 1);
                int weight = 1;

                if (currentGraph.weighted) {
                    printf("Enter weight for edge %s-%s (or -1 for default 1): ",
                           currentGraph.nodes[current].name, currentGraph.nodes[bottom].name);
                    if (scanf("%d", &weight) != 1 || weight == -1) {
                        weight = 1;
                    }
                    clearInputBuffer();
                }
                addEdge(&currentGraph, current, bottom, pathName, weight, true, currentGraph.weightUnit);
            }
        }
    }

    currentGraph.symmetricalGrid = true;
    printf("\nSymmetrical grid created successfully! (%d nodes, %d edges)\n",
           rows * cols, (rows * (cols - 1) + cols * (rows - 1)));

    printGraphASCII(&currentGraph);
    printf("Press Enter to continue...");
    clearInputBuffer();
}

void showCustomGraphCreation() {
    clearScreen();
    printTitle("CUSTOM GRAPH CREATION");

    int numNodes;

    while (1) {
        printf("\nEnter number of nodes (1-%d, or 8 to cancel): ", MAX_NODES);
        if (scanf("%d", &numNodes) != 1) {
            printf("Invalid input.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (numNodes == BACK_OPTION) return;
        if (numNodes == MAIN_MENU_OPTION) return;

        if (numNodes < 1 || numNodes > MAX_NODES) {
            printf("Please enter a number between 1 and %d.\n", MAX_NODES);
            continue;
        }
        break;
    }

    if (currentGraph.weighted) {
        printf("\nEnter weight unit (e.g., 'miles', 'km', 'hours', 'lightyears', or leave empty): ");
        char unit[20];
        fgets(unit, 20, stdin);
        unit[strcspn(unit, "\n")] = 0;
        if (strlen(unit) > 0) {
            strcpy(currentGraph.weightUnit, unit);
        }
    }

    printf("\nEnter names for each node (enter 8 to cancel any time):\n");

    for (int i = 0; i < numNodes; i++) {
        char name[MAX_NAME_LEN];
        while (1) {
            printf("Node %d: ", i + 1);
            fgets(name, MAX_NAME_LEN, stdin);
            name[strcspn(name, "\n")] = 0;

            if (strcmp(name, "8") == 0) return;
            if (strcmp(name, "9") == 0) return;

            if (strlen(name) == 0) {
                printf("Node name cannot be empty.\n");
                continue;
            }

            if (!addNode(&currentGraph, name)) {
                printf("Please try a different name.\n");
                continue;
            }
            break;
        }
    }

    bool editing = true;
    while (editing) {
        clearScreen();
        printTitle("CUSTOM GRAPH EDITING");
        printf("\n");

        int activeNodes = countActiveNodes(&currentGraph);
        int activeEdges = countActiveEdges(&currentGraph);
        printf("Active Nodes: %d | Active Edges: %d\n\n", activeNodes, activeEdges);

        printf("Options:\n");
        printf("  1. Manage Nodes (Add/Delete)\n");
        printf("  2. Add New Edge\n");
        printf("  3. Delete Existing Edge\n");
        printf("  4. View Current Graph\n");
        printf("  5. Finish Editing and Continue\n");
        showBackMenu();

        int choice;
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (choice == BACK_OPTION || choice == MAIN_MENU_OPTION) {
            editing = false;
            if (choice == MAIN_MENU_OPTION) return;
            continue;
        }

        if (choice == 5) {
            editing = false;
            continue;
        }

        if (choice == 4) {
            printGraphASCII(&currentGraph);
            printf("Press Enter to continue...");
            clearInputBuffer();
            continue;
        }

        if (choice == 1) {
            showNodeManagement();
            continue;
        }

        if (choice == 3) {
            if (activeEdges == 0) {
                printf("\nNo edges to delete.\n");
                printf("Press Enter to continue...");
                clearInputBuffer();
                continue;
            }

            listEdges(&currentGraph);

            int edgeNum;
            printf("\nEnter edge number to delete (0 to cancel): ");
            if (scanf("%d", &edgeNum) != 1) {
                printf("Invalid input.\n");
                clearInputBuffer();
                continue;
            }
            clearInputBuffer();

            if (edgeNum == 0) continue;

            int activeCount = 0;
            Edge *edgeToDelete = NULL;
            int edgeIndex = -1;

            for (int i = 0; i < currentGraph.edgeCount; i++) {
                if (currentGraph.edges[i].active) {
                    activeCount++;
                    if (activeCount == edgeNum) {
                        edgeToDelete = &currentGraph.edges[i];
                        edgeIndex = i;
                        break;
                    }
                }
            }

            if (edgeToDelete != NULL) {
                printf("\nDelete edge %s -> %s [%s]? (y/n): ",
                       currentGraph.nodes[edgeToDelete->from].name,
                       currentGraph.nodes[edgeToDelete->to].name,
                       edgeToDelete->pathName);
                char confirm;
                scanf(" %c", &confirm);
                clearInputBuffer();

                if (confirm == 'y' || confirm == 'Y') {
                    deleteEdge(&currentGraph, edgeToDelete->from, edgeToDelete->to);
                    printf("Edge deleted successfully.\n");
                } else {
                    printf("Deletion cancelled.\n");
                }
            } else {
                printf("Invalid edge number.\n");
            }

            printf("Press Enter to continue...");
            clearInputBuffer();
            continue;
        }

        if (choice == 2) {
            printf("\n============================================================\n");
            printf("                      ADD EDGE\n");
            printf("============================================================\n\n");

            printAvailableNodes(&currentGraph);

            char fromName[MAX_NAME_LEN], toName[MAX_NAME_LEN];
            int fromIndex = -1, toIndex = -1;

            while (fromIndex == -1) {
                printf("\nEnter source node (or 8 to cancel): ");
                fgets(fromName, MAX_NAME_LEN, stdin);
                fromName[strcspn(fromName, "\n")] = 0;

                if (strcmp(fromName, "8") == 0) break;
                if (strcmp(fromName, "9") == 0) {
                    editing = false;
                    return;
                }

                fromIndex = findNodeIndex(&currentGraph, fromName);
                if (fromIndex == -1) {
                    printf("Node '%s' not found. ", fromName);
                    printAvailableNodes(&currentGraph);
                }
            }

            if (fromIndex == -1) continue;

            while (toIndex == -1) {
                printf("Enter destination node (or 8 to cancel): ");
                fgets(toName, MAX_NAME_LEN, stdin);
                toName[strcspn(toName, "\n")] = 0;

                if (strcmp(toName, "8") == 0) break;
                if (strcmp(toName, "9") == 0) {
                    editing = false;
                    return;
                }

                toIndex = findNodeIndex(&currentGraph, toName);
                if (toIndex == -1) {
                    printf("Node '%s' not found. ", toName);
                    printAvailableNodes(&currentGraph);
                } else if (strcmp(fromName, toName) == 0) {
                    printf("Source and destination cannot be the same.\n");
                    toIndex = -1;
                }
            }

            if (toIndex == -1) continue;

            char pathName[MAX_NAME_LEN];
            int weight = 1;
            bool bidirectional = false;
            char unit[20] = "";

            printf("Enter path name: ");
            fgets(pathName, MAX_NAME_LEN, stdin);
            pathName[strcspn(pathName, "\n")] = 0;

            if (currentGraph.weighted) {
                while (1) {
                    printf("Enter weight (non-negative): ");
                    if (scanf("%d", &weight) != 1) {
                        printf("Invalid input.\n");
                        clearInputBuffer();
                        continue;
                    }

                    if (weight < 0) {
                        printf("Weight cannot be negative.\n");
                        continue;
                    }

                    clearInputBuffer();
                    break;
                }

                printf("Enter unit for this edge (optional, press Enter for default): ");
                fgets(unit, 20, stdin);
                unit[strcspn(unit, "\n")] = 0;
            }

            printf("Bidirectional? (y/n): ");
            char bidirChoice;
            scanf(" %c", &bidirChoice);
            clearInputBuffer();
            bidirectional = (bidirChoice == 'y' || bidirChoice == 'Y');

            if (strlen(unit) > 0) {
                addEdge(&currentGraph, fromIndex, toIndex, pathName, weight, bidirectional, unit);
            } else {
                addEdge(&currentGraph, fromIndex, toIndex, pathName, weight, bidirectional, currentGraph.weightUnit);
            }

            printf("Edge added successfully.\n");
            printf("Press Enter to continue...");
            clearInputBuffer();
        }
    }

    printf("\nCustom graph created successfully! (%d nodes, %d edges)\n",
           countActiveNodes(&currentGraph), countActiveEdges(&currentGraph));
    printGraphASCII(&currentGraph);
    printf("Press Enter to continue...");
    clearInputBuffer();
}

void showPredefinedGraph() {
    clearScreen();
    printTitle("PREDEFINED CITY GRAPH");
    printf("Loading predefined graph of US cities...\n\n");

    initializeGraph(&currentGraph);

    addNode(&currentGraph, "New York");
    addNode(&currentGraph, "Boston");
    addNode(&currentGraph, "Washington");
    addNode(&currentGraph, "Chicago");
    addNode(&currentGraph, "Detroit");
    addNode(&currentGraph, "Atlanta");
    addNode(&currentGraph, "Miami");
    addNode(&currentGraph, "Dallas");
    addNode(&currentGraph, "Los Angeles");
    addNode(&currentGraph, "San Francisco");

    currentGraph.weighted = true;
    strcpy(currentGraph.weightUnit, "miles");

    addEdge(&currentGraph, 0, 1, "I-95 North", 215, true, "miles");
    addEdge(&currentGraph, 0, 2, "I-95 South", 225, true, "miles");
    addEdge(&currentGraph, 0, 3, "I-80 West", 790, true, "miles");
    addEdge(&currentGraph, 1, 2, "Coastal Route", 440, true, "miles");
    addEdge(&currentGraph, 2, 5, "I-85 South", 640, true, "miles");
    addEdge(&currentGraph, 3, 4, "I-94 East", 280, true, "miles");
    addEdge(&currentGraph, 3, 7, "I-55 South", 970, true, "miles");
    addEdge(&currentGraph, 4, 5, "I-75 South", 750, true, "miles");
    addEdge(&currentGraph, 5, 6, "I-75 South", 665, true, "miles");
    addEdge(&currentGraph, 5, 7, "I-20 West", 795, true, "miles");
    addEdge(&currentGraph, 7, 8, "I-20 West", 1435, true, "miles");
    addEdge(&currentGraph, 8, 9, "Coastal Highway", 382, true, "miles");
    addEdge(&currentGraph, 2, 3, "Appalachian Route", 700, true, "miles");

    printf("Predefined city graph loaded successfully!\n");
    printf("  Cities: 10\n");
    printf("  Routes: 13 (bidirectional)\n");
    printf("  Total connections: 26\n");

    printGraphASCII(&currentGraph);
    printf("Press Enter to continue...");
    clearInputBuffer();
}

void showPathFinding() {
    bool inPathFinding = true;

    while (inPathFinding) {
        clearScreen();
        printTitle("PATH FINDING");
        printf("\n");

        printf("Current Graph: ");
        if (currentGraph.symmetricalGrid) {
            printf("Symmetrical Grid (%d x %d)\n", currentGraph.gridRows, currentGraph.gridCols);
        } else {
            printf("Custom Graph\n");
        }
        printf("Active Nodes: %d | Active Edges: %d\n",
               countActiveNodes(&currentGraph), countActiveEdges(&currentGraph));
        printf("Weighted: %s", currentGraph.weighted ? "Yes" : "No");
        if (currentGraph.weighted && strlen(currentGraph.weightUnit) > 0) {
            printf(" (%s)", currentGraph.weightUnit);
        }
        printf("\n\n");

        printf("Options:\n");
        printf("  1. Find a Path\n");
        printf("  2. View Graph Visualization\n");
        printf("  3. Edit Graph (Add/Delete Nodes & Edges)\n");
        printf("  4. Reset Current Graph\n");
        showBackMenu();

        int menuChoice;
        while (1) {
            printf("Enter choice: ");
            if (scanf("%d", &menuChoice) != 1) {
                printf("Invalid input.\n");
                clearInputBuffer();
                continue;
            }
            clearInputBuffer();

            if (menuChoice == BACK_OPTION) {
                inPathFinding = false;
                return;
            }

            if (menuChoice == MAIN_MENU_OPTION) {
                inPathFinding = false;
                return;
            }

            if (menuChoice >= 1 && menuChoice <= 4) {
                break;
            } else {
                printf("Invalid choice. Please enter 1-4, 8, or 9.\n");
            }
        }

        switch(menuChoice) {
            case 1:
                break;
            case 2:
                printGraphASCII(&currentGraph);
                printf("Press Enter to continue...");
                clearInputBuffer();
                continue;
            case 3: {
                bool editing = true;
                while (editing) {
                    clearScreen();
                    printTitle("GRAPH EDITING");
                    printf("\n");

                    int activeNodes = countActiveNodes(&currentGraph);
                    int activeEdges = countActiveEdges(&currentGraph);
                    printf("Active Nodes: %d | Active Edges: %d\n\n", activeNodes, activeEdges);

                    printf("Options:\n");
                    printf("  1. Manage Nodes (Add/Delete)\n");
                    printf("  2. Add New Edge\n");
                    printf("  3. Delete Existing Edge\n");
                    printf("  4. View Current Graph\n");
                    printf("  5. Return to Path Finding\n");
                    showBackMenu();

                    printf("Enter choice: ");
                    if (scanf("%d", &menuChoice) != 1) {
                        printf("Invalid input.\n");
                        clearInputBuffer();
                        continue;
                    }
                    clearInputBuffer();

                    if (menuChoice == BACK_OPTION || menuChoice == MAIN_MENU_OPTION) {
                        editing = false;
                        inPathFinding = false;
                        return;
                    }

                    if (menuChoice == 5) {
                        editing = false;
                        break;
                    }

                    if (menuChoice == 4) {
                        printGraphASCII(&currentGraph);
                        printf("Press Enter to continue...");
                        clearInputBuffer();
                        continue;
                    }

                    if (menuChoice == 1) {
                        showNodeManagement();
                        continue;
                    }

                    if (menuChoice == 3) {
                        if (activeEdges == 0) {
                            printf("\nNo edges to delete.\n");
                            printf("Press Enter to continue...");
                            clearInputBuffer();
                            continue;
                        }

                        listEdges(&currentGraph);

                        int edgeNum;
                        printf("\nEnter edge number to delete (0 to cancel): ");
                        if (scanf("%d", &edgeNum) != 1) {
                            printf("Invalid input.\n");
                            clearInputBuffer();
                            continue;
                        }
                        clearInputBuffer();

                        if (edgeNum == 0) continue;

                        int activeCount = 0;
                        Edge *edgeToDelete = NULL;

                        for (int i = 0; i < currentGraph.edgeCount; i++) {
                            if (currentGraph.edges[i].active) {
                                activeCount++;
                                if (activeCount == edgeNum) {
                                    edgeToDelete = &currentGraph.edges[i];
                                    break;
                                }
                            }
                        }

                        if (edgeToDelete != NULL) {
                            printf("\nDelete edge %s -> %s [%s]? (y/n): ",
                                   currentGraph.nodes[edgeToDelete->from].name,
                                   currentGraph.nodes[edgeToDelete->to].name,
                                   edgeToDelete->pathName);
                            char confirm;
                            scanf(" %c", &confirm);
                            clearInputBuffer();

                            if (confirm == 'y' || confirm == 'Y') {
                                deleteEdge(&currentGraph, edgeToDelete->from, edgeToDelete->to);
                                printf("Edge deleted successfully.\n");
                            } else {
                                printf("Deletion cancelled.\n");
                            }
                        } else {
                            printf("Invalid edge number.\n");
                        }

                        printf("Press Enter to continue...");
                        clearInputBuffer();
                        continue;
                    }

                    if (menuChoice == 2) {
                        printf("\n============================================================\n");
                        printf("                      ADD EDGE\n");
                        printf("============================================================\n\n");

                        printAvailableNodes(&currentGraph);

                        char fromName[MAX_NAME_LEN], toName[MAX_NAME_LEN];
                        int fromIndex = -1, toIndex = -1;

                        while (fromIndex == -1) {
                            printf("\nEnter source node (or 8 to cancel): ");
                            fgets(fromName, MAX_NAME_LEN, stdin);
                            fromName[strcspn(fromName, "\n")] = 0;

                            if (strcmp(fromName, "8") == 0) break;
                            if (strcmp(fromName, "9") == 0) {
                                editing = false;
                                inPathFinding = false;
                                return;
                            }

                            fromIndex = findNodeIndex(&currentGraph, fromName);
                            if (fromIndex == -1) {
                                printf("Node '%s' not found. ", fromName);
                                printAvailableNodes(&currentGraph);
                            }
                        }

                        if (fromIndex == -1) continue;

                        while (toIndex == -1) {
                            printf("Enter destination node (or 8 to cancel): ");
                            fgets(toName, MAX_NAME_LEN, stdin);
                            toName[strcspn(toName, "\n")] = 0;

                            if (strcmp(toName, "8") == 0) break;
                            if (strcmp(toName, "9") == 0) {
                                editing = false;
                                inPathFinding = false;
                                return;
                            }

                            toIndex = findNodeIndex(&currentGraph, toName);
                            if (toIndex == -1) {
                                printf("Node '%s' not found. ", toName);
                                printAvailableNodes(&currentGraph);
                            } else if (strcmp(fromName, toName) == 0) {
                                printf("Source and destination cannot be the same.\n");
                                toIndex = -1;
                            }
                        }

                        if (toIndex == -1) continue;

                        char pathName[MAX_NAME_LEN];
                        int weight = 1;
                        bool bidirectional = false;
                        char unit[20] = "";

                        printf("Enter path name: ");
                        fgets(pathName, MAX_NAME_LEN, stdin);
                        pathName[strcspn(pathName, "\n")] = 0;

                        if (currentGraph.weighted) {
                            while (1) {
                                printf("Enter weight (non-negative): ");
                                if (scanf("%d", &weight) != 1) {
                                    printf("Invalid input.\n");
                                    clearInputBuffer();
                                    continue;
                                }

                                if (weight < 0) {
                                    printf("Weight cannot be negative.\n");
                                    continue;
                                }

                                clearInputBuffer();
                                break;
                            }

                            printf("Enter unit for this edge (optional, press Enter for default): ");
                            fgets(unit, 20, stdin);
                            unit[strcspn(unit, "\n")] = 0;
                        }

                        printf("Bidirectional? (y/n): ");
                        char bidirChoice;
                        scanf(" %c", &bidirChoice);
                        clearInputBuffer();
                        bidirectional = (bidirChoice == 'y' || bidirChoice == 'Y');

                        if (strlen(unit) > 0) {
                            addEdge(&currentGraph, fromIndex, toIndex, pathName, weight, bidirectional, unit);
                        } else {
                            addEdge(&currentGraph, fromIndex, toIndex, pathName, weight, bidirectional, currentGraph.weightUnit);
                        }

                        printf("Edge added successfully.\n");
                        printf("Press Enter to continue...");
                        clearInputBuffer();
                    }
                }
                continue;
            }
            case 4: {
                printf("\nReset current graph? All data will be lost. (y/n): ");
                char confirm;
                scanf(" %c", &confirm);
                clearInputBuffer();

                if (confirm == 'y' || confirm == 'Y') {
                    initializeGraph(&currentGraph);
                    printf("Graph reset successfully!\n");
                    printf("Press Enter to continue...");
                    clearInputBuffer();
                }
                continue;
            }
        }

        bool findAnotherPath = true;
        while (findAnotherPath) {
            clearScreen();
            printTitle("FIND PATH");
            printf("\n");

            printAvailableNodes(&currentGraph);

            char startName[MAX_NAME_LEN], endName[MAX_NAME_LEN];
            int startIndex = -1, endIndex = -1;

            while (startIndex == -1) {
                printf("\nEnter Start node (or 8 to go back): ");
                fgets(startName, MAX_NAME_LEN, stdin);
                startName[strcspn(startName, "\n")] = 0;

                if (strcmp(startName, "8") == 0) {
                    findAnotherPath = false;
                    break;
                }

                if (strcmp(startName, "9") == 0) {
                    inPathFinding = false;
                    return;
                }

                startIndex = findNodeIndex(&currentGraph, startName);
                if (startIndex == -1) {
                    printf("Node '%s' not found. ", startName);
                    printAvailableNodes(&currentGraph);
                }
            }

            if (!findAnotherPath) break;

            while (endIndex == -1) {
                printf("Enter Destination node (or 8 to go back): ");
                fgets(endName, MAX_NAME_LEN, stdin);
                endName[strcspn(endName, "\n")] = 0;

                if (strcmp(endName, "8") == 0) {
                    findAnotherPath = false;
                    break;
                }

                if (strcmp(endName, "9") == 0) {
                    inPathFinding = false;
                    return;
                }

                endIndex = findNodeIndex(&currentGraph, endName);
                if (endIndex == -1) {
                    printf("Node '%s' not found. ", endName);
                    printAvailableNodes(&currentGraph);
                } else if (strcmp(startName, endName) == 0) {
                    printf("Start and destination cannot be the same.\n");
                    endIndex = -1;
                }
            }

            if (!findAnotherPath) break;

            int parent[MAX_NODES];
            long long totalWeight = 0;
            bool pathFound;

            if (currentGraph.weighted) {
                pathFound = dijkstra(&currentGraph, startIndex, endIndex, parent, &totalWeight);
            } else {
                pathFound = bfs(&currentGraph, startIndex, endIndex, parent);
            }

            clearScreen();
            if (pathFound) {
                int path[MAX_NODES];
                int steps = 0;
                int current = endIndex;

                while (current != -1) {
                    path[steps++] = current;
                    current = parent[current];
                }

                printTitle("PATH FOUND!");
                printf("\n");

                printf("Start: %s\n", currentGraph.nodes[startIndex].name);
                printf("Destination: %s\n\n", currentGraph.nodes[endIndex].name);

                printf("Route: ");
                for (int i = steps - 1; i >= 0; i--) {
                    printf("%s", currentGraph.nodes[path[i]].name);
                    if (i > 0) printf(" -> ");
                }

                printf("\n\nStep-by-Step Directions:\n");
                printf("------------------------------------------------------------\n");

                for (int i = steps - 1; i > 0; i--) {
                    int u = path[i], v = path[i - 1];
                    printf("From %s, take %s",
                           currentGraph.nodes[u].name, currentGraph.adjPath[u][v]);
                    if (currentGraph.weighted) {
                        printf(" (%d", currentGraph.adjMatrix[u][v]);
                        char unit[20] = "";
                        for (int e = 0; e < currentGraph.edgeCount; e++) {
                            Edge edge = currentGraph.edges[e];
                            if (edge.active && ((edge.from == u && edge.to == v) ||
                                (edge.bidirectional && edge.from == v && edge.to == u))) {
                                if (strlen(edge.unit) > 0) {
                                    strcpy(unit, edge.unit);
                                }
                                break;
                            }
                        }
                        if (strlen(unit) > 0) {
                            printf(" %s", unit);
                        } else if (strlen(currentGraph.weightUnit) > 0) {
                            printf(" %s", currentGraph.weightUnit);
                        }
                        printf(")");
                    }
                    printf(" to %s.\n", currentGraph.nodes[v].name);
                }

                printf("------------------------------------------------------------\n");
                printf("Arrive at %s.\n\n", currentGraph.nodes[endIndex].name);

                printf("Summary:\n");
                printf("  - Connections taken: %d\n", steps - 1);
                printf("  - Nodes visited: %d\n", steps);
                if (currentGraph.weighted && totalWeight >= 0) {
                    printf("  - Total distance: %lld", totalWeight);
                    if (strlen(currentGraph.weightUnit) > 0) {
                        printf(" %s", currentGraph.weightUnit);
                    }
                    printf("\n");
                }
            } else {
                printTitle("NO PATH FOUND");
                printf("\n");
                printf("No path exists between %s and %s\n\n", startName, endName);
                printf("Possible reasons:\n");
                printf("  1. Nodes are not connected\n");
                printf("  2. Edges only go one direction\n");
                printf("  3. Graph is disconnected\n");
            }

            printf("\n============================================================\n");
            printf("Options:\n");
            printf("  1. Find another path\n");
            printf("  2. Return to path finding menu\n");
            showBackMenu();

            printf("Enter choice: ");
            char choice;
            scanf(" %c", &choice);
            clearInputBuffer();

            if (choice == '8') {
                findAnotherPath = false;
            } else if (choice == '9') {
                inPathFinding = false;
                return;
            } else if (choice == '2') {
                findAnotherPath = false;
            } else if (choice != '1') {
                findAnotherPath = false;
            }
        }
    }
}

// ================== MAIN FUNCTION ==================
int main() {
    bool running = true;

    while (running) {
        clearScreen();
        printTitle("PATHFINDER");
        printf("             Advanced Graph Pathfinding System\n\n");

        printf("============================================================\n");
        printf("                     MAIN MENU\n");
        printf("============================================================\n");
        printf("Choose an option:\n");
        printf("  1. Symmetrical Grids\n");
        printf("  2. Fully Customised Graph\n");
        printf("  3. Predefined City Graph\n");
        printf("  0. Exit Program\n");
        printf("============================================================\n");

        printf("\nNote: Use [8] to go back, [9] for main menu\n");
        printf("Enter choice: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (choice == 0) {
            running = false;
            continue;
        }

        if (choice == BACK_OPTION || choice == MAIN_MENU_OPTION) {
            printf("You are already in the main menu.\n");
            printf("Press Enter to continue...");
            clearInputBuffer();
            continue;
        }

        initializeGraph(&currentGraph);

        if (choice == 1) {
            int weightChoice = showWeightSelectionScreen();
            if (weightChoice == BACK_OPTION || weightChoice == MAIN_MENU_OPTION) {
                continue;
            }
            if (weightChoice == 1 || weightChoice == 2) {
                currentGraph.weighted = (weightChoice == 1);
                showSymmetricalGridCreation();
                showPathFinding();
            }
        }
        else if (choice == 2) {
            int weightChoice = showWeightSelectionScreen();
            if (weightChoice == BACK_OPTION || weightChoice == MAIN_MENU_OPTION) {
                continue;
            }
            if (weightChoice == 1 || weightChoice == 2) {
                currentGraph.weighted = (weightChoice == 1);
                showCustomGraphCreation();
                showPathFinding();
            }
        }
        else if (choice == 3) {
            showPredefinedGraph();
            showPathFinding();
        }
        else {
            printf("Invalid choice. Please enter 0-3.\n");
            printf("Press Enter to continue...");
            clearInputBuffer();
        }
    }

    clearScreen();
    printTitle("THANK YOU FOR USING PATHFINDER!");
    printf("                  Goodbye!\n");
    printSeparator();

    return 0;
}
