#include <stdlib.h>
#include "list.h"
#include "commit.h"

int main(){

    int num_of_authors = 0;
    
    Author* author = initialize_author(1, "lucas");
    List* mod_list = initialize_list();
    

    //---------
    char* filepath = "this/is/the/path";
    char* old_code = "print(hello world)";
    char* new_code = "print('hello world!!')// fixed message";
    int start_line = 5;
    
    Modification* mod = initialize_modification(filepath, old_code, new_code, start_line);
    insert_item(mod_list, mod);
    //---------


    //---------
    char* filepath2 = "this/is/the/path2";
    char* old_code2 = "int x = 8.";
    char* new_code2 = "int x = 8; // fixed ;";
    int start_line2 = 3;
    
    Modification* mod2 = initialize_modification(filepath2, old_code2, new_code2, start_line2);
    insert_item(mod_list, mod2);
    //---------





    return 0;
}