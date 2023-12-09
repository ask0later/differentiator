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
const size_t NUM_MATH_COMMANDS  = 11;
const size_t MAX_NUM_TOKENS = 25;
const size_t NUM_COMMANDS_T = 3;
const size_t NUM_COMMANDS_U = 5;
const size_t MAX_NUM_VARS  = 10;

typedef char* Elem_t;


enum for_what
{
    NOTHING,
    GNUPLOT,
    LATEX
};
enum Order
{
    PRE_ORDER  = 1,
    IN_ORDER   = 2,
    POST_ORDER = 3,
};

enum TreeError
{
    NO_ERROR,
    LOOP_ERROR,
    ALLOC_ERROR,
    ERROR_CONST,
    FILE_NOT_OPEN,
    ERROR_POSITIONING_FUNC,
    DEFINE_IS_NULL,
    ELEMENT_NOT_FOUND,
    READER_ERROR
};

enum UnaryorBinary
{
    UNARY  = 1,
    BINARY = 2
};

enum Type
{
    NUM = 1,
    OPERATOR,
    VAR
};

enum Operators
{
    OP_ADD = 1,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    FUN_SIN,
    FUN_COS,
    FUN_POW,
    FUN_SQRT,
    FUN_LN,
    OP_UN_SUB,
    L_BRACKET,
    R_BRACKET,
    END
};

union tag_data
{
    double         value;
    char*       variable;
    Operators   value_op;
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
    size_t num_vars;
    size_t changes_num;
};

struct Tokens
{
    Node**    tokens;
    size_t  position;
    size_t      size;
};

struct Command
{
    char name[MAX_SIZE_NAME];
    size_t   size_name;
    Type          type;
    Operators    value;
    UnaryorBinary num_args;
};

struct Var
{
    char name[MAX_SIZE_NAME];
    double             value;
    size_t         name_size;
};

struct Table
{
    Var vars[MAX_NUM_VARS];
    size_t num_var;
};

const size_t SIZE_BRACKET  = 1;
const size_t SIZE_ADD  = 1;
const size_t SIZE_SUB  = 1;
const size_t SIZE_MUL  = 1;
const size_t SIZE_DIV  = 1;
const size_t SIZE_SIN  = 3;
const size_t SIZE_COS  = 3;
const size_t SIZE_POW  = 1;
const size_t SIZE_SQRT = 4;
const size_t SIZE_LN   = 2;

const Command math_cmds[NUM_MATH_COMMANDS] = {{"(",    SIZE_BRACKET,  OPERATOR, L_BRACKET,   UNARY},\
                                              {")",    SIZE_BRACKET,  OPERATOR, R_BRACKET, UNARY},
                                              {"+",    SIZE_ADD,  OPERATOR, OP_ADD  , BINARY},\
                                              {"-",    SIZE_SUB,  OPERATOR, OP_SUB  , BINARY},\
                                              {"*",    SIZE_MUL,  OPERATOR, OP_MUL  , BINARY},\
                                              {"/",    SIZE_DIV,  OPERATOR, OP_DIV  , BINARY},\
                                              {"sin",  SIZE_SIN,  OPERATOR, FUN_SIN , UNARY },\
                                              {"cos",  SIZE_COS,  OPERATOR, FUN_COS , UNARY },\
                                              {"^",    SIZE_POW,  OPERATOR, FUN_POW , BINARY},\
                                              {"sqrt", SIZE_SQRT, OPERATOR, FUN_SQRT, UNARY },\
                                              {"ln",   SIZE_LN,   OPERATOR, FUN_LN  , UNARY } };

const Command cmdsT[NUM_COMMANDS_T] = {{"*",    SIZE_MUL,  OPERATOR, OP_MUL  , BINARY},\
                                       {"/",    SIZE_DIV,  OPERATOR, OP_DIV  , BINARY},
                                       {"^",    SIZE_POW,  OPERATOR, FUN_POW , BINARY}};

const Command cmdsU[NUM_COMMANDS_U] = {{"sin",  SIZE_SIN,  OPERATOR, FUN_SIN , UNARY },\
                                       {"cos",  SIZE_COS,  OPERATOR, FUN_COS , UNARY },\
                                       {"sqrt", SIZE_SQRT, OPERATOR, FUN_SQRT, UNARY },\
                                       {"ln",   SIZE_LN,   OPERATOR, FUN_LN  , UNARY },\
                                       {"-",    SIZE_SUB,  OPERATOR, OP_UN_SUB, UNARY }};


TreeError ConstructorTree(Tree* tree);
void       DestructorTree(Tree* tree);


Node* CreateNode(Type type, void* value, Node* left, Node* right);

Node* CreateVariable(char* value, Node* left, Node* right);
Node* CreateNumber(double value, Node* left, Node* right);
Node* CreateOperator(Operators value, Node* left, Node* right);
void DeleteNode(Node* node);
void DeleteToken(Node* node);

TreeError ConstructorTokens(Tokens* tkns, Text* buf);
TreeError DestructorTokens(Tokens* tkns);

TreeError CreateTokens(Tokens* tkns, Text* buf, Table* names);
TreeError DeleteTokens(Tokens* tkns, Text* buf);

void SkipSpaces(Text* buf);

TreeError         ParseNumber(Tokens* tkns, Text* buf);
TreeError       ParseVariable(Tokens* tkns, Text* buf, Table* names);
TreeError  ParseMathOperators(Tokens* tkns, Text* buf);


Node* GetG(Tokens* tkns);
Node* GetExpression(Tokens* tkns);
Node* GetTerm(Tokens* tkns);
Node* GetUnary(Tokens* tkns);
Node* GetPrimaryExpression(Tokens* tkns);
Node* GetC(Tokens* tkns);
Node* GetN(Tokens* tkns);



TreeError ReadTree(Tree* tree, Node** node, char** position, Order order_value, Var* names, Text buffer);
TreeError  PrintNode(Node* node, FILE* To, Order order_value, for_what for_what);
void PrintObject(Node* node, FILE* To, for_what for_what);
void PrintOperator(Operators value_Operators, FILE* To, for_what for_what);

TreeError LatexPrintNode(Node* node, FILE* To);


TreeError SkipSpaces(char** position);
TreeError ReadObject(char* source, char** position);
TreeError PasteObject(Tree* tree, char* source, Node** node, Var* names);



TreeError   CheckNoLoop(Tree tree);
TreeError NodeTraversal(Node* node, Node* addresses[], size_t counter);

TreeError Qsort(Node* addresses[], int first, int last);
TreeError  Swap(Node* addresses[], int left, int right);

void DumpErrors(TreeError error);
void DumpTokens(Tokens* tkns);

///////
void AddGraphics(Tree* tree1, Tree* tree2, Tree* tree3);

#endif