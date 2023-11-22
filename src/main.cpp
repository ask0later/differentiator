#include "tree.h"
#include "readFile.h"
#include "dif.h"
#include "graphic.h"


int main()
{
    Text buffer = {};
    Tree tree   = {};

    CreateBuffer(&buffer, "file.txt");

    char* position = buffer.position;

    ReadTree(&tree, &tree.root, &position, PRE_ORDER);
    
    GraphicDump(&tree);
    printf("^_^ _/%lg\n", Evaluate(tree.root));

    printf("\n");
    PrintNode(tree.root, stdout, IN_ORDER);
    printf("\n");

    DestructorTree(&tree);
    DeleteBuffer(&buffer);

    return 0;
}