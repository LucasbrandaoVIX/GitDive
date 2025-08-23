#ifndef COMMIT_H
#define COMMIT_H
#include "list.h"

typedef struct Commit Commit;
typedef struct Modification Modification;
typedef struct Author Author;

Commit* initialize_commit(Author* author, List* modifications, char* message);
Modification* initialize_modification(char* filepath, char* old_code, char* new_code, int start_line);
Author* initialize_author(int author_id, char* name);


#endif