#include "list.h"

struct Node{
    Node* previous;
    Node* next;
    void* data;
};

struct List{

    Node* first;
    Node* last;
    int items_num;
};

