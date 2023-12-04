#ifndef COMMANDREADlib
#define COMMANDREADlib

#include <stdio.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <assert.h>
#include <climits>


enum ReaderError
{
    ReaderErrorNO_ERROR,
    ReaderErrorALLOC_ERROR
};


struct Text
{
    size_t  size_buffer;
    char*           str;
    size_t     position;
};



ReaderError  CreateBuffer(Text* buf, const char*  input_file);
void  DeleteBuffer(Text* buf);


void ReadFile(Text* buf, const char*  input_file);
size_t Get_Size_File(const char*  input_file);

size_t Get_Num_Line(Text* buf);


#endif