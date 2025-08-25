#include "binary_search_tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct TreeNode{

    void* data;
    TreeNode* left;
    TreeNode* right;

};  

TreeNode* initialize_tree(void* data){

    TreeNode* tree = malloc(sizeof(TreeNode));
    tree->data = data;
    tree->left = NULL;
    tree->right = NULL;
    return tree;

}

TreeNode* insert_bst(TreeNode* tree, void* data, int (*compare)(void* a, void* b)){
    
    if(tree == NULL){
        tree = (TreeNode*)malloc(sizeof(TreeNode));
        tree->data = data;
        tree->left = tree->right = NULL;
    }
    else {
        int cmp = compare(data, tree->data);
        if(cmp < 0){
            tree->left = insert_bst(tree->left, data, compare);
        }
        else if(cmp > 0){ 
            tree->right = insert_bst(tree->right, data, compare);
        }
        /* if cmp == 0, data already exists, don't insert duplicates */
    }
    
    return tree;
}

void print_btree(TreeNode* tree, void (*print_function)(void* data), int depth){
    
    if(tree == NULL){
        return;
    }
    
    // Print right subtree first (so it appears at top)
    print_btree_simple(tree->right, print_function, depth + 1);
    
    // Print current node with indentation
    for(int i = 0; i < depth; i++){
        printf("    ");
    }
    printf("└─ ");
    print_function(tree->data);
    printf("\n");
    
    // Print left subtree
    print_btree_simple(tree->left, print_function, depth + 1);
}


TreeNode* search(TreeNode* tree, void* data, int (*compare)(void* a, void* b)){
    
    if(tree == NULL){
        return NULL;  // Data not found
    }
    
    int cmp = compare(data, tree->data);
    
    if(cmp == 0){
        return tree;  // Found the data!
    }
    else if(cmp < 0){
        return search(tree->left, data, compare);  // Search left subtree
    }
    else {
        return search(tree->right, data, compare); // Search right subtree
    }
}
