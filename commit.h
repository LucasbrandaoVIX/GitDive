#ifndef COMMIT_H
#define COMMIT_H
#include "list.h"

typedef struct Commit Commit;
typedef struct Modification Modification;
typedef struct Author Author;

// Initialize a commit with a numeric id, author, modifications, message, and unix timestamp (seconds since epoch)
Commit* initialize_commit(int commit_id, Author* author, List* modifications, char* message, long long timestamp);
Modification* initialize_modification(char* filepath, char* old_code, char* new_code, int start_line);
Author* initialize_author(int author_id, char* name);
void print_mod(Modification* mod);
void print_commit(Commit* commit);
// Single-line summary printer suitable for tree printing (no trailing newline)
void print_commit_for_tree(void* data);

// Comparator for commits (returns <0 if a<b, 0 if equal, >0 if a>b)
int compare_commits(void* a, void* b);

// Comparator for commits by timestamp (older first)
int compare_commits_by_timestamp(void* a, void* b);

// Optional: accessor for commit timestamp (useful for tests or other modules)
long long get_commit_timestamp(Commit* commit);

#endif