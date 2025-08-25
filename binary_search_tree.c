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
    print_btree(tree->right, print_function, depth + 1);
    
    // Print current node with indentation
    for(int i = 0; i < depth; i++){
        printf("    ");
    }
    printf("└─ ");
    print_function(tree->data);
    printf("\n");
    
    // Print left subtree
    print_btree(tree->left, print_function, depth + 1);
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


void free_tree(TreeNode* tree){

    if(tree != NULL){
        free_tree(tree->left);
        free_tree(tree->right);
        free(tree);
    }

}


// Helper function to find the minimum node in a subtree
TreeNode* find_min(TreeNode* tree){
    while(tree->left != NULL){
        tree = tree->left;
    }
    return tree;
}

TreeNode* delete_node(TreeNode* tree, void* data, int (*compare)(void* a, void* b)){
    
    // Base case: tree is empty
    if(tree == NULL){
        return tree;
    }
    
    int cmp = compare(data, tree->data);
    
    // If data is smaller, delete from left subtree
    if(cmp < 0){
        tree->left = delete_node(tree->left, data, compare);
    }
    // If data is larger, delete from right subtree
    else if(cmp > 0){
        tree->right = delete_node(tree->right, data, compare);
    }
    // Found the node to delete
    else {
        // Case 1: Node has no children (leaf node)
        if(tree->left == NULL && tree->right == NULL){
            free(tree);
            return NULL;
        }
        // Case 2: Node has only right child
        else if(tree->left == NULL){
            TreeNode* temp = tree->right;
            free(tree);
            return temp;
        }
        // Case 3: Node has only left child
        else if(tree->right == NULL){
            TreeNode* temp = tree->left;
            free(tree);
            return temp;
        }
        // Case 4: Node has both children
        else {
            // Find the inorder successor (smallest in right subtree)
            TreeNode* successor = find_min(tree->right);
            
            // Replace node's data with successor's data
            tree->data = successor->data;
            
            // Delete the successor
            tree->right = delete_node(tree->right, successor->data, compare);
        }
    }
    
    return tree;
}

