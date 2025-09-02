#ifndef LIST_H
#define LIST_H

typedef struct Node{
    struct Node* previous;
    struct Node* next;
    void* data;
} Node;

typedef struct List List;

List* initialize_list();
List* create_list(); // Alias for initialize_list
void insert_item(List* list, void* data);
void print_list(List* list, void (*print_function)(void*));
void* traverse_list(List* list, void* (*callback_function)(void*));
// Traverse with additional user context passed to callback: callback(data, ctx)
void* traverse_list_with_context(List* list, void* (*callback_function)(void*, void*), void* context);
int get_number_of_items(List* list);

// Node navigation functions
Node* get_first_node(List* list);
Node* get_last_node(List* list);
Node* get_next_node(Node* node);
Node* get_previous_node(Node* node);
void* get_node_data(Node* node);

// Cleanup function
void free_list(List* list);

#endif