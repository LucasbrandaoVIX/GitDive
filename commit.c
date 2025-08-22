#include "commit.h"

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
    int old_line_count;          // Quantidade de linhas do código antigo
    int new_line_count;          // Quantidade de linhas do código novo

};

