#include <stdlib.h>
#include "list.h"

typedef struct Node{
    struct Node* previous;
    struct Node* next;
    void* data;
}Node;

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
    
}
