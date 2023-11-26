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
    if (node->type == OPERATOR)
    {
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

    if (node->type == FUNCTION)
    {
        switch(node->data.value_fun)
        {
            case FUN_SIN:  return CreateNode(OPERATOR, OP_MUL, CreateNode(FUNCTION, FUN_COS, Copynator(node->left), Copynator(node->right)), Differentiator(node->right, name));

            case FUN_COS:  return CreateNode(OPERATOR, OP_SUB, CreateNode(NUM, 0, NULL, NULL), CreateNode(OPERATOR, OP_MUL, CreateNode(FUNCTION, FUN_SIN, Copynator(node->left), Copynator(node->right)), Differentiator(node->right, name)));

            case FUN_POW:  
                if (node->right->type == NUM)
                {
                    return CreateNode(OPERATOR, OP_MUL, CreateNode(NUM, node->right->data.value, NULL, NULL), CreateNode(FUNCTION, FUN_POW, Copynator(node->left), CreateNode(NUM, node->right->data.value - 1, NULL, NULL)));
                }
                else if (node->left->type == NUM)
                {
                    return CreateNode(OPERATOR, OP_MUL,
                                                       Differentiator(node->left, name), 
                                                       CreateNode(OPERATOR, OP_MUL,
                                                                                   CreateNode(FUNCTION, FUN_LN, NULL, Copynator(node->left)),
                                                                                   Copynator(node->right)));
                }
                else
                {
                    return CreateNode(OPERATOR, OP_MUL,
                                                        CreateNode(OPERATOR, OP_ADD, 
                                                                                    CreateNode(OPERATOR, OP_DIV, 
                                                                                                                CreateNode(OPERATOR, OP_MUL, Differentiator(node->left, name), Copynator(node->right)),
                                                                                                                Copynator(node->left)),
                                                                                    CreateNode(OPERATOR, OP_MUL,
                                                                                                                CreateNode(FUNCTION, FUN_LN, NULL, Copynator(node->left)),
                                                                                                                Differentiator(node->right, name))),
                    
                                                        Copynator(node));
                }

            
            case FUN_SQRT: return CreateNode(OPERATOR, OP_DIV, CreateNode(OPERATOR, OP_SUB, CreateNode(OPERATOR, OP_MUL, Differentiator(node->left, name), Copynator(node->right)), CreateNode(OPERATOR, OP_MUL, Copynator(node->left), Differentiator(node->right, name))),
                                CreateNode(OPERATOR, OP_MUL, Copynator(node->right), Copynator(node->right)));

            case FUN_LN: return CreateNode(OPERATOR, OP_MUL, Differentiator(node->right, name), CreateNode( OPERATOR, OP_DIV, CreateNode(NUM, 1, NULL, NULL), Copynator(node->right) ) );
            
            default:
                printf("extra error");
                break;
        }
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
    else if (node->type == FUNCTION)
        value = (double) node->data.value_fun;


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

    if (node->type == OPERATOR)
    {
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
    if (node->type == FUNCTION)
    {
        if (node->left)
            left  = Evaluate(tree, node->left, names);
        if (node->right)
            right = Evaluate(tree, node->right, names);
        
        switch(node->data.value_op)
        {
            case FUN_SIN:  return (sin(right));

            case FUN_COS:  return (cos(right));

            case FUN_POW:  return (pow(left, right));

            case FUN_SQRT: return (sqrt(right));

            case FUN_LN:   return (log(right));
        }
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

    Node* copy_node = NULL;

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
        else if (((*right)->data.value == 0) && ((*node)->data.value_op == OP_MUL))
        {
            DeleteNode(*node);
            DeleteNode(copy_node);
            *node = CreateNode(NUM, 0 , NULL, NULL);
        }
        else if (((*right)->data.value == 0) && ((*node)->data.value_op == OP_DIV))
        {
            // delete 0
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
        else if (((*left)->data.value == 0) && ((*node)->data.value_op == OP_ADD))
        {
            DeleteNode(*node);
            *node = copy_node;
        }
        else if (((*left)->data.value == 0) && ((*node)->data.value_op == OP_MUL))
        {
            DeleteNode(*node);
            DeleteNode(copy_node);
            *node = CreateNode(NUM, 0 , NULL, NULL);
        }
        else if (((*left)->data.value == 0) && ((*node)->data.value_op == OP_DIV))
        {
            DeleteNode(*node);
            DeleteNode(copy_node);
            *node = CreateNode(NUM, 0 , NULL, NULL);
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

    } while (start_changes_num != tree->changes_num);

    return NO_ERROR;
}

TreeError PrintfLatex(Tree* tree, Tree* tree_dif)
{
    FILE* To = fopen("latex.txt", "w");

    fprintf(To, "\\documentclass[a4paper,12pt]{article}\n"
                "\\usepackage[T1]{fontenc}\n"
                "\\usepackage[utf8]{inputenc}\n"
                "\\usepackage[english,russian]{babel}\n"
                "\\usepackage{amsmath,amsfonts,amssymb,amsthm,mathtools}\n"
                "\\usepackage[left=10mm, top=10mm, right=10mm, bottom=20mm, nohead, nofoot]{geometry}\n"
                "\\usepackage{wasysym}\n"
                "\\author{\\LARGEМерзляков Арсений Б01-304}\n"
                "\\title{Производная}\n"
                "\\pagestyle {empty}\n"
                "\\begin{document}\n"
                "\\maketitle\n"
                "\\begin{flushleft}\n"
                "\\Large\n"
                "$(cosx)^{sinx}\\cdot (cosx\\cdot \\ln{(cosx)}+sinx\\cdot \\dfrac{1.000}{cosx}\\cdot sinx\\cdot (-1.000))$\n"
                "\\end{flushleft}\n"
                "\\end{document}\n");
    printf("Вашему вниманию представляется контрольная работа советского второклассника\n"
           "Это задание, было дано ученику первым номером, как самое простое\n"
           "1. Найдите производную f(x);\n"
           "2. Найдите производную в точке x0 f(x0);\n"
           "3. Разложите f(x) по формуле Тейлора при X --> 0;\n");

    




}