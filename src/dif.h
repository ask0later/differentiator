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


//diff

double Evaluate(Tree* tree, Node* node, Var* vars);

Node* Copynator(Node* node);
Node* Differentiator(Node* node, Var vars, FILE* To, bool need_print);
Node* Taylortition(Tree* tree, size_t power, Var* vars, size_t real_var);
Node* GetTangetTree(Tree* tree, Var* vars, size_t real_var);

size_t Factorial(size_t n);

size_t GetRealVar(Var* vars);
void PrintNameTable(Tree* tree, Var* vars);


// gnuplot
void ReadFromTextToBuffer(char* function, const char* filename);
void BuildGraphic(const char* Type, const char* ToGnuplot, const char* FromGnuplot, char* function1, char* function2, const char* yrange, const char* xrange, const char* title);


// print
void LatexPrintBeginning(FILE* To);
void LatexPrintDiff(FILE* To);
void LatexPrintEnding(FILE* To);


void PrintSolutionDiff(Node* node, FILE* To, Var vars);


void       LatexPrintAdd(Node* node, FILE* To);
void       LatexPrintSub(Node* node, FILE* To);
void       LatexPrintMul(Node* node, FILE* To);
void       LatexPrintDiv(Node* node, FILE* To);
void LatexPrintExpandPow(Node* node, FILE* To);
void       LatexPrintExp(Node* node, FILE* To);
void       LatexPrintPow(Node* node, FILE* To);
void       LatexPrintCos(Node* node, FILE* To);
void       LatexPrintSin(Node* node, FILE* To);
void      LatexPrintSqrt(Node* node, FILE* To);
void        LatexPrintLn(Node* node, FILE* To);



// simplification
TreeError Simplification(Tree* tree);

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

bool isEqual(double arg1, double arg2);


#endif