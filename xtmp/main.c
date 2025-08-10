#include <stdio.h>
#include <stdlib.h>
#define M_DEGREE 2
/*

## ✅ 1. **Order (Degree) of a B-Tree**

* Notation: `t`
* This parameter defines the **capacity of a node**

### 💡 A B-Tree of order `t` has the following properties:

| Property                          | Value               |
| --------------------------------- | ------------------- |
| **Minimum number of keys** /node   | `t - 1` (except root) |
| **Maximum number of keys** /node   | `2t - 1`             |
| **Maximum number of children** /node | `2t`               |
| **Minimum number of children**     | `t` (except root)   |

> 👉 **Root** may have fewer than `t - 1` keys.

---
*/

typedef struct BTreeNode{
	int idkey;
    int num_keys; // Number of keys currently in the node
    int keys[M_DEGREE-1]; // Array of keys
    struct BTreeNode *children[M_DEGREE]; // Array of child pointers
    int is_leaf; // True if node is a leaf	
} BTreeNode;

typedef struct BTree {
	BTreeNode *root;
} BTree;
BTree test_tree;
int main(int argc, char *argv[])
{
	int a = 0;
	fprintf(stdout, "0x%p.\n", &a);
	return 0;
}
