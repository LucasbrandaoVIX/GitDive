#ifndef LIST_H
#define LIST_H

typedef struct List List;

List* initialize_list();
void insert_item(List* list, void* data);

#endif