#include "tree.h"
#include "readFile.h"
#include "dif.h"
#include "graphic.h"


int main()
{
    Text buffer = {};
    Tree tree   = {};
    Tree tree_dif = {};

    Table names[MAX_NUM_VARS] = {};

    CreateBuffer(&buffer, "file.txt");

    char* position = buffer.position;

    TreeError error = ReadTree(&tree, &tree.root, &position, IN_ORDER, names);
    if (error != NO_ERROR)
    {
        DumpErrors(error);
        DestructorTree(&tree);
        return 1;
    }

    PrintNameTable(&tree, names);
    names[0].var_value = 1;
    names[1].var_value = 2;
    names[2].var_value = 3;
    PrintNameTable(&tree, names); 

    printf("\n");
    printf("%lg\n", Evaluate(&tree, tree.root, names));
    
    tree_dif.root = Differentiator(tree.root, names[2]);

    printf("%lg\n", Evaluate(&tree, tree_dif.root, names));

    
    
    GraphicDump(&tree, &tree_dif);

    PrintNode(tree.root, stdout, IN_ORDER);
    //printf("это было дерево считанное\n");
    

    //PrintNode(tree_dif.root, stdout, IN_ORDER);
    //printf("это было дерево продиф\n");



    DestructorTree(&tree_dif);
    DestructorTree(&tree);

    DeleteBuffer(&buffer);


    return 0;
}