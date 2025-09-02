#include "commit.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
struct Commit{

    int id;
    Author* author;
    char* message;
    List* modifications;
    long long timestamp; // unix epoch seconds
};

struct Author{

    int author_id;
    char* name;
    
};

struct Modification {

    char* filepath;           
    char* old_code;      
    char* new_code;    
    int start_line;   
};

Commit* initialize_commit(int commit_id, Author* author, List* modifications, char* message, long long timestamp){
    Commit* commit = malloc(sizeof(Commit));
    if (!commit) return NULL;
    commit->id = commit_id;
    commit->author = author;
    
    // Allocate memory for message string
    if (message) {
        commit->message = malloc(strlen(message) + 1);
        if (!commit->message) {
            free(commit);
            return NULL;
        }
        strcpy(commit->message, message);
    } else {
        commit->message = NULL;
    }
    
    commit->modifications = modifications;
    commit->timestamp = timestamp;
    return commit;
}

Modification* initialize_modification(char* filepath, char* old_code, char* new_code, int start_line){
    Modification* mod = malloc(sizeof(Modification));
    if (!mod) return NULL;

    // Allocate memory for filepath string
    if (filepath) {
        mod->filepath = malloc(strlen(filepath) + 1);
        if (!mod->filepath) {
            free(mod);
            return NULL;
        }
        strcpy(mod->filepath, filepath);
    } else {
        mod->filepath = NULL;
    }
    
    // Allocate memory for old_code string
    if (old_code) {
        mod->old_code = malloc(strlen(old_code) + 1);
        if (!mod->old_code) {
            free(mod->filepath);
            free(mod);
            return NULL;
        }
        strcpy(mod->old_code, old_code);
    } else {
        mod->old_code = NULL;
    }
    
    // Allocate memory for new_code string
    if (new_code) {
        mod->new_code = malloc(strlen(new_code) + 1);
        if (!mod->new_code) {
            free(mod->filepath);
            free(mod->old_code);
            free(mod);
            return NULL;
        }
        strcpy(mod->new_code, new_code);
    } else {
        mod->new_code = NULL;
    }
    
    mod->start_line = start_line;

    return mod;
}

Author* initialize_author(int author_id, char* name){
    Author* author = malloc(sizeof(Author));
    if (!author) return NULL;
    author->author_id = author_id;
    
    // Allocate memory for name string
    if (name) {
        author->name = malloc(strlen(name) + 1);
        if (!author->name) {
            free(author);
            return NULL;
        }
        strcpy(author->name, name);
    } else {
        author->name = NULL;
    }
    
    return author;
}

void print_mod(Modification* mod){
    if (mod == NULL) {
        printf("Modification is NULL\n");
        return;
    }
    
    // Count lines changed without printing code content
    int removed_lines = 0;
    for (const char* p = mod->old_code; *p; ++p) if (*p == '\n') removed_lines++;
    int added_lines = 0;
    for (const char* p = mod->new_code; *p; ++p) if (*p == '\n') added_lines++;

    printf("- %s (start: %d, -%d +%d)\n", mod->filepath, mod->start_line, removed_lines, added_lines);
}

void print_commit(Commit* commit){
    if (commit == NULL) {
        printf("Commit is NULL\n");
        return;
    }
    
    printf("\n======= COMMIT =======\n");
    printf("Commit ID: %d\n", commit->id);
    printf("Timestamp: %lld\n", commit->timestamp);
    printf("Author: %s (ID: %d)\n", commit->author->name, commit->author->author_id);
    printf("Message: %s\n", commit->message);
    int mod_count = commit->modifications ? get_number_of_items(commit->modifications) : 0;
    printf("Files changed: %d\n", mod_count);
    
    printf("=====================\n\n");
}

// Simple comparator exposed via commit.h
int compare_commits(void* a, void* b){
    if (a == NULL || b == NULL) return 0;
    Commit* A = (Commit*)a;
    Commit* B = (Commit*)b;
    return (A->id - B->id);
}

int compare_commits_by_timestamp(void* a, void* b){
    if (a == NULL || b == NULL) return 0;
    Commit* A = (Commit*)a;
    Commit* B = (Commit*)b;
    if (A->timestamp < B->timestamp) return -1;
    if (A->timestamp > B->timestamp) return 1;
    return 0;
}

long long get_commit_timestamp(Commit* commit){
    if (!commit) return 0;
    return commit->timestamp;
}

// Compact single-line printer for tree output (matches print_btree's inline style)
void print_commit_for_tree(void* data){
    Commit* c = (Commit*)data;
    if (!c) {
        printf("(null)");
        return;
    }
    // No trailing newline; print_btree handles layout/newlines
    printf("id=%d ts=%lld | %s | %s", c->id, c->timestamp, c->author->name, c->message);
}

// Cleanup functions to free dynamically allocated memory
void free_author(Author* author) {
    if (author) {
        free(author->name);
        free(author);
    }
}

void free_modification(Modification* mod) {
    if (mod) {
        free(mod->filepath);
        free(mod->old_code);
        free(mod->new_code);
        free(mod);
    }
}

void free_commit(Commit* commit) {
    if (commit) {
        free(commit->message);
        free_author(commit->author);
        // Note: modifications list should be freed separately using the list's cleanup function
        free(commit);
    }
}