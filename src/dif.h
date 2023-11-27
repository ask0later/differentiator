#ifndef DIFlib
#define DIFlib

#include "tree.h"
#include "graphic.h"



double Evaluate(Tree* tree, Node* node, Table* names);

Node* Differentiator(Node* node, Table name);

Node* Copynator(Node* node);

void PrintNameTable(Tree* tree, Table* names);


TreeError CollapsingConstants(Tree* tree, Node** node);
TreeError RemoveDummyElements(Tree* tree, Node** node);
TreeError Simplification(Tree* tree);

TreeError PrintfLatex(Tree* tree, Table* names);

Node* MaclaurinExpansion(Tree* tree, size_t power, Table* names);

size_t Factorial(size_t n);


void ReadReadyFunctionFrom(char* function, char filename[]);
void BuildGraphic(char Type[], char ToGnuplot[], char FromGnuplot[], char* function1, char* function2, char yrange[], char xrange[], char title[]);

void PrintTangentEquation(Tree* tree, FILE* To, Table* names);

#endif