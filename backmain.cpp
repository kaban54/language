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
    else            input_file_name = "save.txt";

    if (argc >= 3) output_file_name = argv [2];
    else           output_file_name = "asm.txt";

    Prog_t prog = {};
    ProgCtor (&prog);
    LoadProg (&prog, input_file_name);

    TreeDump (&(prog.tree));

    GenerateAsm (&prog, output_file_name);

    ProgDtor (&prog);

    fclose (LOG);
    return 0;
}