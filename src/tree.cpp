#include "tree.h"

TreeError ConstructorTree(Tree* tree)
{
    assert(tree);

    tree->size = 0;
    tree->num_vars = 0;
    tree->changes_num = 0;

    return NO_ERROR;
}

Node* CreateVariable(char* value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return NULL;}

    node->type = VAR;

    node->left   = left;
    node->right  = right;

    node->data.variable = strdup(value);

    return node;
}

Node* CreateNumber(double value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return NULL;}

    node->type = NUM;

    node->left   = left;
    node->right  = right;

    node->data.value = value;

    return node;
}

Node* CreateOperator(Operators value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return NULL;}

    node->type = OPERATOR;

    node->left   = left;
    node->right  = right;

    node->data.value_op = value;

    return node;
}


Node* CreateNode(Type type, void* value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return NULL;}

    node->left   = left;
    node->right  = right;

    node->type = type;

    switch (node->type)
    {
    case NUM:
        node->data.value = *((double*) value);
        break;
    case OPERATOR:
        node->data.value_op  = *((Operators*) value);
        break;
    case VAR:
        node->data.variable = strdup((const char*) value);
        break;
    default:
        break;
    }

    return node;
}

void DeleteToken(Node* node)
{
    if (node->type == VAR)
        free(node->data.variable);

    free(node);

    return;
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

void SkipSpaces(Text* buf)
{
    while (isspace(buf->str[buf->position]))
        buf->position++;
}

TreeError ConstructorTokens(Tokens* tkns, Text* buf)
{
    tkns->size = buf->size_buffer;
    tkns->position = 0;
    tkns->tokens = (Node**) calloc(tkns->size, sizeof(Node*));
    if (!tkns->tokens) return ALLOC_ERROR;
    
    return NO_ERROR;
}

TreeError DestructorTokens(Tokens* tkns)
{
    for (size_t i = 0; i < tkns->size; i++)
        DeleteToken(tkns->tokens[i]);

    tkns->size = (size_t) INT_MAX;
    tkns->position = (size_t) INT_MAX;
    free(tkns->tokens);
    
    return NO_ERROR;
}


TreeError CreateTokens(Tokens* tkns, Text* buf, Table* names)
{
    while (buf->str[buf->position] != '\0')
    {
        SkipSpaces(buf);
        
        if (buf->position == buf->size_buffer)
            return NO_ERROR;

        ParseNumber(tkns, buf);
        
        ParseMathOperators(tkns, buf);

        ParseVariable(tkns, buf, names);
    }

    tkns->tokens[tkns->position] = CreateOperator(END, NULL, NULL);

    tkns->position++;

    tkns->size = tkns->position;

    Node** ptr = (Node**) realloc(tkns->tokens, tkns->size * sizeof(Node*));
    if (ptr == NULL) {return ALLOC_ERROR;}

    tkns->tokens = ptr;

    tkns->position = 0;

    return NO_ERROR;
}   

TreeError ParseVariable(Tokens* tkns, Text* buf, Table* names)
{
    if (isalpha(buf->str[buf->position]))
    {
        char var[MAX_SIZE_NAME] = {};
        size_t i_var = 0;
        var[i_var] = buf->str[buf->position];
        i_var++;
        buf->position++;
        while (isalnum(buf->str[buf->position]) || buf->str[buf->position] == '_')
        {
            var[i_var] = buf->str[buf->position];
            buf->position++;
            i_var++;
        }
        tkns->tokens[tkns->position] = CreateVariable(var, NULL, NULL);
        tkns->position++;

        for (size_t i = 0; i < MAX_NUM_VARS - 1; i++)
        {
            if (strcmp(var, names->vars[i].name) == 0)
            {
                return NO_ERROR;
            }
        }
        names->vars[names->num_var].name_size = strlen(var);
        memcpy(names->vars[names->num_var].name, var, names->vars[names->num_var].name_size);
        names->vars[names->num_var].value = (double) INT_MAX;

        names->num_var++;
    }

    return NO_ERROR;
}

TreeError ParseMathOperators(Tokens* tkns, Text* buf)
{
    for (size_t i = 0; i < NUM_MATH_COMMANDS; i++)
    {
        if (strncmp(&(buf->str[buf->position]), math_cmds[i].name, math_cmds[i].size_name) == 0)
        {
            buf->position += math_cmds[i].size_name;
     
            tkns->tokens[tkns->position] = CreateOperator(math_cmds[i].value, NULL, NULL);
            tkns->position++;
        }
    }

    return NO_ERROR;
}

TreeError ParseNumber(Tokens* tkns, Text* buf)
{
    int val = 0;
    while (isdigit(buf->str[buf->position]))
    {
        val = 10 * val + buf->str[buf->position] - '0';
        buf->position++;
    }
    if (val != 0)
    {
        tkns->tokens[tkns->position] = CreateNumber(val, NULL, NULL);
        tkns->position++;    
    }
    
    return NO_ERROR;
}

Node* GetG(Tokens* tkns)
{
    Node* current = GetExpression(tkns);
    return current;
}

Node* GetExpression(Tokens* tkns)
{
    
    Node* value_1 = GetTerm(tkns);
    Node* value_3 = NULL;

    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        while ((tkns->tokens[tkns->position]->data.value_op == OP_ADD) || (tkns->tokens[tkns->position]->data.value_op == OP_SUB))
        {
            value_3 = tkns->tokens[tkns->position];
            printf("%d", tkns->tokens[tkns->position]->data.value_op);
            tkns->position++;
            
            Node* value_2 = GetTerm(tkns);

            value_3->left = value_1;
            value_3->right = value_2;

            value_1 = value_3;
        }
    }
    
    return value_1;
}


Node* GetTerm(Tokens* tkns)
{
    Node* value_1 = GetUnary(tkns);
    
    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        for (size_t i = 0; i < NUM_COMMANDS_T; i++)
        {
            if (cmdsT[i].value == tkns->tokens[tkns->position]->data.value_op)
            {
                Node* value_3 = tkns->tokens[tkns->position];
                tkns->position++;
                
                Node* value_2 = GetUnary(tkns);
                value_3->left = value_1;
                value_3->right = value_2;

                value_1 = value_3;
            }
        }
    }
    
    return value_1;
}

Node* GetUnary(Tokens* tkns)
{
    Node* value_1 = NULL;
    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        for (size_t i = 0; i < NUM_COMMANDS_U; i++)
        {
            if (cmdsU[i].value == tkns->tokens[tkns->position]->data.value_op)
            {
                value_1 = tkns->tokens[tkns->position];
                tkns->position++;
                Node* value_2 = GetPrimaryExpression(tkns);

                value_1->right = value_2;
            }
        }
    }
    
    if (value_1 == NULL)
        value_1 = GetPrimaryExpression(tkns);

    return value_1;
}

Node* GetPrimaryExpression(Tokens* tkns)
{
    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        if (tkns->tokens[tkns->position]->data.value_op == L_BRACKET)
        {
            tkns->position++;
            Node* val = GetExpression(tkns);
            if (tkns->tokens[tkns->position]->data.value_op == R_BRACKET)
                tkns->position++;
            else
                return NULL;

            return val;
        }
    }

    return GetC(tkns);
}

Node* GetC(Tokens* tkns)
{
    if (tkns->tokens[tkns->position]->type == VAR)
    {
        Node* var = tkns->tokens[tkns->position];
        tkns->position++;
        return var;
    }
    return GetN(tkns);
}

Node* GetN(Tokens* tkns)
{

    if (tkns->tokens[tkns->position]->type == NUM)
    {
        Node* num = tkns->tokens[tkns->position];
        tkns->position++;
        return num;
    }

    return NULL;
}


TreeError LatexPrintNode(Node* node, FILE* To)
{
    if (!node) {return NO_ERROR;}
    if (To == NULL) {return FILE_NOT_OPEN;}
    
    TreeError error = NO_ERROR;
    
    if ((node->type != NUM) && (node->type != VAR))
        fprintf(To, "( ");

    Order order = IN_ORDER;
    if (node->type == OPERATOR)
    {
        if (node->data.value_op == OP_DIV)
            order = PRE_ORDER;
    }

    if (order == PRE_ORDER)
    {
        PrintObject(node, To, LATEX);
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
        PrintObject(node, To, LATEX);
    
    if (node->type == OPERATOR)
    {
        if ((node->data.value_op == FUN_LN) || (node->data.value_op == FUN_POW))
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

    if (node->type == OPERATOR)
    {
        if ((node->data.value_op == FUN_LN) || (node->data.value_op == FUN_POW))
            fprintf(To, "}");
    }

    if ((node->type != NUM) && (node->type != VAR))
        fprintf(To, " ) ");
    
    return NO_ERROR;
}



TreeError PrintNode(Node* node, FILE* To, Order order_value, for_what for_what)
{
    if (!node) {return NO_ERROR;}
    if (To == NULL) {return FILE_NOT_OPEN;}
    
    TreeError error = NO_ERROR;

    fprintf(To, "( ");

    if (order_value == PRE_ORDER)
        PrintObject(node, To, for_what);

    if (node->left)
    {
        error = PrintNode(node->left, To, order_value, for_what);
        if (error != NO_ERROR)
            return error;
    }
    
    if (order_value == IN_ORDER)
    {
        PrintObject(node, To, for_what);
    }
    
    if (node->right)
    {
        error = PrintNode(node->right, To, order_value, for_what);
        if (error != NO_ERROR)
            return error;
    }
    
    if (order_value == POST_ORDER)
        PrintObject(node, To, for_what);

    fprintf(To, " ) ");

    return NO_ERROR;
}

void PrintObject(Node* node, FILE* To, for_what for_what)
{
    if (node->type == NUM)
        fprintf(To, "%lg", node->data.value);
    else if (node->type == OPERATOR)
    {
        PrintOperator(node->data.value_op, To, for_what);
    }
    else if (node->type == VAR)
    {
        fprintf(To, "%s", node->data.variable);
    }
}


void PrintOperator(Operators value_Operators, FILE* To, for_what for_what)
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
            if (for_what == LATEX)
                fprintf(To, " \\cdot ");
            else
                fprintf(To, " * ");
            break;
        case OP_DIV:
            if (for_what == LATEX)
                fprintf(To, " \\dfrac ");
            else
                fprintf(To, " / ");
            break;
        case FUN_SIN:
            fprintf(To, " sin ");
            break;
        case FUN_COS:
            fprintf(To, " cos ");
            break;
        case FUN_POW:
            if (for_what == GNUPLOT)
                fprintf(To, "**");
            else
                fprintf(To, " ^ ");
            break;
        case FUN_SQRT:
            fprintf(To, " sqrt ");
            break;
        case FUN_LN:
            if (for_what == GNUPLOT)
                fprintf(To, " log ");
            else
                fprintf(To, " ln ");
            break;
        case OP_UN_SUB:
            fprintf (To, " - ");
            break;
        case L_BRACKET:
        case R_BRACKET:
        case END:
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

TreeError PasteObject(Tree* tree, char* source, Node** node, Var* names)
{
    double value = 0;

    for (size_t i = 0; i < NUM_MATH_COMMANDS; i++)
    {
        if (strncmp(source, math_cmds[i].name, math_cmds[i].size_name) == 0)
        {
            (*node)->num_args = math_cmds[i].num_args;
            (*node)->type  = math_cmds[i].type;
            if ((*node)->type == OPERATOR)
                (*node)->data.value_op  = (Operators) math_cmds[i].value;
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
            if (strcmp(source, names[i].name) == 0)
            {
                (*node)->data.variable = strdup(names[i].name);
                return NO_ERROR;
            }
        }
        names[tree->num_vars].name_size = strlen(source);
        (*node)->data.variable = strndup(source, names[tree->num_vars].name_size);
        memcpy(names[tree->num_vars].name, (*node)->data.variable, names[tree->num_vars].name_size);
        names[tree->num_vars].value = (double) INT_MAX;

        tree->num_vars++;
    }

    return NO_ERROR;
}


TreeError ReadTree(Tree* tree, Node** node, char** position, Order order_value, Var* names, Text buffer)
{
    if (*position - buffer.str >= (long int) buffer.size_buffer) {return READER_ERROR;}

    char source[MAX_SIZE_ARG] = {};
    TreeError error = NO_ERROR;
    
    SkipSpaces(position);

    if (**position == '(')
    {
        *node = CreateNode((Type) NULL, 0, NULL, NULL);

        tree->size++;

        (*position)++;

        char* start_posirion = *position;

        Node* node_temp = CreateNode((Type) NULL, 0, NULL, NULL);

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
    //printf("!%s!\n", source);
    if (i != 0)
        source[i - 1] = '\0'; // убирает последний пробел обЪекта
    //printf("%s\n", source);

    (*position)--;

    return NO_ERROR;
}

void DumpTokens(Tokens* tkns)
{
    for (size_t i = 0; i < tkns->size; i++)
    {
        if (tkns->tokens[i]->type == OPERATOR)
        {
            printf("operator = %d;\n", tkns->tokens[i]->data.value_op);
        }
        else if (tkns->tokens[i]->type == NUM)
        {
            printf("num = %lg;\n", tkns->tokens[i]->data.value);
        }
        else if (tkns->tokens[i]->type == VAR)
        {
            printf("var = <%s>;\n", tkns->tokens[i]->data.variable);
        }
    }
}


void DumpErrors(TreeError error)
{
    switch(error)
    {
        case NO_ERROR:
            return;
            break;
        case LOOP_ERROR:
            printf("error: loop in tree\n");
            break;
        case ALLOC_ERROR:
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
            return LOOP_ERROR;
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