#include "lang.h"

FILE *LOG = NULL;

int main ()
{
    LOG = fopen (LOGFILENAME, "w");
    if (LOG == nullptr) return 0;
    fprintf (LOG, "<pre>\n");

    char *text = ReadProg ("testprog.txt");
    Prog_t prog = {};
    ProgCtor (&prog);    

    GetCode (&prog, text);
    GetTree (&prog);
    TreeDump (&(prog.tree));

    free (text);
    ProgDtor (&prog);

    fclose (LOG);
    return 0;
}