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
    int num_keys; // Number of keys currently in the node
    int keys[2 * M_DEGREE-1]; // Array of keys
    struct BTreeNode *children[2 * M_DEGREE]; // Array of child pointers
    int is_leaf; // True if node is a leaf	
	char data[0];
} BTreeNode;



typedef struct BTree {
	BTreeNode *root;
} BTree;
BTree test_tree;

int insert_key(BTree *, int key, void *data, int len_data);
int find_key(BTree *, int key, BTreeNode **node);
int delete_key(BTree *, int key);

int main(int argc, char *argv[])
{
	int a = 0;
	fprintf(stdout, "0x%p.\n", &a);
	return 0;
}
typedef enum BTREE_ERR{
	BTREE_ERR_OK = 0,
	BTREE_ERR_NULL,
	BTREE_MEM_NULL,


	BTREE_ERR_END,
} BTREE_ERR;
int insert_key(BTree *tree, int key, void *data, int len_data) {
	int ret = 0;
	do {
		if(!tree) {
			ret = BTREE_ERR_NULL;
			break;
		}
		if(!tree->root) {
			BTreeNode *root = 0;
			size_t sz  = sizeof(BTreeNode) + len_data + 1;
			root = (BTreeNode*) malloc(sz);
			if(!root) {
				ret = BTREE_MEM_NULL;
				break;
			}
			memset(root, 0, sz);
			root->keys[0] = key;
			root->num_keys++;
			if(len_data > 0) {
				memcpy(root->data, data, len_data);
			}
			break;
		}		
	} while(0);
	return ret;
}
//https://github.com/tidwall/btree.c