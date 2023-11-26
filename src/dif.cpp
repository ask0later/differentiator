#include "dif.h"


Node* Differentiator(Node* node, Table name)
{
    if (node->type == NUM) return CreateNode(NUM, 0, NULL, NULL);

    if (node->type == VAR) 
    {
        if (strcmp(node->data.variable, name.var_name) == 0)
            return CreateNode(NUM, 1, NULL, NULL);
        else
            return CreateNode(NUM, 0, NULL, NULL);
    }

    switch(node->data.value_op)
    {
        case OP_ADD: return CreateNode(OPERATOR, OP_ADD, Differentiator(node->left, name), Differentiator(node->right, name));
        case OP_SUB: return CreateNode(OPERATOR, OP_SUB, Differentiator(node->left, name), Differentiator(node->right, name));
        case OP_MUL: return CreateNode(OPERATOR, OP_ADD, CreateNode(OPERATOR, OP_MUL, Differentiator(node->left, name), Copynator(node->right)), CreateNode(OPERATOR, OP_MUL, Copynator(node->left), Differentiator(node->right, name)));
        case OP_DIV: return CreateNode(OPERATOR, OP_DIV, CreateNode(OPERATOR, OP_SUB, CreateNode(OPERATOR, OP_MUL, Differentiator(node->left, name), Copynator(node->right)), CreateNode(OPERATOR, OP_MUL, Copynator(node->left), Differentiator(node->right, name))),
                            CreateNode(OPERATOR, OP_MUL, Copynator(node->right), Copynator(node->right)));
        default:
            printf("extra error");
            break;
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
    
    if (node->type == VAR)
        new_node->data.variable = strdup(node->data.variable);

    return new_node;
}


double Evaluate(Tree* tree, Node* node, Table* names)
{
    if (!node) {return NAN;}
    if (node->type == NUM)
    {
        return node->data.value;
    }
    if (node->type == VAR)
    {
        for (size_t i = 0; i < tree->num_names; i++)
        {
            if (strncmp(node->data.variable, names[i].var_name, names[i].name_size) == 0)
            {
                return names[i].var_value;
            }
        }
    }
    double left = 0, right = 0;

    if (node->left)
        left  = Evaluate(tree, node->left, names);
    if (node->right)
        right = Evaluate(tree, node->right, names);

    switch(node->data.value_op)
    {
        case OP_ADD: return (left + right);

        case OP_SUB: return (left - right);

        case OP_MUL: return (left * right);

        case OP_DIV: return (left / right);
    }
}



void PrintNameTable(Tree* tree, Table* names)
{
    for (size_t i = 0; i < tree->num_names; i++)
    {
        printf("%15s = %3lg (длина имени переменной = %2lu)\n", names[i].var_name, names[i].var_value, names[i].name_size);
    }
}


TreeError RemoveDummyElements(Tree* tree, Node** node)
{
    if (!(*node)) {return NO_ERROR;}

    Node** left  = &(*node)->left;
    Node** right = &(*node)->right;
    double value = 0;

    Node* copy_node = 0;

    if (((*node)->type == OPERATOR) && ((*right)->type == NUM))
    {
        tree->changes_num++;
        copy_node = Copynator((*node)->left);
        if (((*right)->data.value == 1) && ((*node)->data.value_op == OP_MUL)) 
        {
            DeleteNode(*node);
            *node = copy_node;
        }
        else if (((*right)->data.value == 1) && ((*node)->data.value_op == OP_DIV)) 
        {
            DeleteNode(*node);
            *node = copy_node;
        }
        else if (((*right)->data.value == 0) && ((*node)->data.value_op == OP_ADD))
        {
            DeleteNode(*node);
            *node = copy_node;
        }
        else if (((*right)->data.value == 0) && ((*node)->data.value_op == OP_SUB))
        {
            DeleteNode(*node);
            *node = copy_node;
        }
        else 
        {
            DeleteNode(copy_node);
            tree->changes_num--;
            RemoveDummyElements(tree, left);
            RemoveDummyElements(tree, right);
        }
    }
    else if (((*node)->type == OPERATOR) && ((*left)->type == NUM))
    {
        tree->changes_num++;
        copy_node = Copynator((*node)->right);
        if (((*left)->data.value == 1) && ((*node)->data.value_op == OP_MUL))
        {
            DeleteNode(*node);
            *node = copy_node;
        }
        else if (((*left)->data.value == 1) && ((*node)->data.value_op == OP_DIV)) 
        {
            DeleteNode(*node);
            *node = copy_node;
        }
        else if (((*left)->data.value == 0) && ((*node)->data.value_op == OP_ADD))
        {
            DeleteNode(*node);
            *node = copy_node;
        }
        else if (((*left)->data.value == 0) && ((*node)->data.value_op == OP_SUB))
        {
            DeleteNode(*node);
            *node = copy_node;
        }
        else
        {
            DeleteNode(copy_node);
            tree->changes_num--;
            RemoveDummyElements(tree, left);
            RemoveDummyElements(tree, right);
        }
    }
    else
    {
        RemoveDummyElements(tree, left);
        RemoveDummyElements(tree, right);
    }

    return NO_ERROR;
}

TreeError CollapsingConstants(Tree* tree, Node** node)
{
    if (!(*node)) {return NO_ERROR;}

    Node** left  = &(*node)->left;
    Node** right = &(*node)->right;

    if ((*node)->type == OPERATOR)
    {
        if (((*left)->type == NUM) && ((*right)->type == NUM))
        {
            tree->changes_num++;
            double value_left = (*left)->data.value, value_right = (*right)->data.value;
            double value = 0;
            switch ((*node)->data.value_op)
            {
                case OP_ADD: value = value_left + value_right;
                    break;
                case OP_SUB: value = value_left - value_right;
                    break;
                case OP_MUL: value = value_left * value_right;
                    break;
                case OP_DIV: value = value_left / value_right;
                    break;
            }
            DeleteNode(*node);
            *node = CreateNode(NUM, value, NULL, NULL);
            return NO_ERROR;
        }
        else
        {
            CollapsingConstants(tree, left);
            CollapsingConstants(tree, right);
        }
    }
    else
    {
        CollapsingConstants(tree, left);
        CollapsingConstants(tree, right);
    }

    return NO_ERROR;
}


TreeError Simplification(Tree* tree)
{
    size_t start_changes_num = 0;
    do
    {
        start_changes_num = tree->changes_num;
        CollapsingConstants(tree, &(tree->root));
        RemoveDummyElements(tree, &(tree->root));
        printf("<%lu>\n", tree->changes_num);
    } while (start_changes_num != tree->changes_num);

    return NO_ERROR;
}