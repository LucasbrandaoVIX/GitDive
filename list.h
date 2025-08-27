#ifndef LIST_H
#define LIST_H

typedef struct List List;

List* initialize_list();
void insert_item(List* list, void* data);
void print_list(List* list, void (*print_function)(void*));
void* traverse_list(List* list, void* (*callback_function)(void*));
// Traverse with additional user context passed to callback: callback(data, ctx)
void* traverse_list_with_context(List* list, void* (*callback_function)(void*, void*), void* context);
int get_number_of_items(List* list);

#endif