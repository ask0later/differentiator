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
    {
        free(new_node->data.variable);
        new_node->data.variable = strdup(node->data.variable);
    }
        

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
        //fprintf(stderr, "%lu\n", tree->num_names);
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
        {
            left  = Evaluate(tree, node->left, names);
        }
        if (node->right)
        {
            right = Evaluate(tree, node->right, names);
        }
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

    if ((*right))
    {
        printf("AAA?");
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
                printf("AAA???");
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
            printf("AAA???");
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

TreeError PrintfLatex(Tree* tree, Table* names)
{
    FILE* To = fopen("latex.txt", "w");

    Tree tree_dif = {};
    Tree tree_tay = {};
    tree_dif.num_names = 1;
    tree_tay.num_names = 1;
    tree_dif.changes_num = 0;
    tree_tay.changes_num = 0;

    tree_dif.root = Differentiator(tree->root, names[0]);
    Simplification(&tree_dif);
    printf("||");
    tree_tay.root = MaclaurinExpansion(tree, 3, names);
    Simplification(&tree_tay);
    GraphicDump(&tree_tay, NULL);

    fprintf(To, "\\documentclass[a4paper,12pt]{article}\n"
                "\\usepackage[T1]{fontenc}\n"
                "\\usepackage[utf8]{inputenc}\n"
                "\\usepackage[english,russian]{babel}\n"
                "\\usepackage{amsmath,amsfonts,amssymb,amsthm,mathtools}\n"
                "\\usepackage[left=10mm, top=10mm, right=10mm, bottom=20mm, nohead, nofoot]{geometry}\n"
                "\\usepackage{wasysym}\n"
                "\\author{\\LARGEКузнецов Ярослав Б01-306}\n"
                "\\title{Решение контрольной работы второго класса}\n"
                "\\pagestyle {empty}\n"
                "\\begin{document}\n"
                "\\maketitle\n"
                "\\begin{flushleft}\n"
                "\\Large\n");
    
    fprintf(To, "   Вашему вниманию представляется контрольная работа советского второклассника.\\\\\n"
                "   Это задание, было дано ученику первым номером, как самое простое:\\\\\n"
                "1. Найдите производную f(x);\\\\\n"
                "2. Разложите f(x) по формуле Тейлора при x --> 0;\\\\\n"
                "3. И тд.\\\\\n");


                //"$(cosx)^{sinx}\\cdot (cosx\\cdot \\ln{(cosx)}+sinx\\cdot \\dfrac{1.000}{cosx}\\cdot sinx\\cdot (-1.000))$\n"
                
    fprintf(To, "f (x) = $");
    LatexPrintNode(tree->root, To);
    
    fprintf(To, "\\\\\n");
    fprintf(To, "   Производная высчитывается при помощи элементарых правил арифметики и очевидных преобразований\\\\\n");

    fprintf(To, "f'(x) = $");
    LatexPrintNode(tree_dif.root, To);
    fprintf(To, "\\\\\n");
    
    fprintf(To, "   Разложение по Макарону:\\\\\n");
    fprintf(To, "f(x) = ");
    LatexPrintNode(tree_tay.root, To);
    
    fprintf(To, " + o( x ^ 5 )\\\\\n");

    fprintf(To, "\\end{flushleft}\n"
                "\\end{document}\n");

    fclose(To);
    DeleteNode(tree_dif.root);
    DeleteNode(tree_tay.root);

    return NO_ERROR;
}

void PrintTangentEquation(Tree* tree, FILE* To, Table* names)
{
    fprintf(To, " %lg ", Evaluate(tree, tree->root, names));

    Tree tree_dif = {};
    tree_dif.root = Differentiator(tree->root, names[0]);
    tree_dif.num_names = 1;

    fprintf(To, " + %lg ", Evaluate(&tree_dif, tree_dif.root, names));
    fprintf(To, " * ( %s - %lg )", names[0].var_name, names[0].var_value);

    DeleteNode(tree_dif.root);
}

Node* MaclaurinExpansion(Tree* tree, size_t power, Table* names)
{
    char str[128] = {};

    Tree dif_next = {};
    Tree dif_prev = {};
    Tree tree_tay = {};
    Node** current = &(tree_tay.root);

    dif_prev.root = Copynator(tree->root);
    dif_prev.num_names = tree->num_names;
    size_t i = 0;
    for (i = 0; i <= power; i++)
    {
        // if (latex == true)
        // {
        //     // fprintf(To, " + \\dfrac{%lg}{%lu} ", Evaluate(&dif_prev, dif_prev.root, names), Factorial(i));
        //     // fprintf(To, " \\cdot (x ^ {%lu})", i);
        // }
        // else
        // {
            *current = CreateNode(OPERATOR, OP_ADD, CreateNode(OPERATOR, OP_MUL, 
                                                                                CreateNode(OPERATOR, OP_DIV, 
                                                                                                            CreateNode(NUM, Evaluate(&dif_prev, dif_prev.root, names), NULL, NULL), 
                                                                                                            CreateNode(NUM, Factorial(i), NULL, NULL)),
                                                                                CreateNode(FUNCTION, FUN_POW,
                                                                                                            CreateNode(VAR, 0, NULL, NULL),
                                                                                                            CreateNode(NUM, i, NULL, NULL))), 
                                                    NULL);
            current = &((*current)->right);

            // fprintf(To, " + (%lg) ", Evaluate(&dif_prev, dif_prev.root, names) / Factorial(i));
            // fprintf(To, " * (x ** %lu)", i);
        //}
        
        dif_next.root = Differentiator(dif_prev.root, names[0]);
        dif_next.num_names = 1;

        DeleteNode(dif_prev.root);
        dif_prev.root = dif_next.root;
        dif_next.root = NULL;
    }

    DeleteNode(dif_prev.root);

    return tree_tay.root;
}


size_t Factorial(size_t n)
{
    if (n == 0)
        return 1;
    
    return (n * Factorial(n - 1));
}


void BuildGraphic(char Type[], char ToGnuplot[], char FromGnuplot[], char* function1, char* function2, char xrange[], char yrange[], char title[])
{
    char str[1024]   = {};
    char func1[256]  = {};
    char betwen[3]   = {};
    char func2[256]  = {};
    char temp[64]    = {};


    sprintf(str,  "set terminal %s size 800, 600\n", Type);

    sprintf(temp, "set output \"%s\"\n", ToGnuplot);

    strcat(str, temp);

    sprintf(temp, "set xlabel \"X\"\n" "set ylabel \"F(X)\"\n" "set grid\n");

    strcat(str, temp);

    if (yrange)
        sprintf(temp, "set yrange%s\n", yrange);
    else
        sprintf(temp, "set yrange[-3:3]\n");

    strcat(str, temp);
    
    if (xrange)
        sprintf(temp, "set xrange%s\n", xrange);
    else
        sprintf(temp, "set xrange[-20:20]\n");

    strcat(str, temp);

    sprintf(temp, "set title \"%s\" font \"Helvetica Bold, 20\"\n", title);

    strcat(str, temp);

    strcat(str, "set terminal png size 800, 600\n""plot ");

    if (function1)
        sprintf(func1, "%s lc rgb \"red\"", function1);
    if ((function1) && (function2))
        sprintf(betwen, "%s", ",");
    if (function2)
        sprintf(func2, "%s lc rgb \"green\"", function2);

    FILE* ToGnup = fopen(FromGnuplot, "w");
    fprintf(ToGnup, "%s %s %s %s", str, func1, betwen, func2);
    fclose(ToGnup);
}

void ReadReadyFunctionFrom(char* function, char* filename)
{
    FILE* From = fopen(filename, "r");
    fread(function, Get_Size_File(filename), 1, From);
    fclose(From);
}