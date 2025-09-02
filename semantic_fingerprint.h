#ifndef SEMANTIC_FINGERPRINT_H
#define SEMANTIC_FINGERPRINT_H

#include "commit.h"
#include <stddef.h>

// Semantic fingerprint dimensions
#define FINGERPRINT_DIMENSIONS 32
#define MAX_KEYWORDS 20
#define MAX_FILE_PATTERNS 10

typedef struct SemanticFingerprint SemanticFingerprint;
typedef struct CommitKeywords CommitKeywords;
typedef struct FilePathAnalysis FilePathAnalysis;
typedef struct CodeDeltaAnalysis CodeDeltaAnalysis;

/**
 * Represents the semantic fingerprint of a commit
 * This is a high-dimensional vector that captures the "meaning" of a change
 */
struct SemanticFingerprint {
    // Core vector representation (normalized to unit length)
    double vector[FINGERPRINT_DIMENSIONS];
    
    // Components that contribute to the fingerprint
    CodeDeltaAnalysis* code_delta;
    FilePathAnalysis* file_analysis;
    CommitKeywords* keywords;
    
    // Metadata
    double magnitude;  // Original magnitude before normalization
    char* commit_hash; // Reference to the commit this fingerprint represents
};

/**
 * Keywords extracted from commit message using NLP-style analysis
 */
struct CommitKeywords {
    char* intent_type;        // "fix", "feat", "refactor", "docs", "chore", etc.
    char* keywords[MAX_KEYWORDS];
    int keyword_count;
    char* ticket_reference;   // JIRA-123, #456, etc.
    double sentiment_score;   // -1.0 (negative) to 1.0 (positive)
};

/**
 * Analysis of where changes occurred in the codebase
 */
struct FilePathAnalysis {
    char* primary_module;     // src/, test/, docs/, config/, etc.
    char* patterns[MAX_FILE_PATTERNS];
    int pattern_count;
    double file_diversity;    // How spread out the changes are
    int total_files_changed;
};

/**
 * Analysis of the semantic distance of code changes
 */
struct CodeDeltaAnalysis {
    double semantic_distance; // How much the meaning changed (0.0 to 1.0)
    int lines_added;
    int lines_removed;
    int functions_modified;
    int classes_modified;
    double complexity_delta;   // Change in cyclomatic complexity
};

// Core functions for creating and managing semantic fingerprints
SemanticFingerprint* create_semantic_fingerprint(Commit* commit);
void free_semantic_fingerprint(SemanticFingerprint* fingerprint);

// Similarity calculation
double calculate_fingerprint_similarity(const SemanticFingerprint* fp1, const SemanticFingerprint* fp2);
double cosine_similarity(const double* vec1, const double* vec2, int dimensions);

// Component analysis functions
CommitKeywords* analyze_commit_message(const char* message);
FilePathAnalysis* analyze_file_paths(List* modifications);
CodeDeltaAnalysis* analyze_code_delta(List* modifications);

// Utility functions
void normalize_fingerprint_vector(SemanticFingerprint* fingerprint);
void print_semantic_fingerprint(const SemanticFingerprint* fingerprint);
void print_fingerprint_similarity(const SemanticFingerprint* fp1, const SemanticFingerprint* fp2);

// Cleanup functions
void free_commit_keywords(CommitKeywords* keywords);
void free_file_path_analysis(FilePathAnalysis* analysis);
void free_code_delta_analysis(CodeDeltaAnalysis* analysis);

#endif
