#include "tree.h"
#include "readFile.h"
#include "dif.h"
#include "graphic.h"


int main()
{
    Text buffer = {};
    Tree tree   = {};
    Tree tree_dif = {};
    ConstructorTree(&tree);
    ConstructorTree(&tree_dif);

    Table names[MAX_NUM_VARS] = {};

    CreateBuffer(&buffer, "file.txt");

    char* position = buffer.position;

    TreeError error = ReadTree(&tree, &tree.root, &position, IN_ORDER, names, buffer);
    if (error != NO_ERROR)
    {
        DumpErrors(error);
        DestructorTree(&tree);
        return 1;
    }
    Simplification(&tree);
    printf("\\");
    PrintNode(tree.root, stdout, IN_ORDER);
    printf("\n");

    
    printf("<%s>\n", names[0].var_name);
    tree_dif.root = Differentiator(tree.root, names[0]);

    PrintNode(tree_dif.root, stdout, IN_ORDER);
    printf("\n");

    Simplification(&tree_dif);

    PrintNode(tree_dif.root, stdout, IN_ORDER);
    printf("\n");

    GraphicDump(&tree, &tree_dif);

    // PrintNode(tree_dif.root, stdout, IN_ORDER);
    // RemoveDummyElements(&(tree_dif.root));
    // PrintNode(tree_dif.root, stdout, IN_ORDER);

    // CollapsingConstants(&(tree_dif.root));
    // RemoveDummyElements(&(tree_dif.root));

    
    // names[0].var_value = 1;
    // printf("f(%lg) = %lg\n", names[0].var_value, Evaluate(&tree, tree.root, names));

    
    

    // printf("f (%s, %s, %s) = ", names[0].var_name, names[1].var_name, names[2].var_name);
    // PrintNode(tree.root, stdout, IN_ORDER);
    // printf("\n");
    // printf("f'(%s, %s, %s) = ", names[0].var_name, names[1].var_name, names[2].var_name);
    // PrintNode(tree_dif.root, stdout, IN_ORDER);
    // printf("\n");



    DestructorTree(&tree_dif);
    DestructorTree(&tree);

    DeleteBuffer(&buffer);


    return 0;
}