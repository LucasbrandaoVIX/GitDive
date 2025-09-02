#include <stdlib.h>
#include "list.h"

struct List{
    Node* first;
    Node* last;
    int items_num;
};

List* initialize_list() {
    List* list = malloc(sizeof(List));
    if (list == NULL) return NULL; // Check allocation
    list->first = NULL;
    list->last = NULL;
    list->items_num = 0;
    return list;
}

void insert_item(List* list, void* data){

    Node* node = malloc(sizeof(Node));
    node->data = data;

    if(list->first == NULL){

        list->first = list->last = node;
        node->previous = node->next = NULL;

        
    }else{
        node->next = NULL;
        node->previous = list->last;
        list->last->next = node; 
        list->last = node;
    }
    
    list->items_num++;
}


void print_list(List* list, void (*print_function)(void*)){
    Node* node = list->first;
    while (node != NULL){
        
        print_function(node->data);
        
        node = node->next;
    }

}


void* traverse_list(List* list, void* (*callback_function)(void*)){
    Node* node = list->first;
    while (node != NULL){
        
        void* v = callback_function(node->data);
        if(v){
            return v;
        }
        
        node = node->next;
    }
    
    return NULL;
}

int get_number_of_items(List* list){

    return list->items_num;
}

void* traverse_list_with_context(List* list, void* (*callback_function)(void*, void*), void* context){
    Node* node = list->first;
    while (node != NULL){
        void* v = callback_function(node->data, context);
        if(v){
            return v;
        }
        node = node->next;
    }
    return NULL;
}

// Alias for initialize_list
List* create_list() {
    return initialize_list();
}

// Node navigation functions
Node* get_first_node(List* list) {
    if (!list) return NULL;
    return list->first;
}

Node* get_last_node(List* list) {
    if (!list) return NULL;
    return list->last;
}

Node* get_next_node(Node* node) {
    if (!node) return NULL;
    return node->next;
}

Node* get_previous_node(Node* node) {
    if (!node) return NULL;
    return node->previous;
}

void* get_node_data(Node* node) {
    if (!node) return NULL;
    return node->data;
}

// Cleanup function
void free_list(List* list) {
    if (!list) return;
    
    Node* current = list->first;
    while (current) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    
    free(list);
}
