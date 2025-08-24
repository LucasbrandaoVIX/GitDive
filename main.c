#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "commit.h"

int main(){

    // Create a list to hold commits
    List* commit_list = initialize_list();
    
    // Create first author
    Author* author1 = initialize_author(1, "lucas");
    
    // First commit with modifications
    List* mod_list1 = initialize_list();
    
    char* filepath1 = "this/is/the/path";
    char* old_code1 = "print(hello world)";
    char* new_code1 = "print('hello world!!')// fixed message";
    int start_line1 = 5;
    
    Modification* mod1 = initialize_modification(filepath1, old_code1, new_code1, start_line1);
    insert_item(mod_list1, mod1);

    char* filepath2 = "this/is/the/path2";
    char* old_code2 = "int x = 8.";
    char* new_code2 = "int x = 8; // fixed ;";
    int start_line2 = 3;
    
    Modification* mod2 = initialize_modification(filepath2, old_code2, new_code2, start_line2);
    insert_item(mod_list1, mod2);

    Commit* commit1 = initialize_commit(1, author1, mod_list1, "First commit: Fixed print statement and semicolon");
    insert_item(commit_list, commit1);

    // Create second author
    Author* author2 = initialize_author(2, "maria");
    
    // Second commit with different modifications
    List* mod_list2 = initialize_list();
    
    char* filepath3 = "src/utils.c";
    char* old_code3 = "return null;";
    char* new_code3 = "return NULL; // fixed case";
    int start_line3 = 15;
    
    Modification* mod3 = initialize_modification(filepath3, old_code3, new_code3, start_line3);
    insert_item(mod_list2, mod3);

    Commit* commit2 = initialize_commit(2, author2, mod_list2, "Second commit: Fixed NULL case sensitivity");
    insert_item(commit_list, commit2);

    // Third commit by first author
    List* mod_list3 = initialize_list();
    
    char* filepath4 = "main.c";
    char* old_code4 = "#include <stdio.h>";
    char* new_code4 = "#include <stdio.h>\n#include <stdlib.h> // added stdlib";
    int start_line4 = 1;
    
    Modification* mod4 = initialize_modification(filepath4, old_code4, new_code4, start_line4);
    insert_item(mod_list3, mod4);

    Commit* commit3 = initialize_commit(3, author1, mod_list3, "Third commit: Added stdlib include");
    insert_item(commit_list, commit3);

    // Print the entire commit list
    printf("========== COMMIT HISTORY ==========\n");
    print_list(commit_list, (void (*)(void*))print_commit);
    printf("====================================\n");

    return 0;
}