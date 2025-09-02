#include "commit_similarity_tree.h"
#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// Helper function to find the commit index in the matrix
static int find_commit_index(CommitSimilarityTree* tree, Commit* commit) {
    if (!tree || !commit) return -1;
    
    for (int i = 0; i < tree->total_commits; i++) {
        if (tree->commit_index[i] == commit) {
            return i;
        }
    }
    return -1;
}

// Helper function to calculate all pairwise similarities
static List* calculate_all_similarities(List* commits) {
    if (!commits) return NULL;
    
    List* similarity_pairs = create_list();
    if (!similarity_pairs) return NULL;
    
    Node* node1 = get_first_node(commits);
    while (node1) {
        Commit* commit1 = (Commit*)get_node_data(node1);
        SemanticFingerprint* fp1 = create_semantic_fingerprint(commit1);
        
        Node* node2 = get_next_node(node1);
        while (node2) {
            Commit* commit2 = (Commit*)get_node_data(node2);
            SemanticFingerprint* fp2 = create_semantic_fingerprint(commit2);
            
            if (fp1 && fp2) {
                SimilarityPair* pair = malloc(sizeof(SimilarityPair));
                if (pair) {
                    pair->commit1 = commit1;
                    pair->commit2 = commit2;
                    pair->fp1 = fp1;
                    pair->fp2 = fp2;
                    pair->similarity_score = calculate_fingerprint_similarity(fp1, fp2);
                    
                    insert_item(similarity_pairs, pair);
                }
            }
            
            node2 = get_next_node(node2);
        }
        
        node1 = get_next_node(node1);
    }
    
    return similarity_pairs;
}

// Helper function to find the best placement for a commit in the tree
static SimilarityTreeNode* find_best_insertion_point(SimilarityTreeNode* root, SemanticFingerprint* target_fp) {
    if (!root || !target_fp) return NULL;
    
    double similarity_to_root = calculate_fingerprint_similarity(root->fingerprint, target_fp);
    
    // If this is more similar than both children, place it as a child of root
    if (!root->most_similar || 
        similarity_to_root > calculate_fingerprint_similarity(root->fingerprint, root->most_similar->fingerprint)) {
        return root;
    }
    
    if (!root->second_similar || 
        similarity_to_root > calculate_fingerprint_similarity(root->fingerprint, root->second_similar->fingerprint)) {
        return root;
    }
    
    // Otherwise, recurse to the most similar child
    double sim_to_left = root->most_similar ? 
        calculate_fingerprint_similarity(root->most_similar->fingerprint, target_fp) : 0.0;
    double sim_to_right = root->second_similar ? 
        calculate_fingerprint_similarity(root->second_similar->fingerprint, target_fp) : 0.0;
    
    if (sim_to_left > sim_to_right && root->most_similar) {
        return find_best_insertion_point(root->most_similar, target_fp);
    } else if (root->second_similar) {
        return find_best_insertion_point(root->second_similar, target_fp);
    }
    
    return root;
}

SimilarityTreeNode* create_similarity_node(Commit* commit, SemanticFingerprint* fingerprint) {
    if (!commit) return NULL;
    
    SimilarityTreeNode* node = malloc(sizeof(SimilarityTreeNode));
    if (!node) return NULL;
    
    node->commit = commit;
    node->fingerprint = fingerprint ? fingerprint : create_semantic_fingerprint(commit);
    node->most_similar = NULL;
    node->second_similar = NULL;
    node->parent = NULL;
    node->similarity_to_left = 0.0;
    node->similarity_to_right = 0.0;
    node->depth = 0;
    node->subtree_size = 1;
    
    return node;
}

void insert_commit_by_similarity(CommitSimilarityTree* tree, Commit* commit, SemanticFingerprint* fingerprint) {
    if (!tree || !commit) return;
    
    SimilarityTreeNode* new_node = create_similarity_node(commit, fingerprint);
    if (!new_node) return;
    
    if (!tree->root) {
        tree->root = new_node;
        insert_item(tree->all_nodes, new_node);
        tree->total_commits = 1;
        return;
    }
    
    // Find the best place to insert this commit
    SimilarityTreeNode* insertion_point = find_best_insertion_point(tree->root, new_node->fingerprint);
    if (!insertion_point) {
        free_similarity_tree_node(new_node);
        return;
    }
    
    // Calculate similarities
    double sim_to_insertion = calculate_fingerprint_similarity(insertion_point->fingerprint, new_node->fingerprint);
    
    // Place as most similar child if it's more similar than current most similar
    if (!insertion_point->most_similar || 
        sim_to_insertion > insertion_point->similarity_to_left) {
        
        // Shift current most similar to second similar
        if (insertion_point->most_similar) {
            insertion_point->second_similar = insertion_point->most_similar;
            insertion_point->similarity_to_right = insertion_point->similarity_to_left;
        }
        
        insertion_point->most_similar = new_node;
        insertion_point->similarity_to_left = sim_to_insertion;
    }
    // Otherwise place as second similar child
    else if (!insertion_point->second_similar || 
             sim_to_insertion > insertion_point->similarity_to_right) {
        insertion_point->second_similar = new_node;
        insertion_point->similarity_to_right = sim_to_insertion;
    }
    // If neither, we need to find a different insertion point
    else {
        // For now, just place as second similar (this could be optimized)
        insertion_point->second_similar = new_node;
        insertion_point->similarity_to_right = sim_to_insertion;
    }
    
    new_node->parent = insertion_point;
    new_node->depth = insertion_point->depth + 1;
    
    insert_item(tree->all_nodes, new_node);
    tree->total_commits++;
}

CommitSimilarityTree* build_similarity_tree(List* commits) {
    if (!commits || get_number_of_items(commits) == 0) return NULL;
    
    CommitSimilarityTree* tree = malloc(sizeof(CommitSimilarityTree));
    if (!tree) return NULL;
    
    tree->root = NULL;
    tree->all_nodes = create_list();
    tree->total_commits = 0;
    tree->average_similarity = 0.0;
    tree->similarity_matrix = NULL;
    tree->commit_index = NULL;
    
    if (!tree->all_nodes) {
        free(tree);
        return NULL;
    }
    
    printf("Building semantic similarity tree...\n");
    
    // Insert each commit into the tree
    Node* current = get_first_node(commits);
    int processed = 0;
    while (current) {
        Commit* commit = (Commit*)get_node_data(current);
        printf("Processing commit %d/%d\n", processed + 1, get_number_of_items(commits));
        
        SemanticFingerprint* fingerprint = create_semantic_fingerprint(commit);
        insert_commit_by_similarity(tree, commit, fingerprint);
        
        processed++;
        current = get_next_node(current);
    }
    
    // Build similarity matrix for fast lookups
    build_similarity_matrix(tree, commits);
    
    printf("Similarity tree built with %d commits\n", tree->total_commits);
    return tree;
}

void build_similarity_matrix(CommitSimilarityTree* tree, List* commits) {
    if (!tree || !commits) return;
    
    int num_commits = get_number_of_items(commits);
    
    // Allocate similarity matrix
    tree->similarity_matrix = malloc(num_commits * sizeof(double*));
    tree->commit_index = malloc(num_commits * sizeof(Commit*));
    
    if (!tree->similarity_matrix || !tree->commit_index) return;
    
    for (int i = 0; i < num_commits; i++) {
        tree->similarity_matrix[i] = malloc(num_commits * sizeof(double));
        if (!tree->similarity_matrix[i]) return;
    }
    
    // Build commit index
    Node* current = get_first_node(commits);
    int index = 0;
    while (current && index < num_commits) {
        tree->commit_index[index] = (Commit*)get_node_data(current);
        current = get_next_node(current);
        index++;
    }
    
    // Calculate similarity matrix
    double total_similarity = 0.0;
    int similarity_count = 0;
    
    for (int i = 0; i < num_commits; i++) {
        for (int j = 0; j < num_commits; j++) {
            if (i == j) {
                tree->similarity_matrix[i][j] = 1.0;
            } else {
                // Find the nodes corresponding to these commits
                SemanticFingerprint* fp1 = NULL;
                SemanticFingerprint* fp2 = NULL;
                
                Node* node = get_first_node(tree->all_nodes);
                while (node) {
                    SimilarityTreeNode* tree_node = (SimilarityTreeNode*)get_node_data(node);
                    if (tree_node->commit == tree->commit_index[i]) {
                        fp1 = tree_node->fingerprint;
                    }
                    if (tree_node->commit == tree->commit_index[j]) {
                        fp2 = tree_node->fingerprint;
                    }
                    node = get_next_node(node);
                }
                
                if (fp1 && fp2) {
                    double similarity = calculate_fingerprint_similarity(fp1, fp2);
                    tree->similarity_matrix[i][j] = similarity;
                    total_similarity += similarity;
                    similarity_count++;
                } else {
                    tree->similarity_matrix[i][j] = 0.0;
                }
            }
        }
    }
    
    tree->average_similarity = similarity_count > 0 ? total_similarity / similarity_count : 0.0;
}

double get_cached_similarity(CommitSimilarityTree* tree, Commit* commit1, Commit* commit2) {
    if (!tree || !tree->similarity_matrix || !commit1 || !commit2) return 0.0;
    
    int index1 = find_commit_index(tree, commit1);
    int index2 = find_commit_index(tree, commit2);
    
    if (index1 >= 0 && index2 >= 0 && 
        index1 < tree->total_commits && index2 < tree->total_commits) {
        return tree->similarity_matrix[index1][index2];
    }
    
    return 0.0;
}

void print_similarity_tree(SimilarityTreeNode* root, int depth, double min_similarity) {
    if (!root) return;
    
    // Print indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    // Print commit info
    printf("├─ Commit (depth %d) - Similarity: L=%.3f R=%.3f\n", 
           depth, root->similarity_to_left, root->similarity_to_right);
    
    // Print fingerprint summary
    if (root->fingerprint) {
        for (int i = 0; i < depth + 1; i++) {
            printf("  ");
        }
        printf("Intent: %s, Files: %d, Sentiment: %.2f\n",
               root->fingerprint->keywords ? root->fingerprint->keywords->intent_type : "unknown",
               root->fingerprint->file_analysis ? root->fingerprint->file_analysis->total_files_changed : 0,
               root->fingerprint->keywords ? root->fingerprint->keywords->sentiment_score : 0.0);
    }
    
    // Recursively print children if they meet the similarity threshold
    if (root->most_similar && root->similarity_to_left >= min_similarity) {
        print_similarity_tree(root->most_similar, depth + 1, min_similarity);
    }
    
    if (root->second_similar && root->similarity_to_right >= min_similarity) {
        print_similarity_tree(root->second_similar, depth + 1, min_similarity);
    }
}

void print_tree_statistics(CommitSimilarityTree* tree) {
    if (!tree) return;
    
    printf("\n========== COMMIT SIMILARITY TREE STATISTICS ==========\n");
    printf("Total Commits: %d\n", tree->total_commits);
    printf("Average Similarity: %.4f\n", tree->average_similarity);
    
    // Calculate tree depth
    int max_depth = 0;
    Node* current = get_first_node(tree->all_nodes);
    while (current) {
        SimilarityTreeNode* node = (SimilarityTreeNode*)get_node_data(current);
        if (node->depth > max_depth) {
            max_depth = node->depth;
        }
        current = get_next_node(current);
    }
    printf("Tree Depth: %d\n", max_depth);
    
    // Count nodes at each level
    printf("\nNodes per level:\n");
    for (int level = 0; level <= max_depth; level++) {
        int count = 0;
        current = get_first_node(tree->all_nodes);
        while (current) {
            SimilarityTreeNode* node = (SimilarityTreeNode*)get_node_data(current);
            if (node->depth == level) {
                count++;
            }
            current = get_next_node(current);
        }
        printf("  Level %d: %d nodes\n", level, count);
    }
    
    printf("=======================================================\n");
}

List* find_historical_precedents(CommitSimilarityTree* tree, Commit* current_commit, int max_results) {
    if (!tree || !current_commit) return NULL;
    
    List* precedents = create_list();
    if (!precedents) return NULL;
    
    // Find the node for the current commit
    SimilarityTreeNode* current_node = NULL;
    Node* node = get_first_node(tree->all_nodes);
    while (node) {
        SimilarityTreeNode* tree_node = (SimilarityTreeNode*)get_node_data(node);
        if (tree_node->commit == current_commit) {
            current_node = tree_node;
            break;
        }
        node = get_next_node(node);
    }
    
    if (!current_node) {
        free_list(precedents);
        return NULL;
    }
    
    // Find similar commits by traversing the tree and checking all nodes
    node = get_first_node(tree->all_nodes);
    while (node && get_number_of_items(precedents) < max_results) {
        SimilarityTreeNode* candidate = (SimilarityTreeNode*)get_node_data(node);
        
        if (candidate != current_node) {
            double similarity = calculate_fingerprint_similarity(
                current_node->fingerprint, candidate->fingerprint);
            
            if (similarity > 0.3) { // Minimum threshold for precedents
                HistoricalPrecedent* precedent = malloc(sizeof(HistoricalPrecedent));
                if (precedent) {
                    precedent->similar_commit = candidate->commit;
                    precedent->similarity_score = similarity;
                    
                    // Create explanation based on similarity
                    precedent->similarity_explanation = malloc(256);
                    if (precedent->similarity_explanation) {
                        if (similarity > 0.7) {
                            strcpy(precedent->similarity_explanation, "Very similar commit pattern");
                        } else if (similarity > 0.5) {
                            strcpy(precedent->similarity_explanation, "Similar development approach");
                        } else {
                            strcpy(precedent->similarity_explanation, "Somewhat related change");
                        }
                    }
                    
                    precedent->common_characteristics = create_list();
                    insert_item(precedents, precedent);
                }
            }
        }
        
        node = get_next_node(node);
    }
    
    return precedents;
}

void print_historical_precedents(List* precedents) {
    if (!precedents) return;
    
    printf("\n========== HISTORICAL PRECEDENTS ==========\n");
    
    Node* current = get_first_node(precedents);
    int count = 1;
    while (current) {
        HistoricalPrecedent* precedent = (HistoricalPrecedent*)get_node_data(current);
        
        printf("%d. Similarity: %.3f - %s\n", 
               count, precedent->similarity_score, 
               precedent->similarity_explanation ? precedent->similarity_explanation : "Unknown");
        
        // Print basic commit info (would need access to commit data)
        printf("   Commit: [commit details would be shown here]\n");
        
        count++;
        current = get_next_node(current);
    }
    
    printf("==========================================\n");
}

// Cleanup functions
void free_similarity_tree_node(SimilarityTreeNode* node) {
    if (!node) return;
    
    free_semantic_fingerprint(node->fingerprint);
    free(node);
}

void free_commit_similarity_tree(CommitSimilarityTree* tree) {
    if (!tree) return;
    
    // Free similarity matrix
    if (tree->similarity_matrix) {
        for (int i = 0; i < tree->total_commits; i++) {
            free(tree->similarity_matrix[i]);
        }
        free(tree->similarity_matrix);
    }
    
    free(tree->commit_index);
    
    // Free all nodes
    if (tree->all_nodes) {
        Node* current = get_first_node(tree->all_nodes);
        while (current) {
            SimilarityTreeNode* tree_node = (SimilarityTreeNode*)get_node_data(current);
            free_similarity_tree_node(tree_node);
            current = get_next_node(current);
        }
        free_list(tree->all_nodes);
    }
    
    free(tree);
}

void free_historical_precedent(HistoricalPrecedent* precedent) {
    if (!precedent) return;
    
    free(precedent->similarity_explanation);
    if (precedent->common_characteristics) {
        free_list(precedent->common_characteristics);
    }
    free(precedent);
}

void free_similarity_pair(SimilarityPair* pair) {
    if (!pair) return;
    
    // Note: fingerprints are owned by the tree nodes, don't free them here
    free(pair);
}
