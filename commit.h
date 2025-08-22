#ifndef COMMIT_H
#define COMMIT_H
#include "list.h"

typedef struct Commit Commit;
typedef struct Modification Modification;
typedef struct Author Author;

Commit* initialize_commit(Author* author, List* modifications);
Modification* initialize_modification();
Author* initialize_author(const char* name);


#endif