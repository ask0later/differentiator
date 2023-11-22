#ifndef TREElib
#define TREElib

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

#include "color.h"


const size_t MAX_SIZE_TREE = 64;
const size_t MAX_SIZE_ARG  = 64;
const size_t MAX_SIZE_NAME = 5;
const size_t NUM_COMMANDS  = 4;

typedef char* Elem_t;

struct Command
{
    char name[MAX_SIZE_NAME];
    size_t size_name;
    size_t  value_op;
};

enum Operators
{
    OP_ADD = 1,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

const size_t SIZE_ADD = 3;
const size_t SIZE_SUB = 3;
const size_t SIZE_MUL = 3;
const size_t SIZE_DIV = 3;

const Command cmds[NUM_COMMANDS] = {{"add", SIZE_ADD, OP_ADD},\
                                    {"sub", SIZE_SUB, OP_SUB},\
                                    {"mul", SIZE_MUL, OP_MUL},\
                                    {"div", SIZE_DIV, OP_DIV}};

enum Order
{
    PRE_ORDER  = 1,
    IN_ORDER   = 2,
    POST_ORDER = 3,
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
    ELEMENT_NOT_FOUND,
    READER_ERROR,
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


void       TextDump(Tree* tree);

TreeError  PrintNode(Node* node, FILE*   To, Order order_value);



TreeError   CheckNoLoop(Tree tree);
TreeError NodeTraversal(Node* node, Node* addresses[], size_t counter);

TreeError Qsort(Node* addresses[], int first, int last);
TreeError  Swap(Node* addresses[], int left, int right);

void DumpErrors(TreeError error);

void WriteToFile(Tree* tree);



// diff

void PrintOperator(Operators value_Operators, FILE* TO);



TreeError ReadTree(Tree* tree, Node** node, char** position, Order order_value);

TreeError SkipSpaces(char** position);
TreeError ReadObject(char* source, char** position);
TreeError PasteObject(char* source, Node** node);

#endif