#ifndef BINARY_SEARCH_TREE
#define BINARY_SEARCH_TREE

typedef struct TreeNode TreeNode;

// Core tree functions
TreeNode* create_node(void* data);
TreeNode* insert_bst(TreeNode* tree, void* data, int (*compare)(void* a, void* b));
TreeNode* search(TreeNode* tree, void* data, int (*compare)(void* a, void* b));
TreeNode* delete_node(TreeNode* tree, void* data, int (*compare)(void* a, void* b));
void free_tree(TreeNode* tree);

// Tree traversal functions
void print_btree(TreeNode* tree, void (*print_function)(void* data), int depth);
void print_inorder(TreeNode* tree, void (*print_function)(void* data));
void print_preorder(TreeNode* tree, void (*print_function)(void* data));
void print_postorder(TreeNode* tree, void (*print_function)(void* data));

// Tree information functions
int tree_height(TreeNode* tree);
int tree_size(TreeNode* tree);
TreeNode* find_min(TreeNode* tree);
TreeNode* find_max(TreeNode* tree);

#endif