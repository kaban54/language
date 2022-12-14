#include "lang.h"

FILE *LOG = NULL;

int main ()
{
    LOG = fopen (LOGFILENAME, "w");
    if (LOG == nullptr) return 0;
    fprintf (LOG, "<pre>\n");

    fclose (LOG);
    return 0;
}