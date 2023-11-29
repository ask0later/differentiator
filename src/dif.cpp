#include "dif.h"


Node* Differentiator(Node* node, Var var, FILE* To, bool need_print)
{
    if (need_print == true)
        PrintSolutionDiff(node, To);

    #define _NUM(value) CreateNumber(value, NULL, NULL)

    if (node->type == NUM) return _NUM(0);
                                //CreateNumber(0, NULL, NULL);

    if (node->type == VAR) 
    {
        if (strcmp(node->data.variable, var.name) == 0)
            return _NUM(1);
                    //CreateNumber(1, NULL, NULL);
        else
            return _NUM(0);
                    //CreateNumber(0, NULL, NULL);
    }

    #define _ADD(L, R) CreateOperator(OP_ADD,  L, R)
    #define _SUB(L, R) CreateOperator(OP_SUB,  L, R)
    #define _MUL(L, R) CreateOperator(OP_MUL,  L, R)
    #define _DIV(L, R) CreateOperator(OP_DIV,  L, R)

    #define _SIN(L, R) CreateFunction(FUN_SIN, L, R)
    #define _COS(L, R) CreateFunction(FUN_COS, L, R)
    #define _POW(L, R) CreateFunction(FUN_POW, L, R)
    #define  _LN(L, R) CreateFunction(FUN_LN,  L, R) 

    #define _d(node) Differentiator(node, var, To, need_print)
    #define _c(node) Copynator(node)
    
    #define _VAR(x) CreateVariable(x, NULL, NULL)

    if (node->type == OPERATOR)
    {
        switch(node->data.value_op)
        {
            case OP_ADD: return _ADD(_d(node->left), _d(node->right));//CreateOperator(OP_ADD, Differentiator(node->left, name), Differentiator(node->right, name));
            
            case OP_SUB: return _SUB(_d(node->left), _d(node->right));//CreateOperator(OP_SUB, Differentiator(node->left, name), Differentiator(node->right, name));
            
            case OP_MUL: return _ADD(_MUL(_d(node->left), _c(node->right)), _MUL(_c(node->left), _d(node->right)));//CreateOperator(OP_ADD, CreateOperator(OP_MUL, Differentiator(node->left, name), Copynator(node->right)), CreateOperator(OP_MUL, Copynator(node->left), Differentiator(node->right, name)));
                                

            case OP_DIV: return _DIV(
                                     _SUB(
                                          _MUL(_d(node->left), _c(node->right)),
                                          _MUL(_d(node->right), _c(node->left))),
                                     _MUL(_c(node->right), _c(node->right)));
                                //CreateOperator(OP_DIV, CreateOperator(OP_SUB, CreateOperator(OP_MUL, Differentiator(node->left, name), Copynator(node->right)), CreateOperator(OP_MUL, Copynator(node->left), Differentiator(node->right, name))),
                                //CreateOperator(OP_MUL, Copynator(node->right), Copynator(node->right)));
            
            default:
                printf("extra error");
                break;
        }
    }

    if (node->type == FUNCTION)
    {
        switch(node->data.value_fun)
        {
            case FUN_SIN:  return _MUL(_COS(_c(node->left), _c(node->right)), _d(node->right));
            //CreateOperator(OP_MUL, CreateFunction(FUN_COS, Copynator(node->left), Copynator(node->right)), Differentiator(node->right, name));

            case FUN_COS: return _SUB(_NUM(0), _MUL(_SIN(_c(node->left), _c(node->right)), _d(node->right)));
            //CreateOperator(OP_SUB, CreateNumber(0, NULL, NULL), CreateOperator(OP_MUL, CreateFunction(FUN_SIN, Copynator(node->left), Copynator(node->right)), Differentiator(node->right, name)));

            case FUN_POW:  
                if (node->right->type == NUM)
                {
                    return _MUL(_d(node->left), _MUL(_NUM(node->right->data.value), _POW(_c(node->left), _NUM(node->right->data.value - 1))));
                    //CreateOperator(OP_MUL, CreateNumber(node->right->data.value, NULL, NULL), CreateFunction(FUN_POW, Copynator(node->left), CreateNumber(node->right->data.value - 1, NULL, NULL)));
                }
                else if (node->left->type == NUM)
                {
                    return _MUL(_d(node->left), _MUL(_LN(NULL, _c(node->left)), _c(node->right)));
                                // CreateOperator(OP_MUL,
                                //                        Differentiator(node->left, name), 
                                //                        CreateOperator(OP_MUL,
                                //                                                    CreateFunction(FUN_LN, NULL, Copynator(node->left)),
                                //                                                    Copynator(node->right)));
                }
                else
                {
                    return _MUL(_ADD(_DIV(_MUL(_d(node->left), _c(node->right)), _c(node->left)), _MUL(_LN(NULL, _c(node->left)), _d(node->right))), _c(node));
                            // CreateOperator(OP_MUL,
                            //                             CreateOperator(OP_ADD, 
                            //                                                         CreateOperator(OP_DIV, 
                            //                                                                                     CreateOperator(OP_MUL, Differentiator(node->left, name), Copynator(node->right)),
                            //                                                                                     Copynator(node->left)),
                            //                                                         CreateOperator(OP_MUL,
                            //                                                                                     CreateFunction(FUN_LN, NULL, Copynator(node->left)),
                            //                                                                                     Differentiator(node->right, name))),
                            //                             Copynator(node));
                }

            
            case FUN_SQRT: return _DIV(_MUL(_NUM(0.5), _d(node->right)), _c(node));

                                //_DIV(_SUB(_MUL(_d(node->left), _c(node->right)), _MUL(_c(node->left), _d(node->right))), _MUL(_c(node->right), _c(node->right)));
                                // CreateOperator(OP_DIV, CreateOperator(OP_SUB, CreateOperator(OP_MUL, Differentiator(node->left, name), Copynator(node->right)), CreateOperator(OP_MUL, Copynator(node->left), Differentiator(node->right, name))),
                                //                        CreateOperator(OP_MUL, Copynator(node->right), Copynator(node->right)));

            case FUN_LN: return _MUL(_d(node->right), _DIV(_NUM(1), _c(node->right)));
                                //CreateOperator(OP_MUL, Differentiator(node->right, name), CreateOperator(OP_DIV, CreateNumber(1, NULL, NULL), Copynator(node->right) ) );
            
            default:
                printf("extra error");
                break;
        }
    }
    
    return NULL;

    #undef _ADD
    #undef _SUB
    #undef _MUL
    #undef _DIV

    #undef _SIN
    #undef _COS
    #undef _POW
    #undef  _LN 

    #undef _d
    #undef _c
    
    #undef _VAR
}

Node* Copynator(Node* node)
{
    if (!node) return NULL;

    Node* new_node;

    if (node->type == NUM)
        new_node = CreateNumber(node->data.value, Copynator(node->left), Copynator(node->right));
        //value = (void*) &node->data.value;
    else if (node->type == OPERATOR)
        new_node = CreateOperator(node->data.value_op, Copynator(node->left), Copynator(node->right));
        //value = (void*) &node->data.value_op;
    else if (node->type == FUNCTION)
        new_node = CreateFunction(node->data.value_fun, Copynator(node->left), Copynator(node->right));
        //value = (void*) &node->data.value_fun;
    else if (node->type == VAR)
        new_node = CreateVariable(node->data.variable, Copynator(node->left), Copynator(node->right));
        //value = (void*) node->data.value_fun;


    //Node* new_node = CreateNode(node->type, value, Copynator(node->left), Copynator(node->right));

    return new_node;
}


double Evaluate(Tree* tree, Node* node, Var* names)
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
            if (strncmp(node->data.variable, names[i].name, names[i].name_size) == 0)
            {
                return names[i].value;
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

            default: printf("new operator"); return NAN;
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
        switch(node->data.value_fun)
        {
            case FUN_SIN:  return (sin(right));

            case FUN_COS:  return (cos(right));

            case FUN_POW:  return (pow(left, right));

            case FUN_SQRT: return (sqrt(right));

            case FUN_LN:   return (log(right));

            default: printf("new function"); return NAN;
        }
    }
    return NAN;
}



void PrintNameTable(Tree* tree, Var* names)
{
    for (size_t i = 0; i < tree->num_names; i++)
    {
        printf("%15s = %3lg (длина имени переменной = %2lu)\n", names[i].name, names[i].value, names[i].name_size);
    }
}

bool isMul1(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_MUL) && (kid->data.value == 1))
        return true;
    return false;
}
bool isAdd0(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_ADD) && (kid->data.value == 0))
        return true;
    return false;
}
bool isSub0(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_SUB) && (kid->data.value == 0))
        return true;
    return false;
}
bool isPow1(Node* parent, Node* kid)
{
    if ((parent->data.value_fun == FUN_POW) && (kid->data.value == 1))
        return true;
    return false;
}
bool isDiv1(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_DIV) && (kid->data.value == 1))
        return true;
    return false;
}
bool isMul0(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_MUL) && (kid->data.value == 0))
        return true;
    return false;
}
bool isDiv0(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_DIV) && (kid->data.value == 0)) // only for node->left
        return true;
    return false;
}
bool isPow0(Node* parent, Node* kid)
{
    if ((parent->data.value_fun == FUN_POW) && (kid->data.value == 0))
        return true;
    return false;
}

//CheckСonditionDummy();

TreeError RemoveDummyElements(Tree* tree, Node** node)
{
    if (!(*node)) {return NO_ERROR;}
    
    Node** left  = &(*node)->left;
    Node** right = &(*node)->right;
    double value = 0;

    Node* copy_node = {};

    bool check = false;
    if (((*node)->right))
    {
        if (((*node)->type == OPERATOR) && ((*right)->type == NUM))
        {
            check |= isMul1(*node, *right);
            check |= isAdd0(*node, *right);
            check |= isSub0(*node, *right);
            //check |= isPow1(*node, *right);
            check |= isDiv1(*node, *right);

            if (check == true)
            {
                copy_node = Copynator((*node)->left);
                tree->changes_num++;
                DeleteNode(*node);
                *node = copy_node;
                
                return NO_ERROR;
            }

            //check |= isMul0(*node, *right);

            if (check == true)
            {
                tree->changes_num++;
                DeleteNode(*node);
                *node = CreateNumber(0, NULL, NULL);
                
                return NO_ERROR;
            }

            if (check == false) 
            {
                RemoveDummyElements(tree, left);
                RemoveDummyElements(tree, right);
            }
        }
        else if (((*node)->type == FUNCTION) && ((*right)->type == NUM))
        {
            check |= isPow1(*node, *right);

            if (check == true)
            {
                copy_node = Copynator((*node)->left);
                tree->changes_num++;
                DeleteNode(*node);
                *node = copy_node;

                
                return NO_ERROR;
            }

            check |= isPow0(*node, *right);

            if (check == true)
            {
                tree->changes_num++;
                DeleteNode(*node);
                *node = CreateNumber(1, NULL, NULL);
                
                return NO_ERROR;
            }
        }
        else
        {
            RemoveDummyElements(tree, left);
            RemoveDummyElements(tree, right);
        }
    }
    if (((*node)->left))
    {   
        if (((*node)->type == OPERATOR) && ((*left)->type == NUM))
        {
            check |= isMul1(*node, *left);
            check |= isAdd0(*node, *left);
            //check |= isSub0(*node, *left);
            //check |= isPow1(**node, **right);
            //check |= isDiv1(*node, *left);

            if (check == true)
            {
                copy_node = Copynator((*node)->right);
                tree->changes_num++;
                DeleteNode(*node);
                *node = copy_node;
                
                return NO_ERROR;
            }

            check |= isMul0(*node, *left);
            check |= isDiv0(*node, *left);

            if (check == true)
            {
                tree->changes_num++;
                DeleteNode(*node);
                *node = CreateNumber(0, NULL, NULL);

                return NO_ERROR;
            }

            if (check == false) 
            {
                RemoveDummyElements(tree, left);
                RemoveDummyElements(tree, right);
            }
        }
        else
        {
            RemoveDummyElements(tree, left);
            RemoveDummyElements(tree, right);
        }
    }
    return NO_ERROR;
}

TreeError CollapsingConstants(Tree* tree, Node** node)
{
    if (!(*node)) {return NO_ERROR;}

    Node** left  = &(*node)->left;
    Node** right = &(*node)->right;

    if (((*node)->left == NULL) || ((*node)->right == NULL))
        return NO_ERROR;

    if ((*node)->type == OPERATOR)
    {
        if (((*node)->right) && ((*node)->left))
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
            *node = CreateNumber(value, NULL, NULL);
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
        RemoveDummyElements(tree, &(tree->root));
        CollapsingConstants(tree, &(tree->root));
        
    } while (start_changes_num != tree->changes_num);

    return NO_ERROR;
}

TreeError PrintfLatex(Tree* tree, Var* names)
{
    FILE* To = fopen("latex.txt", "w");

    Tree tree_dif = {};
    Tree tree_tay = {};

    tree_dif.num_names = 1;
    tree_tay.num_names = 1;
    
    tree_dif.changes_num = 0;
    tree_tay.changes_num = 0;


    names[0].value = 0;

    size_t power = 6;
    tree_tay.root = TaylorExpansion(tree, power, names, names[0]);

    Simplification(&tree_tay);

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


    fprintf(To, "f (x) = \\\\$");
    LatexPrintNode(tree->root, To);
    
    fprintf(To, "\\\\\n");
    fprintf(To, "   Производная высчитывается при помощи элементарых правил арифметики и очевидных преобразований\\\\\n");

    tree_dif.root = Differentiator(tree->root, names[0], To, true);
    Simplification(&tree_dif);

    GraphicDump(tree, &tree_dif);


    fprintf(To, "f'(x) = \\\\$");
    LatexPrintNode(tree_dif.root, To);
    fprintf(To, "\\\\\n");
    
    fprintf(To, "   Разложение по Тейлору:\\\\\n");
    fprintf(To, "f(x) = \\\\$");
    LatexPrintNode(tree_tay.root, To);
    
    fprintf(To, " + o( ( x - %lg ) ^ %lu )\\\\\n", names[0].value, power);

    //fprintf(To, "\\includegraphics{%s}")

    fprintf(To, "\\end{flushleft}\n"
                "\\end{document}\n");

    fclose(To);
    DeleteNode(tree_dif.root);
    DeleteNode(tree_tay.root);

    return NO_ERROR;
}

void PrintTangentEquation(Tree* tree, FILE* To, Var* names)
{
    fprintf(To, " %lg ", Evaluate(tree, tree->root, names));

    Tree tree_dif = {};
    tree_dif.root = Differentiator(tree->root, names[0], NULL, false);
    tree_dif.num_names = 1;

    fprintf(To, " + %lg ", Evaluate(&tree_dif, tree_dif.root, names));
    fprintf(To, " * ( %s - %lg )", names[0].name, names[0].value);

    DeleteNode(tree_dif.root);
}

void PrintSolutionDiff(Node* node, FILE* To)
{
    size_t i = (size_t)(rand() % NUM_PHRASES);

    fprintf(To, "%s\n\\\\$", funny_phrases[i]);
    fprintf(To, "(");
    LatexPrintNode(node, To);
    fprintf(To, ")' = ");
    printf("!%d!", node->type);
    if (node->type == NUM)
    {
        fprintf(To, " 0\n\\\\");
    }

    if (node->type == VAR) 
    {
        //if (strcmp(node->data.variable, var.name) == 0)
            fprintf(To, " 1\n\\\\");
        //else
          //  fprintf(To, " 0\n\\\\");
    }

    
    if (node->type == OPERATOR)
    {
        switch(node->data.value_op)
        {
            case OP_ADD:    
                            fprintf(To, "(");
                            LatexPrintNode(node->left, To);
                            fprintf(To, ")'");
                            fprintf(To, " + ");
                            fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")'");  
                            break;
            
            case OP_SUB:    fprintf(To, "(");
                            LatexPrintNode(node->left, To);
                            fprintf(To, ")'");
                            fprintf(To, " - ");
                            fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")'");  
                            break;
        
            case OP_MUL:    fprintf(To, "(");
                            LatexPrintNode(node->left, To);
                            fprintf(To, ")'");
                            fprintf(To, " \\cdot ");
                            fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")");
                            fprintf(To, " + ");
                            fprintf(To, "(");
                            LatexPrintNode(node->left, To);
                            fprintf(To, ")");
                            fprintf(To, " \\cdot ");
                            fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")'");    
                            break;  
            
            case OP_DIV:    fprintf(To, " \\dfrac ");
                            fprintf(To, "{ (");
                            LatexPrintNode(node->left, To);
                            fprintf(To, ")'");
                            fprintf(To, " \\cdot ");
                            fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")");
                            fprintf(To, " - ");
                            fprintf(To, "(");
                            LatexPrintNode(node->left, To);
                            fprintf(To, ")");
                            fprintf(To, " \\cdot ");
                            fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")' }");
                            fprintf(To, "{ ");
                            fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")");
                            fprintf(To, " \\cdot ");
                            fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")");
                            break;
          
            default:
                printf("extra error");
                break;
        }
    }

    if (node->type == FUNCTION)
    {
        switch(node->data.value_fun)
        {
            case FUN_SIN:   fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")'");
                            fprintf(To, " \\cdot ");
                            fprintf(To, " cos(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ") ");
                            break;
            
            case FUN_COS:   fprintf(To, "-(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")'");
                            fprintf(To, " \\cdot ");
                            fprintf(To, " sin(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ") ");
                            break;

            case FUN_POW:  
                if (node->right->type == NUM)
                {   
                            fprintf(To, "(");
                            LatexPrintNode(node->left, To);
                            fprintf(To, ")'");
                            fprintf(To, " \\cdot ");
                            fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")");
                            fprintf(To, " \\cdot ");
                            fprintf(To, "(");
                            LatexPrintNode(node->left, To);
                            fprintf(To, ") ");
                            fprintf(To, "^ {");
                            fprintf(To, "%lg", node->right->data.value - 1);
                            fprintf(To, "} ");
                            break;
                }
                else if (node->left->type == NUM)
                {
                            fprintf(To, "(");
                            LatexPrintNode(node->left, To);
                            fprintf(To, ")'");
                            fprintf(To, " \\cdot ");
                            fprintf(To, "( (ln{");
                            LatexPrintNode(node->left, To);
                            fprintf(To, "})");
                            fprintf(To, " \\cdot ");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")");   
                            break;
                }
            //     else
            //     {
            //         return _MUL(_ADD(_DIV(_MUL(_d(node->left), _c(node->right)), _c(node->left)), _MUL(_LN(NULL, _c(node->left)), _d(node->right))), _c(node));
            //                 // CreateOperator(OP_MUL,
            //                 //                             CreateOperator(OP_ADD, 
            //                 //                                                         CreateOperator(OP_DIV, 
            //                 //                                                                                     CreateOperator(OP_MUL, Differentiator(node->left, name), Copynator(node->right)),
            //                 //                                                                                     Copynator(node->left)),
            //                 //                                                         CreateOperator(OP_MUL,
            //                 //                                                                                     CreateFunction(FUN_LN, NULL, Copynator(node->left)),
            //                 //                                                                                     Differentiator(node->right, name))),
            //                 //                             Copynator(node));
            //     }

            
            case FUN_SQRT:  fprintf(To, " \\dfrac {");
                            fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")'}");
                            fprintf(To, "{(2");
                            fprintf(To, " \\cdot ");
                            LatexPrintNode(node, To);
                            fprintf(To, ")}");
                            break; 

            case FUN_LN:    fprintf(To, "(");
                            LatexPrintNode(node->right, To);
                            fprintf(To, ")'");
                            fprintf(To, " \\cdot ");
                            fprintf(To, "( ");
                            fprintf(To, " \\dfrac {1}");
                            LatexPrintNode(node->left, To);
                            fprintf(To, "{");
                            LatexPrintNode(node->right, To);
                            fprintf(To, "})");
                            break;  
            
            
            default:
                printf("extra error");
                break;
        }    
    }   
    fprintf(To, "\\\\\n");

    return;
}

Node* TaylorExpansion(Tree* tree, size_t power, Var* names, Var var)
{
    char str[128] = {};

    Tree dif_next = {};
    Tree dif_prev = {};
    Tree tree_tay = {};
    Node** current = &(tree_tay.root);

    #define _ADD(L, R) CreateOperator(OP_ADD, L, R)
    #define _SUB(L, R) CreateOperator(OP_SUB, L, R)
    #define _MUL(L, R) CreateOperator(OP_MUL, L, R)
    #define _DIV(L, R) CreateOperator(OP_DIV, L, R)
    #define _POW(L, R) CreateFunction(FUN_POW, L, R)
    #define _NUM(value) CreateNumber(value, NULL, NULL)
    #define _VAR(x) CreateVariable(x, NULL, NULL) 


    dif_prev.root = Copynator(tree->root);
    dif_prev.num_names = tree->num_names;
    size_t i = 0;
    for (i = 0; i <= power; i++)
    {
        *current = _ADD(_MUL(_DIV(_NUM(Evaluate(&dif_prev, dif_prev.root, names)), _NUM(Factorial(i))), _POW(_SUB(_VAR(names[0].name), _NUM(var.value)), _NUM((double) i))), NULL);
        
        // *current = CreateOperator(OP_ADD, CreateOperator(OP_MUL, 
        //                                                                     CreateOperator(OP_DIV, 
        //                                                                                                 CreateNumber(Evaluate(&dif_prev, dif_prev.root, names), NULL, NULL), 
        //                                                                                                 CreateNumber(Factorial(i), NULL, NULL)),
        //                                                                     CreateFunction(FUN_POW,
        //                                                                                                 CreateVariable(names[0].var_name, NULL, NULL),
        //                                                                                                 CreateNumber(i, NULL, NULL))),
        //                                         NULL);
        current = &((*current)->right);

        dif_next.root = Differentiator(dif_prev.root, names[0], NULL, false);
        dif_next.num_names = 1;

        DeleteNode(dif_prev.root);
        dif_prev.root = dif_next.root;
        dif_next.root = NULL;
    }

    DeleteNode(dif_prev.root);

    #undef _ADD 
    #undef _SUB 
    #undef _MUL 
    #undef _DIV 
    #undef _POW 
    #undef _NUM
    #undef _VAR

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
