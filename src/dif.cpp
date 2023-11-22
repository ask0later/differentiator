#include "dif.h"


Node* Differentiator(Node* node)
{
    if (node->type == NUM) return CreateNode(NUM, 0, NULL, NULL);

    if (node->type == VAR) return CreateNode(NUM, 1, NULL, NULL);

    switch(node->data.value_op)
    {
        case OP_ADD: return CreateNode(OPERATOR, OP_ADD, Differentiator(node->left), Differentiator(node->right));
        case OP_SUB: return CreateNode(OPERATOR, OP_SUB, Differentiator(node->left), Differentiator(node->right));
        case OP_MUL: return CreateNode(OPERATOR, OP_ADD, CreateNode(OPERATOR, OP_MUL, Differentiator(node->left), Copynator(node->right)), CreateNode(OPERATOR, OP_MUL, Copynator(node->left), Differentiator(node->right)));
        case OP_DIV: return CreateNode(OPERATOR, OP_DIV, CreateNode(OPERATOR, OP_SUB, CreateNode(OPERATOR, OP_MUL, Differentiator(node->left), Copynator(node->right)), CreateNode(OPERATOR, OP_MUL, Copynator(node->left), Differentiator(node->right))),
                            CreateNode(OPERATOR, OP_MUL, Copynator(node->right), Copynator(node->right)));
    }
}

Node* Copynator(Node* node)
{
    if (!node) return NULL;

    double value = 0;
    if (node->type == NUM)
        value = node->data.value;
    else if (node->type == OPERATOR)
        value = (double) node->data.value_op;

    Node* new_node = CreateNode(node->type, value, Copynator(node->left), Copynator(node->right));
    //Node* new_node = CreateNode(node->type, value, node->left, node->right);
    if (node->type == VAR)
        new_node->data.variable = strdup(node->data.variable);

    return new_node;
}


double Evaluate(Node* node, double x)
{
    //if (!node) {return NAN;}
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

    //printf("%lg and %lg operator = %d\n", left, right, node->data.value_op);

    switch(node->data.value_op)
    {
        case OP_ADD: return (left + right);

        case OP_SUB: return (left - right);

        case OP_MUL: return (left * right);

        case OP_DIV: return (left / right);
    }
}