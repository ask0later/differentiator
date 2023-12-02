#include "readFile.h"



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