#include "tree.h"

const char *const IMGNUMFILE = "log/imgnum.txt";
const char *const  GRAPHFILE = "log/graph.txt";

const char *const  LEFTCOLOR  = "green";
const char *const RIGHTCOLOR  =   "red";
const char *const PARENTCOLOR = "black";


const char *const FICCOLOR     = "lightgrey";
const char *const OPCOLOR      = "lightblue";
const char *const NUMCOLOR     = "blue";
const char *const VARCOLOR     = "lightgreen";
const char *const IFCOLOR      = "lightyellow";
const char *const ELSECOLOR    = "lightyellow";
const char *const WHILECOLOR   = "lightyellow";
const char *const VARDECCOLOR  = "green";
const char *const CALLCOLOR    = "orange";
const char *const FUNCDECCOLOR = "purple";
const char *const RETURNCOLOR  = "purple";

const char *const COLORS[] = {"lightgrey", "blue", "lightgreen", "lightyellow", "lightyellow", "lightyellow",
                              "lightblue", "green", "purple", "orange", "purple"};
const char *const TYPES [] = {"FIC", "NUM", "VAR", "IF", "ELSE", "WHILE", "OP", "VARDEC", "FUNCDEC", "CALL", "RETURN"};



void Tree_txt_dmup (Tree_t *tree, FILE *stream, const char *func_name, const char *file_name, int line)
{
    if (stream == nullptr) stream = stdout;

    if (func_name == nullptr) func_name = "(NULL)";
    if (file_name == nullptr) file_name = "(NULL)";

    fprintf (stream, "\nTree dump from (%s) at (%s) at line (%d):\n", func_name, file_name, line);

    if (tree == nullptr)
    {
        fprintf (stream, "Unknown tree (nullptr).\n");
        return;
    }

    if (tree -> err & TREE_INFO_CORRUPTED) fprintf (stream, "Unknown tree (info corrupted):\n");
    else fprintf (stream, "tree [%p] \"%s\" at (%s) at (%s)(%d):\n", tree, tree -> info.     name, tree -> info.func_name, 
                                                                           tree -> info.file_name, tree -> info.     line);
    fprintf (stream, "{\n");
    fprintf (stream, "\tstatus   = %d\n", tree -> status);
    fprintf (stream, "\terror    = %d\n", tree -> err);
    fprintf (stream, "\tsize     = %d\n", tree -> size);
    fprintf (stream, "\n\t");

    if (tree -> err & TREE_DATA_CORRUPTED) fprintf (stream, "data is corrupted");
    else Tree_print_data (stream, &(tree -> data));

    fprintf (stream, "\n}\n");
}

void Tree_print_data (FILE *stream, TreeElem_t *elem)
{
    fprintf (stream, "(");
    if (L) Tree_print_data (stream, L);
    fprintf (stream, "TYPE = %d; VAL = %d", TYPE, VAL);
    if (R) Tree_print_data (stream, R);
    fprintf (stream, ")");
}

void Tree_dump (Tree_t *tree, const char *func_name, const char *file_name, int line)
{
    fprintf (LOG, "<pre>\n");
    Tree_txt_dmup (tree, LOG, func_name, file_name, line);

    if (tree != nullptr)
    {
        int imgnum = 0;
        FILE *numfile = fopen (IMGNUMFILE, "r");
        if (numfile != nullptr)
        {
            fscanf (numfile, "%d", &imgnum);
            fclose (numfile);
            numfile = fopen (IMGNUMFILE, "w");
            fprintf (numfile, "%d", imgnum + 1);
            fclose (numfile);
        }
        Tree_generate_img (tree, imgnum);
        fprintf (LOG, "<img src=\"./images/dumpimg%d.png\", width=\"80%%\">", imgnum);
    }

}


void Tree_generate_img (Tree_t *tree, int imgnum)
{
    FILE *graph = fopen (GRAPHFILE, "w");
    if (graph == nullptr) return;

    fprintf (graph, "digraph {\n rankdir = TB;\n"
                    "node [shape = record, fontsize = 12, style = \"rounded, filled\", fillcolor = white];\n"
                    "graph [splines = true];\n");
    
    int size = tree -> size;

    Tree_draw_data (graph, &(tree -> data), 0, &size, 0);

    fprintf (graph, "}");

    fclose (graph);

    char cmd [64] = "";
    sprintf (cmd, "dot -T png -o log/images/dumpimg%d.png %s", imgnum, GRAPHFILE);
    system (cmd);
}

void Tree_draw_data (FILE *graph, TreeElem_t *elem, int rank, int *size, int print_adress)
{
    if (*size < 0 || elem == nullptr) return;

    fprintf (graph, "r%d [style = invis];\n", rank);
    if (TYPE >= 0 && TYPE <= 10) fprintf (graph, "elem%p [label = \"{type = %s|value = %d", elem, TYPES [TYPE], VAL);
    else                         fprintf (graph, "elem%p [label = \"{type = %d|value = %d", elem, TYPE        , VAL);


    if (print_adress) fprintf (graph, "|{{adress|parent|left|right}|{%p|%p|%p|%p}}}\"", elem, P, L, R);
    else              fprintf (graph, "}\"");

    if (TYPE >= 0 && TYPE <= 10) fprintf (graph, ", fillcolor = %s", COLORS [TYPE]);

    if (rank == 0)
    {
        fprintf (graph, ", color = red");
    }

    fprintf (graph, "];\n");
    fprintf (graph, "{rank = same; \"r%d\"; \"elem%p\"}", rank, elem);

    *size -= 1;

    if (P)
    {
        fprintf (graph, "elem%p -> elem%p [color = %s, weight = 1];\n", elem, P, PARENTCOLOR);
    }

    if (L)
    {
        fprintf (graph, "elem%p -> elem%p [color = %s, weight = 1];\n", elem, L, LEFTCOLOR);
        Tree_draw_data (graph, L, rank + 1, size, print_adress);
    }
    if (R)
    {
        fprintf (graph, "elem%p -> elem%p [color = %s, weight = 1];\n", elem, R, RIGHTCOLOR);
        Tree_draw_data (graph, R, rank + 1, size, print_adress);
    }
}
