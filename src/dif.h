#ifndef DIFlib
#define DIFlib

#include "tree.h"
#include "graphic.h"
#include "time.h"

const int NUM_PHRASES = 15;

const char* const funny_phrases[NUM_PHRASES] = {"Я лучше промолчу:",
                                          "Поняли? Не поняли? Ну жалко, мне вас жаль.",
                                          "По-моему, такую производную я брал еще в садике.",
                                          "Ни на что не намекаю, но если это не понятно, то... ну на сессии увидимся.",
                                          "Заметим самую очевидную вещь.",
                                          "Я этот момент хотел сделать в уме, но вспомнил кому я вычисляю.",
                                          "Элементарное вычисление дает нам.",
                                          "Не хочу объяснять как это получилось... Заметим, что.",
                                          "Если ты в этом допустил ошибку, ноль очков тебе.",
                                          "Очевидно, что.",
                                          "Даже младенец поймет этот ход решения.",
                                          "Получается, в числителе что получается, а так и получается.",
                                          "Заметим, что.",
                                          "Путем нехитрых преобразований.",
                                          "Легко видеть, что"
                                          };



double Evaluate(Tree* tree, Node* node, Var* names);

Node* Differentiator(Node* node, Var var, FILE* To, bool need_print);

Node* Copynator(Node* node);

void PrintNameTable(Tree* tree, Var* names);


TreeError CollapsingConstants(Tree* tree, Node** node);
TreeError RemoveDummyElements(Tree* tree, Node** node);

bool isMul1(Node* parent, Node* kid);
bool isAdd0(Node* parent, Node* kid);
bool isSub0(Node* parent, Node* kid);
bool isPow1(Node* parent, Node* kid);
bool isDiv1(Node* parent, Node* kid);
bool isMul0(Node* parent, Node* kid);
bool isDiv0(Node* parent, Node* kid);
bool isPow0(Node* parent, Node* kid);

TreeError Simplification(Tree* tree);

Node* TaylorExpansion(Tree* tree, size_t power, Var* names, Var var);

size_t Factorial(size_t n);


void ReadReadyFunctionFrom(char* function, char filename[]);
void BuildGraphic(char Type[], char ToGnuplot[], char FromGnuplot[], char* function1, char* function2, char yrange[], char xrange[], char title[]);


TreeError PrintfLatex(Tree* tree, Var* names);
void PrintTangentEquation(Tree* tree, FILE* To, Var* names);
void PrintSolutionDiff(Node* node, FILE* To);


#endif