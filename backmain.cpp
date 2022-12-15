#include "lang.h"

FILE *LOG = NULL;

int main ()
{
    LOG = fopen (LOGFILENAME, "w");
    if (LOG == nullptr) return 0;
    fprintf (LOG, "<pre>\n");

    Prog_t prog = {};
    ProgCtor (&prog);
    LoadProg (&prog, "save.txt");

    TreeDump (&(prog.tree));

    SaveProg (&prog, "save2.txt");

    ProgDtor (&prog);

    fclose (LOG);
    return 0;
}