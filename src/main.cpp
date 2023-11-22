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

    tree_dif.root = Differentiator(tree.root);

    //printf("^_^ _/%lg\n", Evaluate(tree.root, 0));

    GraphicDump(&tree_dif);


    // printf("%s and %lu\n", names[0].var_name, names[0].name_size);
    // printf("%s and %lu\n", names[1].var_name, names[1].name_size);
    // printf("%s and %lu\n", names[2].var_name, names[2].name_size);
    // printf("%s and %lu\n", names[3].var_name, names[3].name_size);
    // printf("%s and %lu\n", names[4].var_name, names[4].name_size);
    // printf("%lu\n", tree.num_names);


    //printf("^_^ _/%lg\n", Evaluate(tree.root, 2000.0));

    printf("\n");
    PrintNode(tree.root, stdout, IN_ORDER);
    printf("\n");
    

    PrintNode(tree_dif.root, stdout, IN_ORDER);
    printf("\n");



    DestructorTree(&tree_dif);
    DestructorTree(&tree);

    DeleteBuffer(&buffer);


    return 0;
}