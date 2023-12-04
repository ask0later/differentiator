#include "tree.h"
#include "readFile.h"
#include "dif.h"
#include "graphic.h"
#include "tree.h"


int main()
{   
    Text buffer = {};
    CreateBuffer(&buffer, "file.txt");

    // Tree tree   = {};
    
    // Parse parse = {};
    // parse.str = buffer.position;
    // parse.position = 0;
    // Node* root = GetG(&parse);
    // PrintNode(root, stdout, IN_ORDER);

    // DeleteNode(root);

    Tree tree   = {};
    Tree tree_dif = {};

    ConstructorTree(&tree);
    ConstructorTree(&tree_dif);

    Var vars[MAX_NUM_VARS] = {};

    //printf("%s\n", buffer);

    char* position = buffer.position;

    TreeError error = ReadTree(&tree, &tree.root, &position, IN_ORDER, vars, buffer);
    if (error != NO_ERROR)
    {
        DumpErrors(error);
        DestructorTree(&tree);
        return 1;
    }
    

    GraphicDump(&tree, NULL);
    PrintNode(tree.root, stdout, IN_ORDER, NOTHING);
    Simplification(&tree);
    PrintNode(tree.root, stdout, IN_ORDER, NOTHING);

    PrintNameTable(&tree, vars);

    size_t power = 3;
    size_t real_var = 0;

    if (tree.num_vars != 1)
        real_var = GetRealVar(vars); 


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


    PrintNode(tree_tay.root, stdout, IN_ORDER, NOTHING);

    LatexPrintNode(tree_tay.root, To);
    
    fprintf(To, " + o( ( x - %lg ) ^ %lu )$\\\\\n", vars[0].value, power);

    Tree tree_tanget = {};
    tree_tanget.root = GetTangetTree(&tree, vars, real_var);
    Simplification(&tree_tanget);

    fprintf(To, "Касательная в точке %s = %lg:\\\\\n", vars[real_var].name ,vars[real_var].value);
    fprintf(To, "$f(x) = ");
    LatexPrintNode(tree_tanget.root, To);
    fprintf(To, "$\\\\\n");
    //GraphicDump(&tree_tanget, NULL);

    AddGraphics(&tree, &tree_tay, &tree_tanget);

    fprintf(To, "\\includepdf{function1.pdf}\n");
    fprintf(To, "\\includepdf{function2.pdf}\n");

    LatexPrintEnding(To);

    fclose(To);

    system("pdflatex \"latex.txt\"");


    DestructorTree(&tree);
    DestructorTree(&tree_tanget);
    DestructorTree(&tree_dif);
    DestructorTree(&tree_tay);


    DeleteBuffer(&buffer);

    return 0;
}