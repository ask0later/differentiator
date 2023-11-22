#include "graphic.h"
#include "Dotter.h"

TreeError GraphicDump(Tree* tree)
{
    dtBegin("Tree.dot");                        // Начало dot-описания графа

    dtNodeStyle ().shape        ("box");
    dtNodeStyle ().style         ("filled");
    //dtNodeStyle ().fontcolor      ("black");

    GraphicDumpNode(tree->root, 1);

    dtEnd();                                   // Конец dot-описания графа

    dtRender("Tree.dot", "Tree.jpg", "jpg", false);

    return NO_ERROR;
}


TreeError GraphicDumpNode(Node* node, size_t counter)
{
    if (!node) {return NO_ERROR;}

    char str[MAX_SIZE_ARG] = {};
    if (node->type == NUM)
    {
        dtNodeStyle().fillcolor("#7BF2DA");//HEX_TIFFANY
        sprintf(str, "%lg", node->value);
    }
    else if (node->type == OPERATOR)
    {
        dtNodeStyle().fillcolor("#EE204D");//HEX_RED
        switch((int) node->value)
        {
            case OP_ADD:
                sprintf(str, " + ");
                break;
            case OP_SUB:
                sprintf(str, " + ");
                break;
            case OP_MUL:
                sprintf(str, " * ");
                break;
            case OP_DIV:
                sprintf(str, " / ");
                break;
        }
    }
    
    
    dtNode((int) counter, str);

    if (node->left  != 0)
    {
        GraphicDumpNode(node->left, counter * 2 + 1);
        dtLink((int) counter, (int)(counter * 2 + 1), "");
    }

    if (node->right != 0)
    {
        GraphicDumpNode(node->right, counter * 2 + 2);
        dtLink((int) counter, (int)(counter * 2 + 2), "");
    }

    return NO_ERROR;
}