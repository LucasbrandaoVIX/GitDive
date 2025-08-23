#include "commit.h"
#include <stdlib.h>
#include <string.h>
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

    char filepath[256];           // Arquivo onde ocorreu a modificação
    char old_code[1024];         // Código original (antes da modificação)
    char new_code[1024];         // Código novo (após a modificação)
    int start_line;              // Linha onde o bloco começa
};


Commit* initialize_commit(Author* author, List* modifications, char* message){
    Commit* commit = malloc(sizeof(Commit));
    if (!commit) return NULL;
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


