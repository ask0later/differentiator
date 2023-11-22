#include "tree.h"

TreeError ConstructorTree(Tree* tree)
{
    assert(tree);
    
    tree->root = CreateNode(NO_TYPE, NO_OP, NULL, NULL);
    if (tree->root == NULL) {return ERROR_ALLOCATION;}

    tree->size = 1;
    tree->num_names = 0;

    return NO_ERROR;
}

Node* CreateNode(Type type, double value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->left   = left;
    node->right  = right;

    node->type = type;

    node->data.value_op  = NO_OP;
    node->data.variable = NULL;

    if (node->type == OPERATOR)
        node->data.value_op  = (Operators) value;
    else if (node->type == NUM)
        node->data.value = value;

    return node;
}

TreeError PrintNode(Node* node, FILE* To, Order order_value)
{
    if (!node) {return NO_ERROR;}
    if (To == NULL) {return FILE_NOT_OPEN;}

    fprintf(To, "( ");

    if (order_value == PRE_ORDER)
    {
        if (node->type == NUM)
            fprintf(To, "%lg", node->data.value);
        else if (node->type == OPERATOR)
        {
            PrintOperator(node->data.value_op, To);
        }
        else if (node->type == VAR)
        {
            printf("%s", node->data.variable);
        }
    }

    TreeError error = PrintNode(node-> left, To, order_value);
    if (error != NO_ERROR)
        return error;

    
    if (order_value == IN_ORDER)
    {
        if (node->type == NUM)
            fprintf(To, "%lg", node->data.value);
        else if (node->type == OPERATOR)
        {
            PrintOperator(node->data.value_op, To);
        }
        else if (node->type == VAR)
        {
            printf("%s", node->data.variable);
        }
    }
    
    error = PrintNode(node->right, To, order_value);
    if (error != NO_ERROR)
        return error;
    
    if (order_value == POST_ORDER)
    {
        if (node->type == NUM)
            fprintf(To, "%lg", node->data.value);
        else if (node->type == OPERATOR)
        {
            PrintOperator(node->data.value_op, To);
        }
        else if (node->type == VAR)
        {
            printf("%s", node->data.variable);
        }
    }

    fprintf(To,") ");

    return NO_ERROR;
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

    //fprintf(stderr, "%d", node->type);
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
            (*node)->type  = OPERATOR;
            (*node)->data.value_op = cmds[i].value_op;
        }
    }

    if ((*node)->type == NO_TYPE)
    {
        if (sscanf(source, "%lg", &value) == 1)
        {
            (*node)->type  = NUM;
            (*node)->data.value = value;
        }
        else
        {
            (*node)->type = VAR;
            bool x = false;

            for (size_t i = 0; i < MAX_NUM_VARS - 1; i++)
            {
                if (strcmp(source, names[i].var_name) == 0)
                {
                    (*node)->data.variable = strdup(names[i].var_name);
                    x = true;
                }
            }
            if (x == false)
            {
                (*node)->data.variable = strdup(source);
                
                names[tree->num_names].name_size = strlen(source);

                memcpy(names[tree->num_names].var_name, (*node)->data.variable, names->name_size);
                
                tree->num_names++;
            }
        }
    }

    return NO_ERROR;
}


TreeError ReadTree(Tree* tree, Node** node, char** position, Order order_value, Table* names)
{
    char source[100] = {};
    SkipSpaces(position);

    if (**position == '(')
    {
        *node = CreateNode(NO_TYPE, 0, NULL, NULL);

        tree->size++;

        (*position)++;

        if (order_value == PRE_ORDER)
        {
            SkipSpaces(position);
            ReadObject(source, position);
            PasteObject(tree, source, node, names);
        }
        
        TreeError error =  ReadTree(tree, &(*node)->left, position, order_value, names);
        if (error != NO_ERROR)
            return error;

        if (order_value == IN_ORDER)
        {
            SkipSpaces(position);
            ReadObject(source, position);
            PasteObject(tree, source, node, names);
        }

        error = ReadTree(tree, &(*node)->right, position, order_value, names);
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
    // else
    // {
    //     return READER_ERROR;
    // }

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

    (*position)--;

    return NO_ERROR;
}

void TextDump(Tree* tree)
{
    PrintNode(tree->root, stdout, PRE_ORDER);
    printf("\n");
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
    //              ^_______ 2 tree.size
    NodeTraversal(tree.root, addresses, 0);

    Qsort(addresses, 0, MAX_SIZE_TREE - 1);
    for (size_t i = 0; i < MAX_SIZE_TREE - 2; i++)
    {
        if ((addresses[i] == addresses[i + 1]) && (addresses[i] != NULL))
        {
            return ERROR_LOOP; // loop
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