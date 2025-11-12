#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_NODES 10000
#define MAX_NEIGHBORS 10
#define NUM_INSERTS 50000

struct Node {
    int id;
    int dist;
    int degree;
    struct Node **neighbors;
};

// Simple queue for BFS
struct Queue {
    int head, tail;
    int capacity;
    struct Node **data;
};

void enqueue(struct Queue *q, struct Node *n) {
    q->data[q->tail++] = n;
    if (q->tail == q->capacity)
        q->tail = 0;
}

struct Node *dequeue(struct Queue *q) {
    struct Node *ret = q->data[q->head++];
    if (q->head == q->capacity)
        q->head = 0;
    return ret;
}

int is_empty(struct Queue *q) { return q->head == q->tail; }

int main() {
    // Declarations for C89 compatibility MUST be at the beginning of the function
    int i, j;
    struct Node **nodes;
    struct Queue queue;

    // Variables used inside loops/logic blocks must be declared here too
    struct Node *node;
    struct Node *neighbor;
    struct Node *curr;
    struct Node *nbr;
    int node_id;
    int add_count;
    int neighbor_id;

    srand((unsigned)time(NULL));

    nodes = (struct Node **)malloc(NUM_NODES * sizeof(struct Node *));
    if (!nodes) {
        printf("Failed to allocate node array\n");
        return 1;
    }

    for (i = 0; i < NUM_NODES; i++) {
        nodes[i] = (struct Node *)malloc(sizeof(struct Node));
        if (!nodes[i]) {
            printf("Failed to allocate node %d\n", i);
            return 1;
        }
        nodes[i]->id = i;
        nodes[i]->dist = -1; // unvisited
        nodes[i]->degree = 0;
        nodes[i]->neighbors = (struct Node **)malloc(MAX_NEIGHBORS * sizeof(struct Node *));
        if (!nodes[i]->neighbors) {
            printf("Failed to allocate neighbors for node %d\n", i);
            return 1;
        }
    }

    // Randomly add edges, creating a dense graph with randomness
    for (i = 0; i < NUM_INSERTS; i++) {
        node_id = rand() % NUM_NODES;
        add_count = (rand() % MAX_NEIGHBORS) + 1;
        node = nodes[node_id];

        for (j = 0; j < add_count && node->degree < MAX_NEIGHBORS; j++) {
            neighbor_id = rand() % NUM_NODES;
            if (neighbor_id != node_id) {
                node->neighbors[node->degree++] = nodes[neighbor_id];
                // For undirected graph, add inverse edge, check capacity
                // 'neighbor' declared at start of main
                neighbor = nodes[neighbor_id];
                if (neighbor->degree < MAX_NEIGHBORS)
                    neighbor->neighbors[neighbor->degree++] = node;
            }
        }
    }

    // Initialize queue for BFS
    queue.capacity = NUM_NODES + 1;
    queue.data = (struct Node **)malloc(queue.capacity * sizeof(struct Node *));
    queue.head = queue.tail = 0;

    // Start BFS from node 0
    nodes[0]->dist = 0;
    enqueue(&queue, nodes[0]);

    while (!is_empty(&queue)) {
        curr = dequeue(&queue);
        for (i = 0; i < curr->degree; i++) {
            // 'nbr' declared at start of main
            nbr = curr->neighbors[i];
            if (nbr->dist == -1) {
                nbr->dist = curr->dist + 1;
                enqueue(&queue, nbr);
            }
        }
    }

    // Print distances of first 20 nodes as sanity check
    for (i = 0; i < 20 && i < NUM_NODES; i++) {
        printf("Node %d distance = %d\n", nodes[i]->id, nodes[i]->dist);
    }

    // Free memory
    for (i = 0; i < NUM_NODES; i++) {
        free(nodes[i]->neighbors);
        free(nodes[i]);
    }
    free(nodes);
    free(queue.data);

    return 0;
}
