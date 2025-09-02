#include "semantic_fingerprint.h"
#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Helper function to extract intent from commit message
static char* extract_intent_type(const char* message) {
    if (!message) return NULL;
    
    // Convert to lowercase for analysis
    char* lower_msg = malloc(strlen(message) + 1);
    if (!lower_msg) return NULL;
    
    for (int i = 0; message[i]; i++) {
        lower_msg[i] = tolower(message[i]);
    }
    lower_msg[strlen(message)] = '\0';
    
    char* intent = NULL;
    
    // Check for conventional commit patterns
    if (strstr(lower_msg, "fix:") || strstr(lower_msg, "bug") || strstr(lower_msg, "error")) {
        intent = malloc(4);
        if (intent) strcpy(intent, "fix");
    } else if (strstr(lower_msg, "feat:") || strstr(lower_msg, "add") || strstr(lower_msg, "new")) {
        intent = malloc(5);
        if (intent) strcpy(intent, "feat");
    } else if (strstr(lower_msg, "refactor") || strstr(lower_msg, "restructure") || strstr(lower_msg, "cleanup")) {
        intent = malloc(9);
        if (intent) strcpy(intent, "refactor");
    } else if (strstr(lower_msg, "docs:") || strstr(lower_msg, "documentation") || strstr(lower_msg, "readme")) {
        intent = malloc(5);
        if (intent) strcpy(intent, "docs");
    } else if (strstr(lower_msg, "test") || strstr(lower_msg, "spec")) {
        intent = malloc(5);
        if (intent) strcpy(intent, "test");
    } else {
        intent = malloc(6);
        if (intent) strcpy(intent, "chore");
    }
    
    free(lower_msg);
    return intent;
}

// Helper function to extract ticket references
static char* extract_ticket_reference(const char* message) {
    if (!message) return NULL;
    
    // Look for patterns like JIRA-123, #456, etc.
    char* ticket = NULL;
    
    // Search for #number pattern
    char* hash_pos = strchr(message, '#');
    if (hash_pos) {
        int num_start = 1;
        int num_end = num_start;
        while (hash_pos[num_end] && isdigit(hash_pos[num_end])) {
            num_end++;
        }
        if (num_end > num_start) {
            int len = num_end;
            ticket = malloc(len + 1);
            if (ticket) {
                strncpy(ticket, hash_pos, len);
                ticket[len] = '\0';
            }
        }
    }
    
    // Search for WORD-NUMBER pattern (like JIRA-123)
    if (!ticket) {
        const char* pos = message;
        while (*pos) {
            if (isalpha(*pos)) {
                const char* word_start = pos;
                while (*pos && (isalnum(*pos) || *pos == '_')) pos++;
                if (*pos == '-' && isdigit(*(pos + 1))) {
                    const char* num_start = pos + 1;
                    while (*pos && isdigit(*pos)) pos++;
                    int total_len = pos - word_start;
                    ticket = malloc(total_len + 1);
                    if (ticket) {
                        strncpy(ticket, word_start, total_len);
                        ticket[total_len] = '\0';
                    }
                    break;
                }
            } else {
                pos++;
            }
        }
    }
    
    return ticket;
}

// Helper function to calculate sentiment score (simplified)
static double calculate_sentiment_score(const char* message) {
    if (!message) return 0.0;
    
    double score = 0.0;
    char* lower_msg = malloc(strlen(message) + 1);
    if (!lower_msg) return 0.0;
    
    for (int i = 0; message[i]; i++) {
        lower_msg[i] = tolower(message[i]);
    }
    lower_msg[strlen(message)] = '\0';
    
    // Positive indicators
    if (strstr(lower_msg, "improve")) score += 0.3;
    if (strstr(lower_msg, "optimize")) score += 0.3;
    if (strstr(lower_msg, "enhance")) score += 0.3;
    if (strstr(lower_msg, "add")) score += 0.2;
    if (strstr(lower_msg, "better")) score += 0.2;
    if (strstr(lower_msg, "clean")) score += 0.2;
    
    // Negative indicators
    if (strstr(lower_msg, "fix")) score -= 0.2;
    if (strstr(lower_msg, "bug")) score -= 0.3;
    if (strstr(lower_msg, "error")) score -= 0.3;
    if (strstr(lower_msg, "issue")) score -= 0.2;
    if (strstr(lower_msg, "problem")) score -= 0.3;
    if (strstr(lower_msg, "fail")) score -= 0.3;
    
    free(lower_msg);
    
    // Clamp to [-1.0, 1.0]
    if (score > 1.0) score = 1.0;
    if (score < -1.0) score = -1.0;
    
    return score;
}

CommitKeywords* analyze_commit_message(const char* message) {
    if (!message) return NULL;
    
    CommitKeywords* keywords = malloc(sizeof(CommitKeywords));
    if (!keywords) return NULL;
    
    keywords->intent_type = extract_intent_type(message);
    keywords->ticket_reference = extract_ticket_reference(message);
    keywords->sentiment_score = calculate_sentiment_score(message);
    keywords->keyword_count = 0;
    
    // Initialize keywords array
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        keywords->keywords[i] = NULL;
    }
    
    // Simple keyword extraction (words longer than 3 characters)
    char* msg_copy = malloc(strlen(message) + 1);
    if (!msg_copy) {
        free_commit_keywords(keywords);
        return NULL;
    }
    strcpy(msg_copy, message);
    
    char* token = strtok(msg_copy, " \t\n\r.,;:!?()[]{}");
    while (token && keywords->keyword_count < MAX_KEYWORDS) {
        if (strlen(token) > 3) {
            keywords->keywords[keywords->keyword_count] = malloc(strlen(token) + 1);
            if (keywords->keywords[keywords->keyword_count]) {
                strcpy(keywords->keywords[keywords->keyword_count], token);
                keywords->keyword_count++;
            }
        }
        token = strtok(NULL, " \t\n\r.,;:!?()[]{}");
    }
    
    free(msg_copy);
    return keywords;
}

FilePathAnalysis* analyze_file_paths(List* modifications) {
    if (!modifications) return NULL;
    
    FilePathAnalysis* analysis = malloc(sizeof(FilePathAnalysis));
    if (!analysis) return NULL;
    
    analysis->primary_module = NULL;
    analysis->pattern_count = 0;
    analysis->file_diversity = 0.0;
    analysis->total_files_changed = get_number_of_items(modifications);
    
    // Initialize patterns array
    for (int i = 0; i < MAX_FILE_PATTERNS; i++) {
        analysis->patterns[i] = NULL;
    }
    
    // Count module types
    int src_count = 0, test_count = 0, docs_count = 0, config_count = 0, other_count = 0;
    
    Node* current = get_first_node(modifications);
    while (current) {
        Modification* mod = (Modification*)get_node_data(current);
        char* filepath = get_modification_filepath(mod);
        
        if (filepath) {
            // Simple classification based on common patterns
            if (strstr(filepath, "src/") || strstr(filepath, "lib/") || strstr(filepath, ".c") || strstr(filepath, ".cpp") || strstr(filepath, ".h")) {
                src_count++;
            } else if (strstr(filepath, "test") || strstr(filepath, "spec")) {
                test_count++;
            } else if (strstr(filepath, "doc") || strstr(filepath, "README") || strstr(filepath, ".md")) {
                docs_count++;
            } else if (strstr(filepath, "config") || strstr(filepath, ".json") || strstr(filepath, ".yml") || strstr(filepath, "Makefile")) {
                config_count++;
            } else {
                other_count++;
            }
        }
        
        current = get_next_node(current);
    }
    
    // Determine primary module
    if (src_count > test_count && src_count > docs_count && src_count > config_count) {
        analysis->primary_module = malloc(4);
        if (analysis->primary_module) strcpy(analysis->primary_module, "src");
    } else if (test_count > docs_count && test_count > config_count) {
        analysis->primary_module = malloc(5);
        if (analysis->primary_module) strcpy(analysis->primary_module, "test");
    } else if (docs_count > config_count) {
        analysis->primary_module = malloc(5);
        if (analysis->primary_module) strcpy(analysis->primary_module, "docs");
    } else {
        analysis->primary_module = malloc(7);
        if (analysis->primary_module) strcpy(analysis->primary_module, "config");
    }
    
    // Calculate file diversity (simplified)
    int total = src_count + test_count + docs_count + config_count + other_count;
    if (total > 0) {
        double entropy = 0.0;
        if (src_count > 0) entropy -= ((double)src_count / total) * log2((double)src_count / total);
        if (test_count > 0) entropy -= ((double)test_count / total) * log2((double)test_count / total);
        if (docs_count > 0) entropy -= ((double)docs_count / total) * log2((double)docs_count / total);
        if (config_count > 0) entropy -= ((double)config_count / total) * log2((double)config_count / total);
        if (other_count > 0) entropy -= ((double)other_count / total) * log2((double)other_count / total);
        analysis->file_diversity = entropy / log2(5.0); // Normalize to [0,1]
    }
    
    return analysis;
}

CodeDeltaAnalysis* analyze_code_delta(List* modifications) {
    if (!modifications) return NULL;
    
    CodeDeltaAnalysis* analysis = malloc(sizeof(CodeDeltaAnalysis));
    if (!analysis) return NULL;
    
    analysis->semantic_distance = 0.0;
    analysis->lines_added = 0;
    analysis->lines_removed = 0;
    analysis->functions_modified = 0;
    analysis->classes_modified = 0;
    analysis->complexity_delta = 0.0;
    
    Node* current = get_first_node(modifications);
    while (current) {
        Modification* mod = (Modification*)get_node_data(current);
        char* old_code = get_modification_old_code(mod);
        char* new_code = get_modification_new_code(mod);
        
        // Count lines in old and new code
        if (old_code) {
            char* line = old_code;
            while ((line = strchr(line, '\n')) != NULL) {
                analysis->lines_removed++;
                line++;
            }
        }
        
        if (new_code) {
            char* line = new_code;
            while ((line = strchr(line, '\n')) != NULL) {
                analysis->lines_added++;
                line++;
            }
        }
        
        // Simple heuristic: assume each modification affects at least one function
        analysis->functions_modified++;
        
        // Count classes (simple heuristic for C: look for struct definitions)
        if (new_code && (strstr(new_code, "struct ") || strstr(new_code, "typedef struct"))) {
            analysis->classes_modified++;
        }
        
        current = get_next_node(current);
    }
    
    // Calculate semantic distance based on the amount of change
    int total_changes = analysis->lines_added + analysis->lines_removed;
    int total_modifications = get_number_of_items(modifications);
    
    if (total_modifications > 0) {
        // Higher semantic distance for more changes per modification
        analysis->semantic_distance = (double)total_changes / (total_modifications * 20.0); // Adjusted scale
        if (analysis->semantic_distance > 1.0) analysis->semantic_distance = 1.0;
        
        // Calculate complexity delta based on added vs removed lines
        if (analysis->lines_removed > 0) {
            analysis->complexity_delta = (double)(analysis->lines_added - analysis->lines_removed) / analysis->lines_removed;
        } else if (analysis->lines_added > 0) {
            analysis->complexity_delta = 1.0; // Pure addition
        }
        
        // Clamp complexity delta to reasonable range
        if (analysis->complexity_delta > 2.0) analysis->complexity_delta = 2.0;
        if (analysis->complexity_delta < -2.0) analysis->complexity_delta = -2.0;
    }
    
    return analysis;
}

SemanticFingerprint* create_semantic_fingerprint(Commit* commit) {
    if (!commit) return NULL;
    
    SemanticFingerprint* fingerprint = malloc(sizeof(SemanticFingerprint));
    if (!fingerprint) return NULL;
    
    // Initialize vector to zero
    for (int i = 0; i < FINGERPRINT_DIMENSIONS; i++) {
        fingerprint->vector[i] = 0.0;
    }
    
    fingerprint->magnitude = 0.0;
    fingerprint->commit_hash = NULL; // We'll need to add hash to Commit struct
    
    // Analyze components using the actual commit data
    char* commit_message = get_commit_message(commit);
    List* modifications = get_commit_modifications(commit);
    
    fingerprint->keywords = analyze_commit_message(commit_message);
    fingerprint->file_analysis = analyze_file_paths(modifications);
    fingerprint->code_delta = analyze_code_delta(modifications);
    
    if (!fingerprint->keywords || !fingerprint->file_analysis || !fingerprint->code_delta) {
        free_semantic_fingerprint(fingerprint);
        return NULL;
    }
    
    // Build the fingerprint vector from components
    int dim = 0;
    
    // Encode intent type (dimensions 0-7)
    if (fingerprint->keywords->intent_type) {
        if (strcmp(fingerprint->keywords->intent_type, "fix") == 0) {
            fingerprint->vector[0] = 1.0;
        } else if (strcmp(fingerprint->keywords->intent_type, "feat") == 0) {
            fingerprint->vector[1] = 1.0;
        } else if (strcmp(fingerprint->keywords->intent_type, "refactor") == 0) {
            fingerprint->vector[2] = 1.0;
        } else if (strcmp(fingerprint->keywords->intent_type, "docs") == 0) {
            fingerprint->vector[3] = 1.0;
        } else if (strcmp(fingerprint->keywords->intent_type, "test") == 0) {
            fingerprint->vector[4] = 1.0;
        } else {
            fingerprint->vector[5] = 1.0; // chore
        }
    }
    dim += 8;
    
    // Encode sentiment (dimension 8)
    fingerprint->vector[8] = fingerprint->keywords->sentiment_score;
    dim++;
    
    // Encode file analysis (dimensions 9-15)
    fingerprint->vector[9] = fingerprint->file_analysis->file_diversity;
    fingerprint->vector[10] = (double)fingerprint->file_analysis->total_files_changed / 10.0; // Normalize
    if (fingerprint->file_analysis->primary_module) {
        if (strcmp(fingerprint->file_analysis->primary_module, "src") == 0) {
            fingerprint->vector[11] = 1.0;
        } else if (strcmp(fingerprint->file_analysis->primary_module, "test") == 0) {
            fingerprint->vector[12] = 1.0;
        } else if (strcmp(fingerprint->file_analysis->primary_module, "docs") == 0) {
            fingerprint->vector[13] = 1.0;
        } else {
            fingerprint->vector[14] = 1.0; // config/other
        }
    }
    dim += 7;
    
    // Encode code delta (dimensions 16-23)
    fingerprint->vector[16] = fingerprint->code_delta->semantic_distance;
    fingerprint->vector[17] = (double)fingerprint->code_delta->lines_added / 100.0;    // Normalize
    fingerprint->vector[18] = (double)fingerprint->code_delta->lines_removed / 100.0;  // Normalize
    fingerprint->vector[19] = (double)fingerprint->code_delta->functions_modified / 10.0; // Normalize
    fingerprint->vector[20] = (double)fingerprint->code_delta->classes_modified / 5.0;    // Normalize
    fingerprint->vector[21] = fingerprint->code_delta->complexity_delta / 2.0; // Normalize to [-1,1]
    dim += 8;
    
    // Add timestamp influence (dimensions 22-23)
    long long timestamp = get_commit_timestamp(commit);
    if (timestamp > 0) {
        // Use timestamp to add temporal context (normalized)
        double time_factor = (double)(timestamp % 86400) / 86400.0; // Time of day
        fingerprint->vector[22] = sin(2 * M_PI * time_factor); // Cyclical time representation
        fingerprint->vector[23] = cos(2 * M_PI * time_factor);
    }
    
    // Remaining dimensions (24-31) can be used for future features
    // For now, add some controlled noise based on commit properties to make fingerprints unique
    int commit_id = get_commit_id(commit);
    srand(commit_id); // Use commit ID as seed for reproducible "randomness"
    for (int i = 24; i < FINGERPRINT_DIMENSIONS; i++) {
        fingerprint->vector[i] = ((double)rand() / RAND_MAX - 0.5) * 0.05; // Small random component
    }
    
    normalize_fingerprint_vector(fingerprint);
    
    return fingerprint;
}

void normalize_fingerprint_vector(SemanticFingerprint* fingerprint) {
    if (!fingerprint) return;
    
    // Calculate magnitude
    double magnitude = 0.0;
    for (int i = 0; i < FINGERPRINT_DIMENSIONS; i++) {
        magnitude += fingerprint->vector[i] * fingerprint->vector[i];
    }
    magnitude = sqrt(magnitude);
    
    fingerprint->magnitude = magnitude;
    
    // Normalize to unit vector
    if (magnitude > 0.0) {
        for (int i = 0; i < FINGERPRINT_DIMENSIONS; i++) {
            fingerprint->vector[i] /= magnitude;
        }
    }
}

double cosine_similarity(const double* vec1, const double* vec2, int dimensions) {
    if (!vec1 || !vec2) return 0.0;
    
    double dot_product = 0.0;
    for (int i = 0; i < dimensions; i++) {
        dot_product += vec1[i] * vec2[i];
    }
    
    // Since vectors are normalized, their magnitudes are 1
    return dot_product;
}

double calculate_fingerprint_similarity(const SemanticFingerprint* fp1, const SemanticFingerprint* fp2) {
    if (!fp1 || !fp2) return 0.0;
    
    return cosine_similarity(fp1->vector, fp2->vector, FINGERPRINT_DIMENSIONS);
}

void print_semantic_fingerprint(const SemanticFingerprint* fingerprint) {
    if (!fingerprint) return;
    
    printf("Semantic Fingerprint:\n");
    printf("  Magnitude: %.4f\n", fingerprint->magnitude);
    
    if (fingerprint->keywords) {
        printf("  Intent: %s\n", fingerprint->keywords->intent_type ? fingerprint->keywords->intent_type : "unknown");
        printf("  Sentiment: %.2f\n", fingerprint->keywords->sentiment_score);
        if (fingerprint->keywords->ticket_reference) {
            printf("  Ticket: %s\n", fingerprint->keywords->ticket_reference);
        }
    }
    
    if (fingerprint->file_analysis) {
        printf("  Primary Module: %s\n", fingerprint->file_analysis->primary_module ? fingerprint->file_analysis->primary_module : "unknown");
        printf("  File Diversity: %.2f\n", fingerprint->file_analysis->file_diversity);
        printf("  Files Changed: %d\n", fingerprint->file_analysis->total_files_changed);
    }
    
    if (fingerprint->code_delta) {
        printf("  Semantic Distance: %.2f\n", fingerprint->code_delta->semantic_distance);
        printf("  Lines Added: %d\n", fingerprint->code_delta->lines_added);
        printf("  Lines Removed: %d\n", fingerprint->code_delta->lines_removed);
        printf("  Functions Modified: %d\n", fingerprint->code_delta->functions_modified);
    }
}

void print_fingerprint_similarity(const SemanticFingerprint* fp1, const SemanticFingerprint* fp2) {
    if (!fp1 || !fp2) return;
    
    double similarity = calculate_fingerprint_similarity(fp1, fp2);
    printf("Similarity: %.4f ", similarity);
    
    if (similarity > 0.8) {
        printf("(Very Similar)");
    } else if (similarity > 0.6) {
        printf("(Similar)");
    } else if (similarity > 0.4) {
        printf("(Somewhat Similar)");
    } else if (similarity > 0.2) {
        printf("(Weakly Similar)");
    } else {
        printf("(Different)");
    }
    printf("\n");
}

// Cleanup functions
void free_commit_keywords(CommitKeywords* keywords) {
    if (!keywords) return;
    
    free(keywords->intent_type);
    free(keywords->ticket_reference);
    
    for (int i = 0; i < keywords->keyword_count; i++) {
        free(keywords->keywords[i]);
    }
    
    free(keywords);
}

void free_file_path_analysis(FilePathAnalysis* analysis) {
    if (!analysis) return;
    
    free(analysis->primary_module);
    
    for (int i = 0; i < analysis->pattern_count; i++) {
        free(analysis->patterns[i]);
    }
    
    free(analysis);
}

void free_code_delta_analysis(CodeDeltaAnalysis* analysis) {
    if (!analysis) return;
    free(analysis);
}

void free_semantic_fingerprint(SemanticFingerprint* fingerprint) {
    if (!fingerprint) return;
    
    free_commit_keywords(fingerprint->keywords);
    free_file_path_analysis(fingerprint->file_analysis);
    free_code_delta_analysis(fingerprint->code_delta);
    free(fingerprint->commit_hash);
    
    free(fingerprint);
}
