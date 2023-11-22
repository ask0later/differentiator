#include "tree.h"

TreeError ConstructorTree(Tree* tree)
{
    assert(tree);
    
    tree->root = NewNode();
    if (tree->root == NULL) {return ERROR_ALLOCATION;}

    tree->size = 1;

    return NO_ERROR;
}

            //--//
double Evaluate(Node* node)
{
    if (node != NULL) {return 0;}
    if (node->type == NUM)
        return node->value;
    
    double left = 0, right = 0;

    if (!node->left)
        left  = Evaluate(node->left);
    if (!node->right)
        right = Evaluate(node->right);

    switch((int) node->value)
    {
        case OP_ADD: return left + right;

        case OP_SUB: return left - right;

        case OP_MUL: return left * right;

        case OP_DIV: return left / right;
    }
}

Node* NewNode()
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->left   = NULL;
    node->right  = NULL;
    node->value  = NULL;
    node->type   = NO_TYPE;

    return node;
}

TreeError PrintNode(Node* node, FILE* To, Order order_value)
{
    if (To == NULL) {return FILE_NOT_OPEN;}

    if (!node) {/*fprintf(To, "nil ");*/ return NO_ERROR;}

    fprintf(To, "( ");

    if (order_value == PRE_ORDER)
    {
        if (node->type == NUM)
            fprintf(To, "%lg", node->value);
        else
        {
            PrintOperator((OPERATORS) node->value, To);
        }
    }

    TreeError error = PrintNode(node-> left, To, order_value);
    if (error != NO_ERROR)
        return error;

    
    if (order_value == IN_ORDER)
    {
        if (node->type == NUM)
            fprintf(To, "%lg", node->value);
        else
        {
            PrintOperator((OPERATORS) node->value, To);
        }
    }
    
    error = PrintNode(node->right, To, order_value);
    if (error != NO_ERROR)
        return error;
    
    if (order_value == POST_ORDER)
    {
        if (node->type == NUM)
            fprintf(To, "%lg ", node->value);
        else
        {
            PrintOperator((OPERATORS) node->value, To);
        }
    }

    fprintf(To,") ");

    return NO_ERROR;
}

void PrintOperator(OPERATORS value, FILE* To)
{
    switch(value)
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

    DeleteNode(node->left);
    DeleteNode(node->right);
    
    free(node);
    return;
}

TreeError ReadTree(Tree* tree, Node** node, FILE* From, Order order_value)
{
    if (From == NULL) {return FILE_NOT_OPEN;}
    char ptr[MAX_SIZE_ARG] = {};
    char* source;
    source = ptr;
    
    if (fscanf(From, "%s", ptr) == EOF) {return LIB_IS_EMPTY;}
    // if (fscanf(From, "%s", ptr) == 0)
    if (strcmp(source, "nil") == 0)
        return NO_ERROR;

    if (strcmp(source, "(") == 0)
    {
        *node = NewNode();
        tree->size++;

        if (*node == NULL) {return ERROR_ALLOCATION;}

        if (order_value == PRE_ORDER)
        {
            //ReadTextPhrase(source, From);
            PasteObject(source, node);
        }
        
        TreeError error = ReadTree(tree, &(*node)->left, From, order_value);
        if (error != NO_ERROR)
            return error;

        if (order_value == IN_ORDER)
        {
            //ReadTextPhrase(source, From);
            PasteObject(source, node);
        }

        error = ReadTree(tree, &(*node)->right, From, order_value);
        if (error != NO_ERROR)
            return error;
        
        if (order_value == POST_ORDER)
        {
            //ReadTextPhrase(source, From);
            PasteObject(source, node);
        }

        fscanf(From, "%s", source);
    }

    return NO_ERROR;
}

TreeError PasteObject(char* source, Node** node)
{
    double value = 0;
    fprintf(stderr, "|%s|", source);

    if (strncmp(source, "add", 3) == 0)
    {
        (*node)->type  = OPERATOR;
        (*node)->value = OP_ADD;
    } 
    else if (strncmp(source, "sub", 3) == 0)
    {
        fprintf(stderr, "НУ И ЧТО");
        (*node)->type  = OPERATOR;
        (*node)->value = OP_ADD; 
    }
    else if (strncmp(source, "mul", 3) == 0)
    {
        (*node)->type  = OPERATOR;
        (*node)->value = OP_MUL; 
    }
    else if (strncmp(source, "div", 3) == 0)
    {
        (*node)->type  = OPERATOR;
        (*node)->value = OP_DIV; 
    }
    else
    {
        sscanf(source, "%lg", &value);
        
        (*node)->type  = NUM; 
        (*node)->value = value;
    }

    return NO_ERROR;
}


TreeError NewReadTree(Tree* tree, Node** node, char** position, Order order_value)
{
    
    char source[100] = {};
    SkipSpaces(position);

    if (**position == '.')
    {   
        fprintf(stderr, "<%c>", **position);
        (*position)++;
        // fprintf(stderr, "<<%c>>", **position);
        // fprintf(stderr, "<<%c>>", *(*position + 1));

        return NO_ERROR; 
    }

    fprintf(stderr, ".%c.", **position);
    if (**position == '(')
    {

        *node = NewNode();
        //fprintf(stderr, "#узел#");

        tree->size++;


        (*position)++;

        if (order_value == PRE_ORDER)
        {
            SkipSpaces(position);
            ReadObject(source, position);
            PasteObject(source, node);
        }
        //fprintf(stderr, "#левая#");

        NewReadTree(tree, &(*node)->left, position, order_value);
        
        //fprintf(stderr, "#правая#");
        
        NewReadTree(tree, &(*node)->right, position, order_value);
        
        //fprintf(stderr, "#закрываю#");

        SkipSpaces(position);

        if (**position == ')')
        {
            fprintf(stderr, "<%c>", **position);
            fprintf(stderr, "OKEY");
            (*position)++;
        }
        
        //fprintf(stderr, "<%c>", **position);
    }
    else if (**position == ')')
    {
        fprintf(stderr, "ex,");
        return NO_ERROR;
    }
    else
    {
        fprintf(stderr, "error\n");
    }

    return NO_ERROR;
}

TreeError SkipSpaces(char** position)
{
    while (**position == ' ')
    {
        fprintf(stderr, "<%c>", **position);
        (*position)++;
    }

    return NO_ERROR;
}


TreeError ReadObject(char* source, char** position)
{
    size_t i = 0;

    // while (**position != ' ')// && (**position != ')'))
    // {
    //     source[i] = **position;
    //     fprintf(stderr, "<%c>", **position);
    //     (*position)++;
    //     i++;
    // }
    while ((**position != '.') && (**position != ')') && (**position != '('))
    {
        source[i] = **position;
        fprintf(stderr, "<%c>", **position);
        (*position)++;
        i++;
    }
    fprintf(stderr, "[%s]", source);
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
        case LIB_IS_EMPTY:
            printf("error: libary is empty\n");
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