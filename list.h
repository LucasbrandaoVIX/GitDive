#ifndef LIST_H
#define LIST_H

typedef struct List List;

List* initialize_list();
void insert_item(List* list, void* data);
void print_list(List* list, void (*print_function)(void*));
void* traverse_list(List* list, void* (*callback_function)(void*));
int get_number_of_items(List* list);

#endif