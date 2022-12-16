#include "lang.h"

FILE *LOG = NULL;

int main (int argc, char *argv [])
{
    LOG = fopen (LOGFILENAME, "w");
    if (LOG == nullptr) return 0;
    fprintf (LOG, "<pre>\n");

    const char * input_file_name = nullptr;
    const char *output_file_name = nullptr;

    if (argc >= 2)  input_file_name = argv [1];
    else            input_file_name = "testprog.txt";

    if (argc >= 3) output_file_name = argv [2];
    else           output_file_name = "save.txt";

    char *text = ReadProg (input_file_name);
    Prog_t prog = {};
    ProgCtor (&prog);    

    GetCode (&prog, text);
    GetTree (&prog);
    TreeDump (&(prog.tree));

    SaveProg (&prog, output_file_name);

    free (text);
    ProgDtor (&prog);

    fclose (LOG);
    return 0;
}