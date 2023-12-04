#include "tree.h"
#include "readFile.h"
#include "dif.h"
#include "graphic.h"
#include "tree.h"


int main()
{   
    Text buf = {};
    CreateBuffer(&buf, "file.txt");

    Tree tree   = {};
    Tree tree_dif = {};

    ConstructorTree(&tree);
    ConstructorTree(&tree_dif);

    Var vars[MAX_NUM_VARS] = {};


    char* position = buf.str;

    TreeError error = ReadTree(&tree, &tree.root, &position, IN_ORDER, vars, buf);
    if (error != NO_ERROR)
    {
        DumpErrors(error);
        DestructorTree(&tree);
        return 1;
    }

    Simplification(&tree);
    //PrintNode(tree.root, stdout, IN_ORDER, NOTHING);

    AssignVariables(&tree, vars);

    size_t power = 3;
    size_t real_var = 0;

    if (tree.num_vars != 1)
        real_var = GetDifferentiationVar(vars);


    FILE* To = fopen("latex.txt", "w");

    LatexPrintBeginning(To);
    fprintf(To, "$f(x) = ");
    LatexPrintNode(tree.root, To);
    fprintf(To, "$\\\\\n");
    
    fprintf(To, "Производная высчитывается относительно переменной <%s> при помощи элементарых правил арифметики и очевидных преобразований\\\\\n", vars[real_var].name);

    
    tree_dif.root = Differentiate(tree.root, vars[real_var], To, true);
    Simplification(&tree_dif);

    fprintf(To, "В итоге мы получаем.\\\\\n");
    fprintf(To, "$f'(x) = ");
    LatexPrintNode(tree_dif.root, To);
    fprintf(To, "$\\\\\n");


    Tree tree_tay = {};
    fprintf(To, "Разложение по Тейлору:\\\\\n");
    fprintf(To, "$f(x) = ");
    tree_tay.root = Taylortition(&tree, power, vars, real_var);
    
    Simplification(&tree_tay);
    GraphicDump(&tree_tay, NULL);


    // LatexPrintNode(tree_tay.root, To);
    
    // fprintf(To, " + o( ( x - %lg ) ^ %lu )$\\\\\n", vars[0].value, power);

    // Tree tree_tanget = {};
    // tree_tanget.root = GetTangetTree(&tree, vars, real_var);
    // Simplification(&tree_tanget);

    // fprintf(To, "Касательная в точке %s = %lg:\\\\\n", vars[real_var].name ,vars[real_var].value);
    // fprintf(To, "$f(x) = ");
    // LatexPrintNode(tree_tanget.root, To);
    // fprintf(To, "$\\\\\n");

    // AddGraphics(&tree, &tree_tay, &tree_tanget);

    // fprintf(To, "\\includepdf{function1.pdf}\n");
    // fprintf(To, "\\includepdf{function2.pdf}\n");

    // LatexPrintEnding(To);

    // fclose(To);

    // system("pdflatex \"latex.txt\"");

    DestructorTree(&tree);
    //DestructorTree(&tree_tanget);
    DestructorTree(&tree_dif);
    DestructorTree(&tree_tay);


    DeleteBuffer(&buf);

    return 0;
}