#include "tree.h"
#include "readFile.h"
#include "dif.h"
#include "graphic.h"
#include "tree.h"


int main()
{   
    struct Text buf = {};
    struct Tree tree = {};

    Table names = {};

    CreateBuffer(&buf, "file.txt");

    Tokens tkns = {};
    ConstructorTokens(&tkns, &buf);

    CreateTokens(&tkns, &buf, &names);
    
        
    tree.num_vars = names.num_var;
    
    tree.root = GetG(&tkns);
    
    Simplification(&tree);
    GraphicDump(&tree, NULL);
    
    AssignVariables(&names);

    
    size_t real_var = 0;

    if (tree.num_vars != 1)
        real_var = GetDifferentiationVar(&names);


    FILE* To = fopen("latex.txt", "w");

    LatexPrintBeginning(To);

    LatexPrintDif(To, &tree, &names, real_var);
    LatexPrintTaylorAndTanget(To, &tree, &names, real_var);
    //GraphicDump(&tree, &tree_dif);

    LatexPrintEnding(To);

    fclose(To);

    system("pdflatex \"latex.txt\"");

    //DestructorTree(&tree);

    DestructorTokens(&tkns);

    DeleteBuffer(&buf);

    return 0;
}