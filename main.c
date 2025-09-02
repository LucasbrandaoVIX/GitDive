#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "commit.h"
#include "git_local.h"
#include "binary_search_tree.h"
#include "semantic_fingerprint.h"
#include "commit_similarity_tree.h"

// Callback used to insert each commit into the BST when traversing the list.
// - item: pointer to a Commit
// - ctx: pointer to TreeNode* (i.e., &timestamp_tree), so we can update the root as we insert
static void* insert_commit_to_bst(void* item, void* ctx){
    TreeNode** tree_ref = (TreeNode**)ctx;
    *tree_ref = insert_bst(*tree_ref, item, compare_commits_by_timestamp);
    return NULL; // return non-NULL to stop traversal early (not used here)
}

int main(){
    printf("GitDive - Commit-Genealogy: Semantic Change Similarity Explorer\n");
    printf("================================================================\n\n");
    
    // Check if we're in a git repository
    if (!is_git_repository()) {
        printf("Error: Not in a git repository.\n");
        printf("Please run this program from within a git repository.\n");
        return 1;
    }
    
    // Get repository root path
    char* repo_path = malloc(1024); // Allocate dynamic buffer
    if (!repo_path) {
        printf("Error: Memory allocation failed\n");
        return 1;
    }
    
    if (get_git_root(repo_path, 1024)) {
        printf("Repository root: %s\n\n", repo_path);
    }
    
    free(repo_path); // Free the allocated buffer
    
    // Get real commits from the local git repository
    printf("Fetching commits from local git repository...\n");
    List* commit_list = get_git_commits(15); // Get more commits for better analysis
    
    if (commit_list == NULL) {
        printf("Error: Could not retrieve commits from git repository.\n");
        printf("Please ensure:\n");
        printf("1. Git is installed and available in PATH\n");
        printf("2. The repository has commits\n");
        return 1;
    }
    
    // Print the entire commit list with real git data
    printf("\n========== COMMIT HISTORY ==========\n");
    print_list(commit_list, (void (*)(void*))print_commit);
    printf("====================================\n");
    
    // Build a traditional BST of commits ordered by timestamp (for comparison)
    TreeNode* timestamp_tree = NULL;
    if (commit_list && get_number_of_items(commit_list) > 0) {
        // Insert each commit pointer into the BST using the timestamp comparator
        traverse_list_with_context(commit_list, insert_commit_to_bst, &timestamp_tree);

        printf("\n========== TRADITIONAL BST (by timestamp) =========\n");
        print_btree(timestamp_tree, print_commit_for_tree, 0);
        printf("===================================================\n");
    }

    // === NEW: COMMIT-GENEALOGY ANALYSIS ===
    
    printf("\n========== SEMANTIC FINGERPRINT ANALYSIS ==========\n");
    
    // Create semantic fingerprints for a few commits and show them
    Node* current = get_first_node(commit_list);
    int fingerprint_count = 0;
    while (current && fingerprint_count < 3) {
        Commit* commit = (Commit*)get_node_data(current);
        printf("\nFingerprint for Commit ID %d:\n", get_commit_id(commit));
        
        SemanticFingerprint* fp = create_semantic_fingerprint(commit);
        if (fp) {
            print_semantic_fingerprint(fp);
            free_semantic_fingerprint(fp);
        }
        
        fingerprint_count++;
        current = get_next_node(current);
    }
    printf("===================================================\n");
    
    // Build the semantic similarity tree
    printf("\n========== BUILDING COMMIT SIMILARITY TREE ==========\n");
    CommitSimilarityTree* similarity_tree = build_similarity_tree(commit_list);
    
    if (similarity_tree) {
        // Print tree statistics
        print_tree_statistics(similarity_tree);
        
        // Print the similarity tree structure
        printf("\n========== SEMANTIC SIMILARITY TREE ==========\n");
        printf("Tree organized by semantic similarity (not chronological order)\n");
        printf("Left child = most similar commit, Right child = second most similar\n\n");
        print_similarity_tree(similarity_tree->root, 0, 0.2); // Show similarities >= 0.2
        printf("=============================================\n");
        
        // Demonstrate historical precedent search
        if (similarity_tree->root) {
            printf("\n========== HISTORICAL PRECEDENT SEARCH ==========\n");
            printf("Finding commits similar to the root commit...\n");
            
            List* precedents = find_historical_precedents(similarity_tree, similarity_tree->root->commit, 5);
            if (precedents) {
                print_historical_precedents(precedents);
                
                // Cleanup precedents
                Node* prec_node = get_first_node(precedents);
                while (prec_node) {
                    HistoricalPrecedent* prec = (HistoricalPrecedent*)get_node_data(prec_node);
                    free_historical_precedent(prec);
                    prec_node = get_next_node(prec_node);
                }
                free_list(precedents);
            }
            printf("================================================\n");
        }
        
        // Demonstrate similarity comparison between commits
        if (get_number_of_items(commit_list) >= 2) {
            printf("\n========== COMMIT SIMILARITY COMPARISON ==========\n");
            
            Node* node1 = get_first_node(commit_list);
            Node* node2 = get_next_node(node1);
            
            if (node1 && node2) {
                Commit* commit1 = (Commit*)get_node_data(node1);
                Commit* commit2 = (Commit*)get_node_data(node2);
                
                SemanticFingerprint* fp1 = create_semantic_fingerprint(commit1);
                SemanticFingerprint* fp2 = create_semantic_fingerprint(commit2);
                
                if (fp1 && fp2) {
                    printf("Comparing first two commits:\n");
                    printf("Commit 1 ID: %d\n", get_commit_id(commit1));
                    printf("Commit 2 ID: %d\n", get_commit_id(commit2));
                    print_fingerprint_similarity(fp1, fp2);
                    
                    free_semantic_fingerprint(fp1);
                    free_semantic_fingerprint(fp2);
                }
            }
            printf("==================================================\n");
        }
        
        // Clean up similarity tree
        free_commit_similarity_tree(similarity_tree);
    } else {
        printf("Failed to build similarity tree.\n");
    }

    // Clean up traditional BST
    free_tree(timestamp_tree);

    cleanup_git_data();
    
    printf("\n========== COMMIT-GENEALOGY ANALYSIS COMPLETE ==========\n");
    printf("This tool reveals the hidden conceptual relationships between commits,\n");
    printf("moving beyond simple chronological order to show semantic similarity.\n");
    printf("========================================================\n");
    
    return 0;
}