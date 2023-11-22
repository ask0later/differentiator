#include "dif.h"


double Differentiator(Node* node)
{
    
}


double Evaluate(Node* node)
{
    if (!node) {return NAN;}
    if (node->type == NUM)
        return node->value;
    
    double left = 0, right = 0;

    if (node->left)
        left  = Evaluate(node->left);
    if (node->right)
        right = Evaluate(node->right);

    switch((int) node->value)
    {
        case OP_ADD: return (left + right);

        case OP_SUB: return (left - right);

        case OP_MUL: return (left * right);

        case OP_DIV: return (left/right);
    }
}