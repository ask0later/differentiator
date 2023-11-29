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
    NUM = 1,
    OPERATOR,
    FUNCTION,
    VAR
};

enum Operators
{
    OP_ADD = 1,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

enum Functions
{
    FUN_SIN = 1,
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

struct Var
{
    char name[MAX_SIZE_NAME];
    double             value;
    size_t         name_size;
};


const size_t SIZE_ADD  = 1;
const size_t SIZE_SUB  = 1;
const size_t SIZE_MUL  = 1;
const size_t SIZE_DIV  = 1;
const size_t SIZE_SIN  = 3;
const size_t SIZE_COS  = 3;
const size_t SIZE_POW  = 1;
const size_t SIZE_SQRT = 4;
const size_t SIZE_LN   = 2;

const Command cmds[NUM_COMMANDS] = {{"+",    SIZE_ADD,  OPERATOR, OP_ADD  , BINARY},\
                                    {"-",    SIZE_SUB,  OPERATOR, OP_SUB  , BINARY},\
                                    {"*",    SIZE_MUL,  OPERATOR, OP_MUL  , BINARY},\
                                    {"/",    SIZE_DIV,  OPERATOR, OP_DIV  , BINARY},\
                                    {"sin",  SIZE_SIN,  FUNCTION, FUN_SIN , UNARY },\
                                    {"cos",  SIZE_COS,  FUNCTION, FUN_COS , UNARY },\
                                    {"^",    SIZE_POW,  FUNCTION, FUN_POW , BINARY},\
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


Node* CreateNode(Type type, void* value, Node* left, Node* right);

Node* CreateVariable(char* value, Node* left, Node* right);
Node* CreateNumber(double value, Node* left, Node* right);
Node* CreateOperator(Operators value, Node* left, Node* right);
Node* CreateFunction(Functions value, Node* left, Node* right);

void DeleteNode(Node* node);

TreeError  PrintNode(Node* node, FILE*   To, Order order_value);
void PrintObject(Node* node, FILE* To);
void PrintOperator(Operators value_Operators, FILE* TO);
void PrintFunction(Functions value_Functions, FILE* To);

TreeError LatexPrintNode(Node* node, FILE* To);
void LatexPrintObject(Node* node, FILE* To);
void LatexPrintOperator(Operators value_Operators, FILE* To);


TreeError ReadTree(Tree* tree, Node** node, char** position, Order order_value, Var* names, Text buffer);
TreeError SkipSpaces(char** position);
TreeError ReadObject(char* source, char** position);
TreeError PasteObject(Tree* tree, char* source, Node** node, Var* names);



TreeError   CheckNoLoop(Tree tree);
TreeError NodeTraversal(Node* node, Node* addresses[], size_t counter);

TreeError Qsort(Node* addresses[], int first, int last);
TreeError  Swap(Node* addresses[], int left, int right);

void DumpErrors(TreeError error);


#endif