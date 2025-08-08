#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEGREE 3  // max keys per node

typedef struct BPlusNode {
    int is_leaf;
    int num_keys;
    int keys[DEGREE];

    // For internal nodes
    struct BPlusNode* children[DEGREE + 1];

    // For leaf nodes
    int values[DEGREE];
    struct BPlusNode* next;

} BPlusNode;

typedef struct {
    BPlusNode* root;
} BPlusTree;

// Allocate a new node
BPlusNode* create_node(int is_leaf) {
    BPlusNode* node = (BPlusNode*)malloc(sizeof(BPlusNode));
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    node->next = NULL;
    memset(node->children, 0, sizeof(node->children));
    return node;
}

BPlusTree* bptree_init() {
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    tree->root = create_node(1);
    return tree;
}

// Find the value for a key
int bptree_find(BPlusTree* tree, int key, int* value_out) {
    BPlusNode* node = tree->root;
    int i;

    while (!node->is_leaf) {
        for (i = 0; i < node->num_keys; ++i) {
            if (key < node->keys[i]) break;
        }
        node = node->children[i];
    }

    for (i = 0; i < node->num_keys; ++i) {
        if (node->keys[i] == key) {
            *value_out = node->values[i];
            return 1;  // found
        }
    }

    return 0;  // not found
}

// Split leaf node
void split_leaf(BPlusNode* leaf, int* up_key, BPlusNode** new_leaf) {
    int mid = (DEGREE + 1) / 2;

    *new_leaf = create_node(1);
    (*new_leaf)->num_keys = leaf->num_keys - mid;

    memcpy((*new_leaf)->keys, &leaf->keys[mid], sizeof(int) * (*new_leaf)->num_keys);
    memcpy((*new_leaf)->values, &leaf->values[mid], sizeof(int) * (*new_leaf)->num_keys);

    leaf->num_keys = mid;
    (*new_leaf)->next = leaf->next;
    leaf->next = *new_leaf;

    *up_key = (*new_leaf)->keys[0];
}

// Split internal node
void split_internal(BPlusNode* node, int* up_key, BPlusNode** new_node) {
    int mid = node->num_keys / 2;
    int i;

    *up_key = node->keys[mid];
    *new_node = create_node(0);
    (*new_node)->num_keys = node->num_keys - mid - 1;

    for (i = 0; i < (*new_node)->num_keys; ++i) {
        (*new_node)->keys[i] = node->keys[mid + 1 + i];
        (*new_node)->children[i] = node->children[mid + 1 + i];
    }
    (*new_node)->children[i] = node->children[node->num_keys + 1];

    node->num_keys = mid;
}

// Insert into tree
int bptree_insert_internal(BPlusNode* node, int key, int value, int* up_key, BPlusNode** new_child) {
    int i, pos;

    // Leaf node
    if (node->is_leaf) {
        // Find position
        for (pos = 0; pos < node->num_keys && node->keys[pos] < key; ++pos);

        // Shift to make room
        for (i = node->num_keys; i > pos; --i) {
            node->keys[i] = node->keys[i - 1];
            node->values[i] = node->values[i - 1];
        }

        node->keys[pos] = key;
        node->values[pos] = value;
        node->num_keys++;

        if (node->num_keys > DEGREE) {
            split_leaf(node, up_key, new_child);
            return 1;  // need to insert in parent
        }
        return 0;  // done
    }

    // Internal node
    for (pos = 0; pos < node->num_keys && key >= node->keys[pos]; ++pos);

    int need_split = bptree_insert_internal(node->children[pos], key, value, up_key, new_child);

    if (need_split) {
        // Shift to insert new key/child
        for (i = node->num_keys; i > pos; --i) {
            node->keys[i] = node->keys[i - 1];
            node->children[i + 1] = node->children[i];
        }

        node->keys[pos] = *up_key;
        node->children[pos + 1] = *new_child;
        node->num_keys++;

        if (node->num_keys > DEGREE) {
            split_internal(node, up_key, new_child);
            return 1;
        }
    }

    return 0;
}

void bptree_insert(BPlusTree* tree, int key, int value) {
    int up_key;
    BPlusNode* new_child = NULL;

    if (bptree_insert_internal(tree->root, key, value, &up_key, &new_child)) {
        BPlusNode* new_root = create_node(0);
        new_root->keys[0] = up_key;
        new_root->children[0] = tree->root;
        new_root->children[1] = new_child;
        new_root->num_keys = 1;
        tree->root = new_root;
    }
}

// Simplified remove: only deletes from leaf without rebalancing
void bptree_delete(BPlusTree* tree, int key) {
    BPlusNode* node = tree->root;
    int i, pos;

    while (!node->is_leaf) {
        for (pos = 0; pos < node->num_keys && key >= node->keys[pos]; ++pos);
        node = node->children[pos];
    }

    for (i = 0; i < node->num_keys; ++i) {
        if (node->keys[i] == key) {
            for (; i < node->num_keys - 1; ++i) {
                node->keys[i] = node->keys[i + 1];
                node->values[i] = node->values[i + 1];
            }
            node->num_keys--;
            break;
        }
    }
}

// Print leaf keys
void bptree_print_leaves(BPlusTree* tree) {
    BPlusNode* node = tree->root;
    while (!node->is_leaf)
        node = node->children[0];

    while (node) {
        int i;
        for (i = 0; i < node->num_keys; ++i)
            printf("%d ", node->keys[i]);
        node = node->next;
    }
    printf("\n");
}
/*
Input keys: [10, 20, 5, 15, 7, 35]
[10]

[10, 20]

        [20]          ← internal root
       /    \
  [5, 10]   [20]


        [20]
       /    \
  [5, 10]   [15, 20]


          [7, 20]
         /   |    \
    [5]  [7,10]  [15,20]

             [15]
           /      \
       [7]        [20]
     /    \      /     \
  [5]  [7,10]  [15]  [20,35]


[5] → [7,10] → [15] → [20,35]


| Đặc điểm                 | Ý nghĩa                                                  |
| ------------------------ | -------------------------------------------------------- |
| **Balanced** (Cân bằng)  | Mọi đường đi từ root đến lá đều dài bằng nhau            |
| **Multi-way**            | Mỗi node có tối đa `M` con (M là bậc của cây)            |
| **Chỉ lưu dữ liệu ở lá** | Dữ liệu (key/value) chỉ nằm ở các **leaf nodes**         |
| **Liên kết lá**          | Các leaf nodes được nối với nhau → hỗ trợ **range scan** |
https://www.youtube.com/watch?v=K1a2Bk8NrYQ
*/

