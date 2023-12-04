#include "graphic.h"
#include "Dotter.h"

TreeError GraphicDump(Tree* tree1, Tree* tree2)
{
    dtBegin("Tree.dot");                        // Начало dot-описания графа

    dtNodeStyle ().shape        ("box");
    dtNodeStyle ().style         ("filled");
    dtNodeStyle ().fontcolor      ("black");

    if (tree1)
        GraphicDumpNode(tree1->root);
    if (tree2)
        GraphicDumpNode(tree2->root);

    dtEnd();                                   // Конец dot-описания графа

    dtRender("Tree.dot", "Tree.jpg", "jpg", false);

    return NO_ERROR;
}


TreeError GraphicDumpNode(Node* node)
{
    if (!node) return NO_ERROR;

    if (!node) {return NO_ERROR;}

    char str[MAX_SIZE_ARG] = {};
    if (node->type == NUM)
    {
        dtNodeStyle().fillcolor("#7BF2DA");//HEX_TIFFANY
        sprintf(str, "%lg", node->data.value);
    }
    else if (node->type == OPERATOR)
    {
        dtNodeStyle().fillcolor("#EE204D");//HEX_RED
        switch(node->data.value_op)
        {
            case OP_ADD:
                sprintf(str, " + ");
                break;
            case OP_SUB:
                sprintf(str, " - ");
                break;
            case OP_MUL:
                sprintf(str, " * ");
                break;
            case OP_DIV:
                sprintf(str, " / ");
                break;
            case FUN_SIN:
                sprintf(str, " sin ");
                break;
            case FUN_COS:
                sprintf(str, " cos ");
                break;
            case FUN_POW:
                sprintf(str, " ^ ");
                break;
            case FUN_SQRT:
                sprintf(str, " sqrt ");
                break;
            case FUN_LN:
                sprintf(str, " ln ");
                break;
            default:
                printf("extra");
                break;
        }
    }
    else if (node->type == VAR)
    {
        dtNodeStyle().fillcolor("#21C912");//HEX_GREEN
        sprintf(str, "%s", node->data.variable);
    }
    
    
    dtNode((int)(size_t) node, str);

    if (node->left  != 0)
    {
        GraphicDumpNode(node->left);
        dtLink((int)(size_t) node, (int)(size_t)(node->left), "");
    }

    if (node->right != 0)
    {
        GraphicDumpNode(node->right);
        dtLink((int)(size_t) node, (int)(size_t)(node->right), "");
    }

    return NO_ERROR;
}