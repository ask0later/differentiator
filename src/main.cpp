#include "tree.h"
#include "readFile.h"


int main()
{
    Text buffer = {};
    Tree tree   = {};

    CreateBuffer(&buffer, "file.txt");
    // FILE* From = fopen("file.txt", "r");
    // fclose(From);
    char* position = buffer.position;

    fprintf(stderr, "С%sЭ\n", position);
    //ReadTree(&tree, &(&tree)->root, From, PRE_ORDER);

    NewReadTree(&tree, &tree.root, &position, PRE_ORDER);

    
    printf("\n");
    printf("печатаю <");
    

    PrintNode(tree.root, stdout, IN_ORDER);
    printf(">напечатал \n");

    DestructorTree(&tree);
    DeleteBuffer(&buffer);

    return 0;
}