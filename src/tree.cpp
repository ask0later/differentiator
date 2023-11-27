#include "tree.h"

TreeError ConstructorTree(Tree* tree)
{
    assert(tree);

    tree->size = 0;
    tree->num_names = 0;
    tree->changes_num = 0;

    return NO_ERROR;
}

Node* CreateNode(Type type, double value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->left   = left;
    node->right  = right;

    node->type = type;

    // node->data.value_op  = NO_OP;
    // node->data.value_fun = NO_FUN;
    // node->data.variable  = NULL;

    if (node->type == OPERATOR)
        node->data.value_op  = (Operators) value;
    else if (node->type == NUM)
        node->data.value = value;
    else if (node->type == FUNCTION)
        node->data.value_fun = (Functions) value;
    else if (node->type == VAR)
        node->data.variable = strdup("x");

    return node;
}

TreeError LatexPrintNode(Node* node, FILE* To)
{
    if (!node) {return NO_ERROR;}
    if (To == NULL) {return FILE_NOT_OPEN;}
    
    TreeError error = NO_ERROR;
    
    fprintf(To, "( ");

    Order order = IN_ORDER;
    if (node->type == OPERATOR)
    {
        if (node->data.value_op == OP_DIV)
            order = PRE_ORDER;
    }

    if (order == PRE_ORDER)
    {
        LatexPrintObject(node, To);
        fprintf(To, "{ "); // for defra
    }

    if (node->left)
    {
        error = LatexPrintNode(node->left, To);
        if (error != NO_ERROR)
            return error;
    }

    if (order == PRE_ORDER)
    {
        fprintf(To, " } "); // for defra
    }

    if (order == IN_ORDER)
        LatexPrintObject(node, To);
    
    if (node->type == FUNCTION)
    {
        if ((node->data.value_fun == FUN_LN) || (node->data.value_fun == FUN_POW))
            fprintf(To, "{");
    }

    if (order == PRE_ORDER)
    {
        fprintf(To, "{ "); // for defra
    }

    if (node->right)
    {
        error = LatexPrintNode(node->right, To);
        if (error != NO_ERROR)
            return error;
    }
    
    if (order == PRE_ORDER)
    {
        fprintf(To, " } "); // for defra
    }

    if (node->type == FUNCTION)
    {
        if ((node->data.value_fun == FUN_LN) || (node->data.value_fun == FUN_POW))
            fprintf(To, "}");
    }

    fprintf(To, " ) ");
    
    return NO_ERROR;
}

TreeError PrintNode(Node* node, FILE* To, Order order_value)
{
    if (!node) {return NO_ERROR;}
    if (To == NULL) {return FILE_NOT_OPEN;}
    
    TreeError error = NO_ERROR;

    fprintf(To, "( ");

    if (order_value == PRE_ORDER)
        PrintObject(node, To);

    if (node->left)
    {
        error = PrintNode(node->left, To, order_value);
        if (error != NO_ERROR)
            return error;
    }
    
    if (order_value == IN_ORDER)
    {
        PrintObject(node, To);
    }
    
    if (node->right)
    {
        error = PrintNode(node->right, To, order_value);
        if (error != NO_ERROR)
            return error;
    }
    
    if (order_value == POST_ORDER)
        PrintObject(node, To);

    fprintf(To, " ) ");

    return NO_ERROR;
}

void LatexPrintObject(Node* node, FILE* To)
{
    if (node->type == NUM)
        fprintf(To, "%lg", node->data.value);
    else if (node->type == OPERATOR)
    {
        LatexPrintOperator(node->data.value_op, To);
    }
    else if (node->type == FUNCTION)
    {
        PrintFunction(node->data.value_fun, To);
    }
    else if (node->type == VAR)
    {
        fprintf(To, "%s", node->data.variable);
    }
}

void PrintObject(Node* node, FILE* To)
{
    if (node->type == NUM)
        fprintf(To, "%lg", node->data.value);
    else if (node->type == OPERATOR)
    {
        PrintOperator(node->data.value_op, To);
    }
    else if (node->type == FUNCTION)
    {
        PrintFunction(node->data.value_fun, To);
    }
    else if (node->type == VAR)
    {
        fprintf(To, "%s", node->data.variable);
    }
}

void PrintFunction(Functions value_Functions, FILE* To)
{
    switch(value_Functions)
    {
        case FUN_SIN:
            fprintf(To, " sin ");
            break;
        case FUN_COS:
            fprintf(To, " cos ");
            break;
        case FUN_POW:
            fprintf(To, " ^ ");
            break;
        case FUN_SQRT:
            fprintf(To, " sqrt ");
            break;
        case FUN_LN:
            fprintf(To, " ln ");
            break;    
        default:
            printf("extra");
            break;
    }
}

void LatexPrintOperator(Operators value_Operators, FILE* To)
{
    switch(value_Operators)
    {
        case OP_ADD:
            fprintf(To, " + ");
            break;
        case OP_SUB:
            fprintf(To, " - ");
            break;
        case OP_MUL:
            fprintf(To, " \\cdot ");
            break;
        case OP_DIV:
            fprintf(To, " \\dfrac ");
            break;
        default:
            printf("extra");
            break;
    }
}

void PrintOperator(Operators value_Operators, FILE* To)
{
    switch(value_Operators)
    {
        case OP_ADD:
            fprintf(To, " + ");
            break;
        case OP_SUB:
            fprintf(To, " - ");
            break;
        case OP_MUL:
            fprintf(To, " * ");
            break;
        case OP_DIV:
            fprintf(To, " / ");
            break;
        default:
            printf("extra");
            break;
    }
}

void DestructorTree(Tree* tree)
{
    tree->size = 99999;
    
    DeleteNode(tree->root);
}

void DeleteNode(Node* node)
{
    if (!node) return;

    if (node->type == VAR)
        free(node->data.variable);
    
    if (node->left)
        DeleteNode(node->left);
    if (node->right)
        DeleteNode(node->right);
    
    free(node);
    
    return;
}


TreeError PasteObject(Tree* tree, char* source, Node** node, Table* names)
{
    double value = 0;

    for (size_t i = 0; i < NUM_COMMANDS; i++)
    {
        if (strncmp(source, cmds[i].name, cmds[i].size_name) == 0)
        {
            (*node)->num_args = cmds[i].num_args;
            (*node)->type  = cmds[i].type;
            if ((*node)->type == OPERATOR)
                (*node)->data.value_op  = (Operators) cmds[i].value;
            else if ((*node)->type == FUNCTION)
                (*node)->data.value_fun = (Functions) cmds[i].value;
            return NO_ERROR;
        }
    }

    if (sscanf(source, "%lg", &value) == 1)
    {
        (*node)->type  = NUM;
        (*node)->data.value = value;
    }
    else
    {
        (*node)->type = VAR;

        for (size_t i = 0; i < MAX_NUM_VARS - 1; i++)
        {
            if (strcmp(source, names[i].var_name) == 0)
            {
                (*node)->data.variable = strdup(names[i].var_name);
                return NO_ERROR;
            }
        }

        (*node)->data.variable = strdup(source);
        names[tree->num_names].name_size = strlen(source);
        memcpy(names[tree->num_names].var_name, (*node)->data.variable, names[tree->num_names].name_size);
        tree->num_names++;
    }

    return NO_ERROR;
}


TreeError ReadTree(Tree* tree, Node** node, char** position, Order order_value, Table* names, Text buffer)
{
    if (*position - buffer.position >= (long int) buffer.size_buffer) {return READER_ERROR;}

    char source[MAX_SIZE_ARG] = {};
    TreeError error = NO_ERROR;
    
    SkipSpaces(position);

    if (**position == '(')
    {
        *node = CreateNode(NO_TYPE, 0, NULL, NULL);

        tree->size++;

        (*position)++;

        char* start_posirion = *position;

        Node* node_temp = CreateNode(NO_TYPE, 0, NULL, NULL);

        SkipSpaces(position);
        ReadObject(source, position);
        PasteObject(tree, source, &node_temp, names);

        UnaryorBinary unary_binary = (*node)->num_args;

        DeleteNode(node_temp);

        *position = start_posirion;

        if (order_value == PRE_ORDER)
        {
            SkipSpaces(position);
            ReadObject(source, position);
            PasteObject(tree, source, node, names);
        }

        if (unary_binary == UNARY)
        {
            (*node)->left = NULL;
        }
        else
        {
            error =  ReadTree(tree, &(*node)->left, position, order_value, names, buffer);
            if (error != NO_ERROR)
                return error;
        }

        if (order_value == IN_ORDER)
        {
            SkipSpaces(position);
            ReadObject(source, position);
            PasteObject(tree, source, node, names);
        }

        error = ReadTree(tree, &(*node)->right, position, order_value, names, buffer);
        if (error != NO_ERROR)
            return error;

        if (order_value == POST_ORDER)
        {
            SkipSpaces(position);
            ReadObject(source, position);
            PasteObject(tree, source, node, names);
        }
        
        SkipSpaces(position);

        if (**position == ')')
            (*position)++;
    }
    else if (**position == ')')
    {
        return NO_ERROR;
    }

    return NO_ERROR;
}

TreeError SkipSpaces(char** position)
{
    while (isspace(**position))
        (*position)++;

    return NO_ERROR;
}


TreeError ReadObject(char* source, char** position)
{
    size_t i = 0;

    while ((**position != ')') && (**position != '('))
    {
        source[i] = **position;
        (*position)++;
        i++;
    }
    
    //source[i - 1] = 0; // убирает последний пробел обЪекта
    //printf("%s\n", source);

    (*position)--;

    return NO_ERROR;
}


void DumpErrors(TreeError error)
{
    switch(error)
    {
        case NO_ERROR:
            return;
            break;
        case ERROR_LOOP:
            printf("error: loop in tree\n");
            break;
        case ERROR_ALLOCATION:
            printf("error: memory allocation is fail\n");
            break;
        case ERROR_CONST:
            printf("error: address array has no memory for all addresses "
            "(change the constant responsible for the maximum tree size) \n");
            break;
        case FILE_NOT_OPEN:
            printf("error: failed to open file \n");
            break;
        case ERROR_POSITIONING_FUNC:
            printf("error: error in file positioning functions\n");
            break;
        case DEFINE_IS_NULL:
            printf("error: define is null\n");
            break;
        case READER_ERROR:
            printf("error: reading is fail\n");
            break;
        case ELEMENT_NOT_FOUND:
            printf("error: object is not in the tree\n");
            break;
        default:
            printf("extra error\n");
            break;
    }
}



TreeError CheckNoLoop(Tree tree)
{
    if (2 * tree.size > MAX_SIZE_TREE) {return ERROR_CONST;}   

    Node* addresses[MAX_SIZE_TREE] = {};

    NodeTraversal(tree.root, addresses, 0);

    Qsort(addresses, 0, MAX_SIZE_TREE - 1);
    for (size_t i = 0; i < MAX_SIZE_TREE - 2; i++)
    {
        if ((addresses[i] == addresses[i + 1]) && (addresses[i] != NULL))
        {
            return ERROR_LOOP;
        }
    }
    return NO_ERROR;
}


TreeError Qsort(Node* addresses[], int first, int last)
{
    if (first < last)
    {
        int left = first, right = last;
        Node* middle = addresses[(left + right) / 2];
        do
        {
            while (addresses[left] < middle)
                left++;
            while (addresses[right] > middle)
                right--;
            if (left <= right)
            {
                Swap(addresses, left, right);
                left++;
                right--;
            }
        } while (left <= right);
        
        Qsort(addresses, first, right);
        Qsort(addresses, left, last);
    }

    return NO_ERROR;
}

TreeError Swap(Node* addresses[], int left, int right)
{
    Node* temp = 0;
    temp = addresses[left];
    addresses[left] = addresses[right];
    addresses[right] = temp;

    return NO_ERROR;
}


TreeError NodeTraversal(Node* node, Node* addresses[], size_t counter)
{
    if (!node) {return NO_ERROR;}
    addresses[counter] = node;
    
    NodeTraversal(node->left , addresses, 2 * counter + 1);
    NodeTraversal(node->right, addresses, 2 * counter + 2);

    return NO_ERROR;
}