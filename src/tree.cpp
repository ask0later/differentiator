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
    if (!node) {return 0;}

    node->type = VAR;

    node->left   = left;
    node->right  = right;

    

    node->data.variable = strdup(value);

    return node;
}

Node* CreateNumber(double value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->type = NUM;

    node->left   = left;
    node->right  = right;

    

    node->data.value = value;

    return node;
}

Node* CreateOperator(Operators value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->type = OPERATOR;

    node->left   = left;
    node->right  = right;

    

    node->data.value_op = value;

    return node;
}


Node* CreateNode(Type type, void* value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->left   = left;
    node->right  = right;

    node->type = type;

    // node->data.value_op  = NO_OP;
    // node->data.value_fun = NO_FUN;
    // node->data.variable  = NULL;

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


void SkipSpaces(Text* buf)
{
    while (isspace(buf->str[buf->position]))
        buf->position++;
}

void DeleteTokens(Token** tokens)
{
    for (size_t i = 0; i < MAX_NUM_TOKENS; i++)
    {
        if ((*tokens)[i].type == VAR)
            free((*tokens)[i].data.variable);
    }
}

void CreateTokens(Token** tokens, size_t* token_i, Text* buf)
{
    //printf("%lu and %lu\n", buf->position, buf->size_buffer);
    while (buf->str[buf->position] != '\0')
    {
        SkipSpaces(buf);
        
        if (buf->position == buf->size_buffer)
            return;

        ParseNumber(tokens, token_i, buf);
        
        ParseMathOperators(tokens, token_i, buf);

        ParseVariable(tokens, token_i, buf);
    }
    //printf("<<%c>>\n", buf->str[11]);
}   

void ParseVariable(Token** tokens, size_t* token_i, Text* buf)
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
        (*tokens)[*token_i].type = VAR;
        (*tokens)[*token_i].data.variable = strdup(var);
        (*token_i)++;
    }
}

void ParseMathOperators(Token** tokens, size_t* token_i, Text* buf)
{
    for (size_t i = 0; i < NUM_MATH_COMMANDS; i++)
    {
        //printf("<%s> and <%s> \n", op, cmds[i].name);
        if (strncmp(&(buf->str[buf->position]), math_cmds[i].name, math_cmds[i].size_name) == 0)
        {
            //printf("<%s> and <%s> \n", &(buf->str[buf->position]), cmds[i].name);
            //printf("");
            //printf("!%lu %lu!\n", buf->position, buf->size_buffer);
            buf->position += math_cmds[i].size_name;
            //printf("!%lu %lu!\n", buf->position, buf->size_buffer);
            
            //printf("fuck ~%d~", token_i);
            (*tokens)[*token_i].type = OPERATOR;
            //printf("~%d~", (*tokens)[token_i].type);
            (*tokens)[*token_i].data.value_op = (Operators) math_cmds[i].value;
            (*token_i)++;
        }
    }
}

void ParseNumber(Token** tokens, size_t* token_i, Text* buf)
{
    int val = 0;
    while (isdigit(buf->str[buf->position]))
    {
        val = 10 * val + buf->str[buf->position] - '0';
        buf->position++;
    }
    if (val != 0)
    {
        (*tokens)[*token_i].type = NUM;
        (*tokens)[*token_i].data.value = val;
        (*token_i)++;
    }
}

Node* GetG(Token* tokens, size_t* token_i, Var* vars)
{
    Node* current = GetExpression(tokens, token_i, vars);
    return current;
}

Node* GetExpression(Token* tokens, size_t* token_i, Var* vars)
{
    Node* val = GetTerm(tokens, token_i, vars);

    if (tokens[*token_i].type == OPERATOR)
        while ((tokens[*token_i].data.value_op == OP_ADD) || (tokens[*token_i].data.value_op == OP_SUB))
        {
            Operators op = tokens[*token_i].data.value_op;
            (*token_i)++;
            Node* val2 = GetTerm(tokens, token_i, vars);
            val = CreateOperator(op, val, val2);
        }
    
    return val;
}


Node* GetTerm(Token* tokens, size_t* token_i, Var* vars)
{
    Node* val = GetUnary(tokens, token_i, vars);
    
    if (tokens[*token_i].type == OPERATOR)
    {
        for (size_t i = 0; i < NUM_COMMANDS_T; i++)
        {
            if (cmdsT[i].value == tokens[*token_i].data.value_op)
            {
                Operators op = tokens[*token_i].data.value_op;
                (*token_i)++;
                Node* val2 = GetUnary(tokens, token_i, vars);
                val = CreateOperator(op , val, val2);
            }
        }
    }
    
    return val;
}

Node* GetUnary(Token* tokens, size_t* token_i, Var* vars)
{
    Node* val = NULL;
    if (tokens[*token_i].type == OPERATOR)
    {
        for (size_t i = 0; i < NUM_COMMANDS_U; i++)
        {
            if (cmdsU[i].value == tokens[*token_i].data.value_op)
            {
                Operators op = tokens[*token_i].data.value_op;
                (*token_i)++;
                Node* val2 = GetPrimaryExpression(tokens, token_i, vars);
                val = CreateOperator(op , val, val2);
            }
        }
    }
    
    if (val == NULL)
        val = GetPrimaryExpression(tokens, token_i, vars);

    return val;
}

Node* GetPrimaryExpression(Token* tokens, size_t* token_i, Var* vars)
{
    if (tokens[*token_i].type == OPERATOR)
    {
        if (tokens[*token_i].data.value_op == OP_LEFT_P)
        {
            (*token_i)++;
            Node* val = GetExpression(tokens, token_i, vars);
            if (tokens[*token_i].data.value_op == OP_RIGHT_P)
                (*token_i)++;
            return val;
        }
    }

    return GetC(tokens, token_i, vars);
}

Node* GetC(Token* tokens, size_t* token_i, Var* vars)
{
    if (tokens[*token_i].type == VAR)
    {
        Node* var = CreateVariable(tokens[*token_i].data.variable, NULL, NULL);
        vars[0].name_size = strlen(tokens[*token_i].data.variable);
        memcpy(vars[0].name, tokens[*token_i].data.variable, vars[0].name_size);
        vars[0].value = INT_MAX;
        (*token_i)++;
        return var;
    }
    return GetN(tokens, token_i);
}

Node* GetN(Token* tokens, size_t* token_i)
{
    double val = 0;
    if (tokens[*token_i].type == NUM)
    {
        val = tokens[*token_i].data.value;
        (*token_i)++;
    }

    return CreateNumber(val, NULL, NULL);
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
            fprintf(To, " ln ");
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