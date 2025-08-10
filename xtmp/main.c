#include <stdio.h>
#include <stdlib.h>
#define M_DEGREE 2
typedef struct BTreeNode{
	int idkey;
    int num_keys; // Number of keys currently in the node
    int keys[M_DEGREE-1]; // Array of keys
    struct BTreeNode *children[M_DEGREE]; // Array of child pointers
    int is_leaf; // True if node is a leaf	
} BTreeNode;
int main(int argc, char *argv[])
{
	int a = 0;
	fprintf(stdout, "0x%p.\n", &a);
	return 0;
}
