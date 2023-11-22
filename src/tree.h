#ifndef TREElib
#define TREElib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include "color.h"
#include <ctype.h>


const size_t MAX_SIZE_TREE = 64;
const size_t MAX_SIZE_ARG  = 64;
typedef char* Elem_t;

enum Order
{
    PRE_ORDER  = 1,
    IN_ORDER   = 2,
    POST_ORDER = 3,
};

enum OPERATORS
{
    OP_ADD = 1,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

enum TreeError
{
    NO_ERROR,
    ERROR_LOOP,
    ERROR_ALLOCATION,
    ERROR_CONST,
    FILE_NOT_OPEN,
    ERROR_POSITIONING_FUNC,
    DEFINE_IS_NULL,
    LIB_IS_EMPTY,
    ELEMENT_NOT_FOUND,
    EXIT
};

enum Type
{
    NO_TYPE,
    NUM,
    OPERATOR,
    VAR,
    
};

struct Node
{
    double  value;
    Type     type;
    Node*    left;
    Node*   right;
};

struct Tree
{
    Node* root;
    size_t size;
};

TreeError ConstructorTree(Tree* tree);
void       DestructorTree(Tree* tree);

Node*   NewNode();
void DeleteNode(Node* node);


TreeError InsertValue(Tree* tree, Elem_t value);
char*      ReadPhrase(FILE* File);


void       TextDump(Tree* tree);

TreeError  PrintNode(Node* node, FILE*   To, Order order_value);
TreeError   ReadTree(Tree* tree, Node** node, FILE* From, Order order_value);



TreeError   CheckNoLoop(Tree tree);
TreeError NodeTraversal(Node* node, Node* addresses[], size_t counter);

TreeError Qsort(Node* addresses[], int first, int last);
TreeError  Swap(Node* addresses[], int left, int right);

void DumpErrors(TreeError error);

void WriteToFile(Tree* tree);



// diff

void PrintOperator(OPERATORS value, FILE* TO);



TreeError NewReadTree(Tree* tree, Node** node, char** position, Order order_value);
TreeError SkipSpaces(char** position);
TreeError ReadObject(char* source, char** position);
TreeError PasteObject(char* source, Node** node);

#endif