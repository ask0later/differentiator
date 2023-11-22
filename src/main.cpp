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

    printf("%lg\n", Evaluate(tree.root, (double) 1));
    
    tree_dif.root = Differentiator(tree.root);

    printf("%lg\n", Evaluate(tree_dif.root, (double) 1));

    
    GraphicDump(&tree, &tree_dif);

    PrintNode(tree.root, stdout, IN_ORDER);
    printf("это было дерево считанное\n");
    

    PrintNode(tree_dif.root, stdout, IN_ORDER);
    printf("это было дерево продиф\n");



    DestructorTree(&tree_dif);
    DestructorTree(&tree);

    DeleteBuffer(&buffer);


    return 0;
}