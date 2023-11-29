#include "tree.h"
#include "readFile.h"
#include "dif.h"
#include "graphic.h"


int main()
{
    Text buffer = {};
    Tree tree   = {};
    Tree tree_dif = {};

    ConstructorTree(&tree);
    ConstructorTree(&tree_dif);

    Var names[MAX_NUM_VARS] = {};

    CreateBuffer(&buffer, "file.txt");

    char* position = buffer.position;

    TreeError error = ReadTree(&tree, &tree.root, &position, IN_ORDER, names, buffer);
    if (error != NO_ERROR)
    {
        DumpErrors(error);
        DestructorTree(&tree);
        return 1;
    }
    Simplification(&tree);
    // PrintNode(tree.root, stdout, IN_ORDER);

    PrintfLatex(&tree, names);

    // names[0].var_value = 0;
    // PrintNameTable(&tree, names);
    // FILE* FileFunc = fopen("function.txt", "w");
    // PrintNode(tree.root, FileFunc, IN_ORDER);
    // fclose(FileFunc);

    // FILE* TaylorFunc = fopen("taylor.txt", "w");
    // PrintMaclaurinExpansion(&tree, 3, TaylorFunc, names);
    // fclose(TaylorFunc);

    // char function1[1024] = {};
    // char function2[1024] = {};
    // ReadReadyFunctionFrom(function1, "function.txt");
    // ReadReadyFunctionFrom(function2, "taylor.txt");

    // BuildGraphic("png", "function1.png", "function1.txt", function1, function2, "[-3:3]", "[-10:10]", "Function and Taylor");
    // system("gnuplot -c function1.txt");

    // char function3[1024] = {};
    // FILE* TangetFunc = fopen("tanget.txt", "w");
    // PrintTangentEquation(&tree, TangetFunc, names);
    // fclose(TangetFunc);
    // ReadReadyFunctionFrom(function3, "tanget.txt");
    
    //BuildGraphic("png", "function2.png", "function2.txt", function1, function3, "[-3:3]", "[-10:10]", "Function and Tanget");
    //system("gnuplot -c function2.txt");
    
    //GraphicDump(&tree, NULL);

    DestructorTree(&tree);

    DeleteBuffer(&buffer);


    return 0;
}