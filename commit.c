#include "commit.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
struct Commit{

    int id;
    Author* author;
    char message[256];
    List* modifications;
};

struct Author{

    int author_id;
    char name[32];
    
};

struct Modification {

    char filepath[256];           
    char old_code[1024];      
    char new_code[1024];    
    int start_line;   
};

Commit* initialize_commit(int commit_id, Author* author, List* modifications, char* message){
    Commit* commit = malloc(sizeof(Commit));
    if (!commit) return NULL;
    commit->id = commit_id;
    commit->author = author;
    strcpy(commit->message, message);
    commit->modifications = modifications;
    return commit;
}

Modification* initialize_modification(char* filepath, char* old_code, char* new_code, int start_line){
    Modification* mod = malloc(sizeof(Modification));
    if (!mod) return NULL;

    strcpy(mod->filepath, filepath);
    strcpy(mod->old_code, old_code);
    strcpy(mod->new_code, new_code);
    mod->start_line = start_line;

    return mod;
}

Author* initialize_author(int author_id, char* name){
    Author* author = malloc(sizeof(Author));
    if (!author) return NULL;
    author->author_id = author_id;
    strcpy(author->name, name);
    return author;
}

void print_mod(Modification* mod){
    if (mod == NULL) {
        printf("Modification is NULL\n");
        return;
    }
    
    printf("=== Modification Details ===\n");
    printf("File Path: %s\n", mod->filepath);
    printf("Start Line: %d\n", mod->start_line);
    printf("Old Code: %s\n", mod->old_code);
    printf("New Code: %s\n", mod->new_code);
    printf("============================\n");
}

void print_commit(Commit* commit){
    if (commit == NULL) {
        printf("Commit is NULL\n");
        return;
    }
    
    printf("\n======= COMMIT =======\n");
    printf("Commit ID: %d\n", commit->id);
    printf("Author: %s (ID: %d)\n", commit->author->name, commit->author->author_id);
    printf("Message: %s\n", commit->message);
    printf("Modifications:\n");
    
    if (commit->modifications != NULL) {
        print_list(commit->modifications, (void (*)(void*))print_mod);
    } else {
        printf("No modifications\n");
    }
    
    printf("=====================\n\n");
}