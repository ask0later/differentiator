#include "readFile.h"


int GetG(Parse* parse)
{
    int value = GetE(parse);
    syntax_assert(parse->str[parse->position] == '\0', parse);
    return value;
}

int GetE(Parse* parse)
{
    int val = GetT(parse);

    while((parse->str[parse->position] == '+') || (parse->str[parse->position] == '-'))
    {
        char op = parse->str[parse->position];
        parse->position++;
        int val2 = GetT(parse);
        switch (op)
        {
            case '+': val += val2; break;
            case '-': val -= val2; break;
            default: syntax_assert(false, parse);
                break;
        }

    }
    //syntax_assert(parse->str[parse->position] != '\0', parse);
    return val;
}


int GetT(Parse* parse)
{
    int val = GetP(parse);

    while((parse->str[parse->position] == '*') || (parse->str[parse->position] == '/'))
    {
        char op = parse->str[parse->position];
        parse->position++;
        int val2 = GetP(parse);
        switch (op)
        {
            case '*': val *= val2; break;
            case '/': val /= val2; break;
            default: syntax_assert(false, parse);
                break;
        }
    }
    
    //syntax_assert(parse->str[parse->position] != '\0', parse);
    return val;
}

int GetP(Parse* parse)
{
    if (parse->str[parse->position] == '(')
    {
        int val = 0;
        parse->position++;
        val = GetE(parse);
        syntax_assert(parse->str[parse->position] == ')', parse);
        parse->position++;

        return val;
    }
        
    return GetN(parse);
}

int GetN(Parse* parse)
{
    int val = 0;
    size_t old_position = parse->position;
    while(isalnum(parse->str[parse->position]))//('0' <= parse->str[parse->position]) && (parse->str[parse->position] <= '9'))
    {
        val = val * 10 + parse->str[parse->position] - '0';
        parse->position++;
    }
    
    syntax_assert(parse->position > old_position, parse);

    return val;
}

void syntax_assert(bool x, Parse* parse)
{
    if (x == false)
    {
        printf("syntax error: %s\n", parse->str);
        printf("              ");
        for (size_t i = 0; i < parse->position; i++)
        {
            printf(" ");
        }
        printf("^\n");


        exit(1);
    }
}


ReaderError CreateBuffer(Text* buf, const char*  input_file)
{
    buf->size_buffer = Get_Size_File(input_file);
    
    buf->position = (char*) calloc(buf->size_buffer, sizeof(char));
    if (buf->position == NULL) {return ReaderErrorALLOC_ERROR;}

    ReadFile(buf, input_file);

    return ReaderErrorNO_ERROR;
}

void DeleteBuffer(Text* buf)
{
    buf->size_buffer = INT_MAX;

    free(buf->position);
}


void ReadFile(Text* buf, const char* input_file)
{
    FILE* fname = fopen(input_file, "r");

    fread(buf->position, sizeof(char), buf->size_buffer, fname);
    
    fclose(fname);
}


size_t Get_Size_File(const char* input_file)
{
    struct stat    buff;
    stat(input_file, &buff);
    
    return (size_t) (buff.st_size + 1);
}