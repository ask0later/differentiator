#include "dif.h"


double Differentiator(Node* node)
{
    //if (node->type == NUM) return CreateNode(NUM, 0, NULL, NULL);

}


double Evaluate(Node* node, double x)
{
    if (!node) {return NAN;}
    if (node->type == NUM)
    {
        return node->data.value;
    }
    if (node->type == VAR)
        return x;
    
    double left = 0, right = 0;

    if (node->left)
        left  = Evaluate(node->left, x);
    if (node->right)
        right = Evaluate(node->right, x);

    switch(node->data.value_op)
    {
        case OP_ADD: return (left + right);

        case OP_SUB: return (left - right);

        case OP_MUL: return (left * right);

        case OP_DIV: return (left/right);
    }
}