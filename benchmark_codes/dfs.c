#include <stdio.h>
#include <stdlib.h>

#define NODE_COUNT 100000  // Number of tree nodes

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

// Allocate dummy variable-sized array to create heap fragmentation
void allocate_dummy() {
    int size = rand() % 256 + 16;  // allocate between 16 and 271 ints
    int *dummy = (int *)malloc(size * sizeof(int));
    // Touch dummy data to avoid optimization
    if (dummy) {
        for (int i = 0; i < size; i++)
            dummy[i] = i;
    }
    // Intentionally do not free dummy to add fragmentation
}

// Allocate node with fragmentation effect
Node *random_node_alloc() {
    allocate_dummy();  // dummy allocation before node
    Node *node = (Node *)malloc(sizeof(Node));
    node->left = NULL;
    node->right = NULL;
    node->data = 1;
    return node;
}

// Create roughly balanced binary tree with NODE_COUNT nodes
Node* create_random_tree(int n) {
    if (n == 0) return NULL;
    Node *root = random_node_alloc();
    int left_nodes = rand() % n;
    int right_nodes = n - left_nodes - 1;
    root->left = create_random_tree(left_nodes);
    root->right = create_random_tree(right_nodes);
    return root;
}

// DFS traversal to generate pointer chasing workload
int dfs_sum(Node *root) {
    if (root == NULL) return 0;
    return root->data + dfs_sum(root->left) + dfs_sum(root->right);
}

int main() {
    printf("Creating random binary tree with %d nodes...\n", NODE_COUNT);
    srand(1);  // fixed seed for repeatability
    Node *root = create_random_tree(NODE_COUNT);
    printf("Starting DFS traversal...\n");
    int sum = dfs_sum(root);
    printf("DFS sum: %d\n", sum);
    // Free omitted for simplicity
    return 0;
}
