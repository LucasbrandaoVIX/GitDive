#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "commit.h"
#include "git_local.h"
#include "binary_search_tree.h"

// Callback used to insert each commit into the BST when traversing the list.
// - item: pointer to a Commit
// - ctx: pointer to TreeNode* (i.e., &timestamp_tree), so we can update the root as we insert
static void* insert_commit_to_bst(void* item, void* ctx){
    TreeNode** tree_ref = (TreeNode**)ctx;
    *tree_ref = insert_bst(*tree_ref, item, compare_commits_by_timestamp);
    return NULL; // return non-NULL to stop traversal early (not used here)
}

int main(){
    printf("GitDive - Real Git Repository Analysis\n");
    printf("======================================\n\n");
    
    // Check if we're in a git repository
    if (!is_git_repository()) {
        printf("Error: Not in a git repository.\n");
        printf("Please run this program from within a git repository.\n");
        return 1;
    }
    
    // Get repository root path
    char repo_path[512];
    if (get_git_root(repo_path, sizeof(repo_path))) {
        printf("Repository root: %s\n\n", repo_path);
    }
    
    // Get real commits from the local git repository
    printf("Fetching commits from local git repository...\n");
    List* commit_list = get_git_commits(10); // Get last 10 commits
    
    if (commit_list == NULL) {
        printf("Error: Could not retrieve commits from git repository.\n");
        printf("Please ensure:\n");
        printf("1. Git is installed and available in PATH\n");
        printf("2. The repository has commits\n");
        return 1;
    }
    
    // Print the entire commit list with real git data
    printf("\n========== REAL COMMIT HISTORY ==========\n");
    print_list(commit_list, (void (*)(void*))print_commit);
    printf("=========================================\n");
    
    // Build a BST of commits ordered by timestamp (older on the left)
    TreeNode* timestamp_tree = NULL;
    if (commit_list && get_number_of_items(commit_list) > 0) {
        // Insert each commit pointer into the BST using the timestamp comparator
        traverse_list_with_context(commit_list, insert_commit_to_bst, &timestamp_tree);

    printf("\n========== COMMITS BST (by timestamp) =========\n");
    print_btree(timestamp_tree, print_commit_for_tree, 0);
        printf("===============================================\n");
    }


    // Clean up
    // Free the BST nodes (commit objects are owned by the list for now)
    free_tree(timestamp_tree);

    cleanup_git_data();
    
    return 0;
}