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
#include "readFile.h"


const size_t MAX_SIZE_TREE = 64;
const size_t MAX_SIZE_ARG  = 64;
const size_t MAX_SIZE_NAME = 10;
const size_t NUM_COMMANDS  =  9;
const size_t MAX_NUM_VARS  = 10;

typedef char* Elem_t;

enum UnaryorBinary
{
    UNARY  = 1,
    BINARY = 2
};

enum Type
{
    NO_TYPE,
    NUM,
    OPERATOR,
    FUNCTION,
    VAR
};

enum Operators
{
    NO_OP,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

enum Functions
{
    NO_FUN,
    FUN_SIN,
    FUN_COS,
    FUN_POW,
    FUN_SQRT,
    FUN_LN
};

struct Command
{
    char name[MAX_SIZE_NAME];
    size_t   size_name;
    Type          type;
    int          value;
    UnaryorBinary num_args;
};

struct Table
{
    char var_name[MAX_SIZE_NAME];
    double             var_value;
    size_t             name_size;
};


const size_t SIZE_ADD  = 3;
const size_t SIZE_SUB  = 3;
const size_t SIZE_MUL  = 3;
const size_t SIZE_DIV  = 3;
const size_t SIZE_SIN  = 3;
const size_t SIZE_COS  = 3;
const size_t SIZE_POW  = 3;
const size_t SIZE_SQRT = 4;
const size_t SIZE_LN   = 2;

const Command cmds[NUM_COMMANDS] = {{"add",  SIZE_ADD,  OPERATOR, OP_ADD  , BINARY},\
                                    {"sub",  SIZE_SUB,  OPERATOR, OP_SUB  , BINARY},\
                                    {"mul",  SIZE_MUL,  OPERATOR, OP_MUL  , BINARY},\
                                    {"div",  SIZE_DIV,  OPERATOR, OP_DIV  , BINARY},\
                                    {"sin",  SIZE_SIN,  FUNCTION, FUN_SIN , UNARY },\
                                    {"cos",  SIZE_COS,  FUNCTION, FUN_COS , UNARY },\
                                    {"pow",  SIZE_POW,  FUNCTION, FUN_POW , BINARY},\
                                    {"sqrt", SIZE_SQRT, FUNCTION, FUN_SQRT, UNARY },\
                                    {"ln",   SIZE_LN,   FUNCTION, FUN_LN  , UNARY } };

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
    READER_ERROR
};



union tag_data
{
    double         value;
    char*       variable;
    Operators   value_op;
    Functions  value_fun;
};

struct Node
{
    UnaryorBinary  num_args;
    tag_data  data;
    Type      type;
    Node*     left;
    Node*    right;
};

struct Tree
{
    Node* root;
    size_t size;
    size_t num_names;
    size_t changes_num;
};

TreeError ConstructorTree(Tree* tree);
void       DestructorTree(Tree* tree);


Node* CreateNode(Type type, double value, Node* left, Node* right);
void DeleteNode(Node* node);

TreeError  PrintNode(Node* node, FILE*   To, Order order_value);
void PrintObject(Node* node, FILE* To);
void PrintOperator(Operators value_Operators, FILE* TO);
void PrintFunction(Functions value_Functions, FILE* To);

TreeError ReadTree(Tree* tree, Node** node, char** position, Order order_value, Table* names, Text buffer);
TreeError SkipSpaces(char** position);
TreeError ReadObject(char* source, char** position);
TreeError PasteObject(Tree* tree, char* source, Node** node, Table* names);



TreeError   CheckNoLoop(Tree tree);
TreeError NodeTraversal(Node* node, Node* addresses[], size_t counter);

TreeError Qsort(Node* addresses[], int first, int last);
TreeError  Swap(Node* addresses[], int left, int right);

void DumpErrors(TreeError error);


#endif