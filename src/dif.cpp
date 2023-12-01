#include "dif.h"


Node* Differentiator(Node* node, Var vars, FILE* To, bool need_print)
{
    if (need_print == true)
        PrintSolutionDiff(node, To, vars);

    #define _NUM(value) CreateNumber(value, NULL, NULL)

    if (node->type == NUM) return _NUM(0);

    if (node->type == VAR) 
    {
        if (strcmp(node->data.variable, vars.name) == 0)
            return _NUM(1);
        else
        {
            return _NUM(0);
        }
    }

    #define _ADD(L, R) CreateOperator(OP_ADD,  L, R)
    #define _SUB(L, R) CreateOperator(OP_SUB,  L, R)
    #define _MUL(L, R) CreateOperator(OP_MUL,  L, R)
    #define _DIV(L, R) CreateOperator(OP_DIV,  L, R)

    #define _SIN(L, R) CreateFunction(FUN_SIN, L, R)
    #define _COS(L, R) CreateFunction(FUN_COS, L, R)
    #define _POW(L, R) CreateFunction(FUN_POW, L, R)
    #define  _LN(L, R) CreateFunction(FUN_LN,  L, R) 

    #define _d(node) Differentiator(node, vars, To, need_print)
    #define _c(node) Copynator(node)
    
    #define _VAR(x) CreateVariable(x, NULL, NULL)

    if (node->type == OPERATOR)
    {
        switch(node->data.value_op)
        {
            case OP_ADD: return _ADD(_d(node->left), _d(node->right));
            
            case OP_SUB: return _SUB(_d(node->left), _d(node->right));
            
            case OP_MUL: return _ADD(_MUL(_d(node->left), _c(node->right)), _MUL(_c(node->left), _d(node->right)));
                                

            case OP_DIV: return _DIV(
                                     _SUB(
                                          _MUL(_d(node->left), _c(node->right)),
                                          _MUL(_d(node->right), _c(node->left))),
                                     _MUL(_c(node->right), _c(node->right)));
            
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
            
            case FUN_COS: return _SUB(_NUM(0), _MUL(_SIN(_c(node->left), _c(node->right)), _d(node->right)));
            
            case FUN_POW:  
                if (node->right->type == NUM)
                    return _MUL(_d(node->left), _MUL(_NUM(node->right->data.value), _POW(_c(node->left), _NUM(node->right->data.value - 1))));
                else if (node->left->type == NUM)
                    return _MUL(_d(node->left), _MUL(_LN(NULL, _c(node->left)), _c(node->right)));
                else
                    return _MUL(_ADD(_DIV(_MUL(_d(node->left), _c(node->right)), _c(node->left)), _MUL(_LN(NULL, _c(node->left)), _d(node->right))), _c(node));

            
            case FUN_SQRT: return _DIV(_MUL(_NUM(0.5), _d(node->right)), _c(node));

            case FUN_LN: return _MUL(_d(node->right), _DIV(_NUM(1), _c(node->right)));
            
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


double Evaluate(Tree* tree, Node* node, Var* vars)
{
    //if (!node) {return NAN;}
    if (node->type == NUM)
    {
        return node->data.value;
    }
    if (node->type == VAR)
    {
        for (size_t i = 0; i < tree->num_vars; i++)
        {
            if (strncmp(node->data.variable, vars[i].name, vars[i].name_size) == 0)
            {
                return vars[i].value;
            }
        }
    }
    double left = 0, right = 0;

    if (node->type == OPERATOR)
    {
        if (node->left)
            left  = Evaluate(tree, node->left, vars);
        if (node->right)
            right = Evaluate(tree, node->right, vars);

        if ((isEqual(right, 0)) && (node->data.value_op == OP_DIV))
        {
            printf("функция не существует в точке");
            exit(1);
            return NAN;
        }
        
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
            left  = Evaluate(tree, node->left, vars);
        }
        if (node->right)
        {
            right = Evaluate(tree, node->right, vars);
        }
        if ((isEqual(right, 0)) && (node->data.value_fun == FUN_LN))
        {
            printf("функция не существует в точке");
            exit(1);
            return NAN;
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

void LatexPrintBeginning(FILE* To)
{
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
    
    fprintf(To, "Вашему вниманию представляется контрольная работа советского второклассника.\\\\\n"
                "Это задание, было дано ученику первым номером, как самое простое:\\\\\n"
                "1. Найдите производную f(x);\\\\\n"
                "2. Разложите f(x) по формуле Тейлора при x --> 0;\\\\\n"
                "3. И тд.\\\\\n");
}
void LatexPrintEnding(FILE* To)
{
    fprintf(To, "\\end{flushleft}\n"
                "\\end{document}\n");
}

void PrintNameTable(Tree* tree, Var* vars)
{
    double x = 0;
    for (size_t i = 0; i < tree->num_vars; i++)
    {
        if (isEqual(vars[i].value, (double) INT_MAX))
        {
            printf("Введите чему равна переменная \"%s\" = ", vars[i].name);
            scanf("%lg", &x);
            vars[i].value = x;
            while (getchar() != '\n')
                ;
        }
        printf("vars[%lu] = \"%s\" = %3lg (длина имени переменной = %2lu)\n", i, vars[i].name, vars[i].value, vars[i].name_size);
    }
}

size_t GetRealVar(Var* vars)
{
    char str[MAX_SIZE_ARG] = {};
    printf("Введите относительно какой переменной посчитать производнуюю ");
    scanf("%s", str);
    while (getchar() != '\n')
        ;

    for (size_t i = 0; i < MAX_NUM_VARS - 1; i++)
    {
        if (strncmp(str, vars[i].name, vars[i].name_size) == 0)
        {   
            return i;
        }
    }

    return GetRealVar(vars);
}

void AddGraphics(FILE* To, Tree* tree1, Tree* tree2)
{
    FILE* FileFunc = fopen("function.txt", "w");
    PrintNode(tree1->root, FileFunc, IN_ORDER);
    fclose(FileFunc);

    FILE* TaylorFunc = fopen("taylor.txt", "w");
    PrintNode(tree2->root, TaylorFunc, IN_ORDER);
    fclose(TaylorFunc);
    
    char function1[1024] = {};
    char function2[4096] = {};
    ReadFromTextToBuffer(function1, "function.txt");
    ReadFromTextToBuffer(function2, "taylor.txt");
    printf("%s\n", function1);
    printf("%s\n", function2);

    BuildGraphic("pdf", "function1.pdf", "function1.txt", function1, function2, "[-3:3]", "[-10:10]", "Function and Taylor");
    system("gnuplot -c function1.txt");


    fprintf(To, "\\includepdf{function1.pdf}\n");
    return;
}



void PrintTangentEquation(Tree* tree, FILE* To, Var* vars, size_t real_var)
{
    fprintf(To, " %lg ", Evaluate(tree, tree->root, vars));

    Tree tree_dif = {};
    tree_dif.root = Differentiator(tree->root, vars[real_var], NULL, false);
    tree_dif.num_vars = 1;

    fprintf(To, " + %lg ", Evaluate(&tree_dif, tree_dif.root, vars));
    fprintf(To, " * ( %s - %lg )", vars[0].name, vars[0].value);

    DeleteNode(tree_dif.root);
}


void PrintSolutionDiff(Node* node, FILE* To, Var vars)
{
    tm time_random = {};
    int x = rand();

    size_t i = (size_t)((x - time_random.tm_sec) % NUM_PHRASES);

    fprintf(To, "%s\\\\\n", funny_phrases[i]);
    fprintf(To, "$(");
    LatexPrintNode(node, To);
    fprintf(To, ")' = ");
    
    if (node->type == NUM)
    {
        fprintf(To, " 0");
    }

    if (node->type == VAR) 
    {
        if (strcmp(node->data.variable, vars.name) == 0)
            fprintf(To, " 1");
        else 
            fprintf(To, " 0");
            //fprintf(To, " %s", node->data.variable);
    }

    
    if (node->type == OPERATOR)
    {
        switch(node->data.value_op)
        {
            case OP_ADD:    LatexPrintAdd(node, To); break;
            
            case OP_SUB:    LatexPrintSub(node, To); break;
        
            case OP_MUL:    LatexPrintMul(node, To); break;  
            
            case OP_DIV:    LatexPrintDiv(node, To); break;
          
            default:
                printf("extra error");
                break;
        }
    }

    if (node->type == FUNCTION)
    {
        switch(node->data.value_fun)
        {
            case FUN_SIN:   LatexPrintSin(node, To); break;
            
            case FUN_COS:   LatexPrintCos(node, To); break;

            case FUN_POW:  
                if (node->right->type == NUM)
                {   
                            LatexPrintPow(node, To); break;
                }
                else if (node->left->type == NUM)
                {
                            LatexPrintExp(node, To); break;
                }
                else
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
                    
                    //return _MUL(_ADD(_DIV(_MUL(_d(node->left), _c(node->right)), _c(node->left)), _MUL(_LN(NULL, _c(node->left)), _d(node->right))), _c(node));
                }

            
            case FUN_SQRT:  LatexPrintSqrt(node, To); break; 

            case FUN_LN:    LatexPrintLn(node, To); break;  
            
            
            default:
                printf("extra error");
                break;
        }    
    }   
    fprintf(To, "$\\\\\n");

    return;
}



Node* Taylortition(Tree* tree, size_t power, Var* vars, size_t real_var)
{
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
    dif_prev.num_vars = tree->num_vars;
    size_t i = 0;

    double value = 0;

    for (i = 0; i <= power; i++)
    {
        value = Evaluate(&dif_prev, dif_prev.root, vars);
        printf("<%lg>\n", value);
        printf("<%lu>", real_var);
        // if (value == NAN)
        // {
        //     printf("производная в этой точке %s = %lg не существует", vars[real_var].name, vars[real_var].value);
        //     exit(1);
        // }

        *current = _ADD(_MUL(_DIV(_NUM(value), _NUM((double) Factorial(i))), _POW(_SUB(_VAR(vars[real_var].name), _NUM(vars[real_var].value)), _NUM((double) i))), NULL);
    
        current = &((*current)->right);

        dif_next.root = Differentiator(dif_prev.root, vars[0], NULL, false);
        dif_next.num_vars = 1;

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




void BuildGraphic(const char* Type, const char* ToGnuplot, const char* FromGnuplot, char* function1, char* function2, const char* yrange, const char* xrange, const char* title)
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

    sprintf(temp, "set terminal %s size 800, 600\nplot", Type);

    strcat(str, temp);
    

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

void ReadFromTextToBuffer(char* function, const char* filename)
{
    FILE* From = fopen(filename, "r");
    fread(function, Get_Size_File(filename), 1, From);
    fclose(From);
}



bool isEqual(double arg1, double arg2)
{
    double modul = fabs(fabs(arg1) - fabs(arg2));

    if (modul < DBL_EPSILON)
        return true;
    else
        return false;

}

size_t Factorial(size_t n)
{
    if (n == 0)
        return 1;
    
    return (n * Factorial(n - 1));
}

bool isMul1(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_MUL) && (isEqual(kid->data.value, 1)))
        return true;
    return false;
}
bool isAdd0(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_ADD) && (isEqual(kid->data.value, 0)))
        return true;
    return false;
}
bool isSub0(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_SUB) && (isEqual(kid->data.value, 0)))
        return true;
    return false;
}
bool isPow1(Node* parent, Node* kid)
{
    if ((parent->data.value_fun == FUN_POW) && (isEqual(kid->data.value, 1)))
        return true;
    return false;
}
bool isDiv1(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_DIV) && (isEqual(kid->data.value, 1)))
        return true;
    return false;
}
bool isMul0(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_MUL) && (isEqual(kid->data.value, 0)))
        return true;
    return false;
}
bool isDiv0(Node* parent, Node* kid)
{
    if ((parent->data.value_op == OP_DIV) && (isEqual(kid->data.value, 0))) // only for node->left
        return true;
    return false;
}
bool isPow0(Node* parent, Node* kid)
{
    if ((parent->data.value_fun == FUN_POW) && (isEqual(kid->data.value, 0)))
        return true;
    return false;
}


TreeError RemoveDummyElements(Tree* tree, Node** node)
{
    if (!(*node)) {return NO_ERROR;}
    
    Node** left  = &(*node)->left;
    Node** right = &(*node)->right;

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

            check |= isMul0(*node, *right);

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
                default: printf("extra"); break;
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


void LatexPrintAdd(Node* node, FILE* To)
{
    fprintf(To, "(");
    LatexPrintNode(node->left, To);
    fprintf(To, ")'");
    fprintf(To, " + ");
    fprintf(To, "(");
    LatexPrintNode(node->right, To);
    fprintf(To, ")'"); 
}

void LatexPrintSub(Node* node, FILE* To)
{
    fprintf(To, "(");
    LatexPrintNode(node->left, To);
    fprintf(To, ")'");
    fprintf(To, " - ");
    fprintf(To, "(");
    LatexPrintNode(node->right, To);
    fprintf(To, ")'"); 
}

void LatexPrintMul(Node* node, FILE* To)
{
    fprintf(To, "(");
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
}

void LatexPrintDiv(Node* node, FILE* To)
{
    fprintf(To, " \\dfrac ");
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
}

void LatexPrintExpandPow(Node* node, FILE* To)
{

}

void LatexPrintExp(Node* node, FILE* To)
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

}

void LatexPrintPow(Node* node, FILE* To)
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
}

void LatexPrintCos(Node* node, FILE* To)
{
    fprintf(To, "-(");
    LatexPrintNode(node->right, To);
    fprintf(To, ")'");
    fprintf(To, " \\cdot ");
    fprintf(To, " sin(");
    LatexPrintNode(node->right, To);
    fprintf(To, ") ");
}

void LatexPrintSin(Node* node, FILE* To)
{
    fprintf(To, "(");
    LatexPrintNode(node->right, To);
    fprintf(To, ")'");
    fprintf(To, " \\cdot ");
    fprintf(To, " cos(");
    LatexPrintNode(node->right, To);
    fprintf(To, ") ");
}

void LatexPrintSqrt(Node* node, FILE* To)
{
    fprintf(To, " \\dfrac {");
    fprintf(To, "(");
    LatexPrintNode(node->right, To);
    fprintf(To, ")'}");
    fprintf(To, "{(2");
    fprintf(To, " \\cdot ");
    LatexPrintNode(node, To);
    fprintf(To, ")}");
}

void LatexPrintLn(Node* node, FILE* To)
{
    fprintf(To, "(");
    LatexPrintNode(node->right, To);
    fprintf(To, ")'");
    fprintf(To, " \\cdot ");
    fprintf(To, "( ");
    fprintf(To, " \\dfrac {1}");
    LatexPrintNode(node->left, To);
    fprintf(To, "{");
    LatexPrintNode(node->right, To);
    fprintf(To, "})");
}
