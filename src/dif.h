#ifndef DIFlib
#define DIFlib

#include "tree.h"



double Evaluate(Tree* tree, Node* node, Table* names);

Node* Differentiator(Node* node, Table name);

Node* Copynator(Node* node);

void PrintNameTable(Tree* tree, Table* names);








#endif