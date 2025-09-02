#ifndef COMMIT_SIMILARITY_TREE_H
#define COMMIT_SIMILARITY_TREE_H

#include "commit.h"
#include "semantic_fingerprint.h"
#include "list.h"

typedef struct SimilarityTreeNode SimilarityTreeNode;
typedef struct CommitSimilarityTree CommitSimilarityTree;
typedef struct SimilarityPair SimilarityPair;

/**
 * Node in the commit similarity tree
 * Unlike a traditional BST, this tree is organized by semantic similarity
 */
struct SimilarityTreeNode {
    Commit* commit;
    SemanticFingerprint* fingerprint;
    
    // Left child: most semantically similar commit
    SimilarityTreeNode* most_similar;
    
    // Right child: second most similar commit
    SimilarityTreeNode* second_similar;
    
    // Metadata for tree navigation
    double similarity_to_left;   // Similarity score to left child
    double similarity_to_right;  // Similarity score to right child
    
    // Back-reference to parent (useful for navigation)
    SimilarityTreeNode* parent;
    
    // Tree metadata
    int depth;
    int subtree_size;
};

/**
 * Represents a pair of commits and their similarity score
 * Used during tree construction
 */
struct SimilarityPair {
    Commit* commit1;
    Commit* commit2;
    SemanticFingerprint* fp1;
    SemanticFingerprint* fp2;
    double similarity_score;
};

/**
 * The main commit similarity tree structure
 */
struct CommitSimilarityTree {
    SimilarityTreeNode* root;
    List* all_nodes;           // Keep track of all nodes for cleanup
    int total_commits;
    double average_similarity; // Average similarity in the tree
    
    // Cached similarity matrix for quick lookups
    double** similarity_matrix;
    Commit** commit_index;     // Maps matrix indices to commits
};

// Core tree construction functions
CommitSimilarityTree* build_similarity_tree(List* commits);
SimilarityTreeNode* create_similarity_node(Commit* commit, SemanticFingerprint* fingerprint);
void insert_commit_by_similarity(CommitSimilarityTree* tree, Commit* commit, SemanticFingerprint* fingerprint);

// Tree analysis and navigation
SimilarityTreeNode* find_most_similar_commits(CommitSimilarityTree* tree, Commit* target_commit, int max_results);
List* get_similarity_cluster(CommitSimilarityTree* tree, Commit* center_commit, double min_similarity);
List* find_commits_by_pattern(CommitSimilarityTree* tree, const char* intent_type, const char* module);

// Similarity matrix operations
void build_similarity_matrix(CommitSimilarityTree* tree, List* commits);
double get_cached_similarity(CommitSimilarityTree* tree, Commit* commit1, Commit* commit2);
List* get_top_similar_pairs(CommitSimilarityTree* tree, int top_n);

// Tree visualization and analysis
void print_similarity_tree(SimilarityTreeNode* root, int depth, double min_similarity);
void print_commit_clusters(CommitSimilarityTree* tree);
void print_tree_statistics(CommitSimilarityTree* tree);
void print_similarity_path(SimilarityTreeNode* from, SimilarityTreeNode* to);

// Development pattern analysis
typedef struct {
    char* pattern_name;
    int commit_count;
    double avg_similarity;
    List* representative_commits;
} DevelopmentPattern;

List* analyze_development_patterns(CommitSimilarityTree* tree);
void print_development_patterns(List* patterns);
void free_development_pattern(DevelopmentPattern* pattern);

// Historical precedent search
typedef struct {
    Commit* similar_commit;
    double similarity_score;
    char* similarity_explanation;
    List* common_characteristics;
} HistoricalPrecedent;

List* find_historical_precedents(CommitSimilarityTree* tree, Commit* current_commit, int max_results);
void print_historical_precedents(List* precedents);
void free_historical_precedent(HistoricalPrecedent* precedent);

// Tree maintenance and optimization
void balance_similarity_tree(CommitSimilarityTree* tree);
void update_tree_with_new_commit(CommitSimilarityTree* tree, Commit* new_commit);
void optimize_tree_structure(CommitSimilarityTree* tree);

// Cleanup functions
void free_similarity_tree_node(SimilarityTreeNode* node);
void free_commit_similarity_tree(CommitSimilarityTree* tree);
void free_similarity_pair(SimilarityPair* pair);

// Utility functions for tree traversal
typedef void (*similarity_node_visitor)(SimilarityTreeNode* node, void* context);
void traverse_similarity_tree(SimilarityTreeNode* root, similarity_node_visitor visitor, void* context);
void traverse_by_similarity_threshold(SimilarityTreeNode* root, double threshold, similarity_node_visitor visitor, void* context);

#endif
