#include "lang.h"


int GenerateText (Prog_t *prog, const char *filename)
{
    if (prog == nullptr || filename == nullptr) return TREE_NULLPTR_ARG;
    TreeVerify (&(prog -> tree));
   
    FILE *temp = fopen (TEMPFILENAME, "w");
    if (temp == nullptr) return TREE_NULLPTR_ARG;

    Reverse (prog, temp, (prog -> tree).data.left);

    fclose (temp);

    ReverseFile (TEMPFILENAME, filename);

    return TREE_OK;
}

int ReverseFile (const char *in_name, const char *out_name)
{
    if (in_name == nullptr || out_name == nullptr) return TREE_NULLPTR_ARG;
    
    FILE *in = fopen (in_name, "r");
    if (in == nullptr) return TREE_NULLPTR_ARG;

    size_t filesize = GetSize (in);

    char *text = (char *) calloc (filesize + 1, sizeof (char));
    if (text == nullptr) return TREE_NULLPTR_ARG;

    size_t len = fread (text + 1, sizeof (char), filesize, in);
    *text = '\0';

    fclose (in);

    FILE *out = fopen (out_name, "w");
    if (out == nullptr) return TREE_NULLPTR_ARG;

    for (; len > 0; len--)
    {
        fputc (text [len], out);
    }

    fclose (out);
    free (text);
    return TREE_OK;
}

int Reverse (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    if (elem == nullptr) return COMP_OK;

    switch (TYPE)
    {
    case TYPE_FIC:
        return Reverse_fic (prog, file, elem);

    case TYPE_NUM:
        return Reverse_num (file, elem);

    case TYPE_VAR:
        return Reverse_var (prog, file, elem);
    
    case TYPE_IF:
        return Reverse_if (prog, file, elem);
        
    case TYPE_WHILE:
        return Reverse_while (prog, file, elem);

    case TYPE_OP:
        return Reverse_op (prog, file, elem);
    
    case TYPE_VARDEC:
        return Reverse_vardec (prog, file, elem);
    
    case TYPE_FUNCDEC:
        return Reverse_funcdec (prog, file, elem);
    
    case TYPE_CALL:
        return Reverse_call (prog, file, elem);
    
    case TYPE_RETURN:
        return Reverse_return (prog, file, elem);

    default:
        return COMP_ERROR;
    }
}

int Reverse_fic (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    if (L)
    {   
        Reverse (prog, file, L);
        if (LTYPE == TYPE_OP || LTYPE == TYPE_NUM || LTYPE == TYPE_VAR || LTYPE == TYPE_CALL) fprintf (file, " ,.\n");
    }
    if (R)
    {   
        Reverse (prog, file, R);
        if (RTYPE == TYPE_OP || RTYPE == TYPE_NUM || RTYPE == TYPE_VAR || RTYPE == TYPE_CALL) fprintf (file, " ,.\n");
    }
    return COMP_OK;
}

int Reverse_num (FILE *file, TreeElem_t *elem)
{   
    int num = VAL;
    if (num < 0)
    {
        fprintf (file, "{| ) ");
        num = -num;
    }

    char tern [BUFSIZE] = "";
    char buf  [BUFSIZE] = "";
    Dec_to_rev_tern (num, tern);

    int len = (int) strlen (tern);
    int need_split = 0;

    for (int index = 0; index < len; index++)
    {
        if (tern [index] == '2')
        {
            Dec_to_rev_tern (index, buf);
            if (need_split) fputc ('\'', file);
            else need_split = 1;
            fputs (buf, file);
        }
    }

    need_split = 0;
    fputc ('|', file);

    for (int index = 0; index < len; index++)
    {
        if (tern [index] == '1')
        {
            Dec_to_rev_tern (index, buf);
            if (need_split) fputc ('\'', file);
            else need_split = 1;
            fputs (buf, file);
        }
    }

    if (num < 0) fprintf (file, "}");

    return COMP_OK;
}

int Reverse_var (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    fprintf (file, "%s", (prog -> var_table [VAL]).name);
    return COMP_OK;
}

int Reverse_if (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    fprintf (file, "69\n/\n");
    Reverse (prog, file, RL);
    fprintf (file, "\\\n{");
    
    Reverse (prog, file, L);
    fprintf (file, "}\n");

    if (RR)
    {
        fprintf (file, "79\n/\n");
        Reverse (prog, file, RR);
        fprintf (file, "\\\n");
    }

    return COMP_OK;
}

int Reverse_while (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    fprintf (file, "88888888 {");

    Reverse (prog, file, L);
    fprintf (file, "}\n/");

    Reverse (prog, file, R);

    fprintf (file, "\\\n");

    return COMP_OK;
}

int Reverse_op (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    if (VAL == OP_IN) return Reverse_in (file);

    if (!R) return Reverse_onearg (prog, file, elem);
    
    return Reverse_twoarg (prog, file, elem);

    return COMP_ERROR;
}

int Reverse_onearg (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    if (VAL == OP_SIN)
    {
        fprintf (file, "%s {", REV_SIN_WORD);
        Reverse (prog, file, L);
        fprintf (file, "}");
    }

    fprintf (file, "%s {", REV_OP_WORDS [VAL]);
    Reverse (prog, file, L);
    fprintf (file, "}");

    return COMP_OK;
}

int Reverse_twoarg (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    if (LTYPE == TYPE_OP && GetOpRank (LVAL) < GetOpRank (VAL)) fprintf (file, "{");
    Reverse (prog, file, L);
    if (LTYPE == TYPE_OP && GetOpRank (LVAL) < GetOpRank (VAL)) fprintf (file, "}");
    
    fprintf (file, " %s ", REV_OP_WORDS [VAL]);

    if (RTYPE == TYPE_OP && GetOpRank (RVAL) < GetOpRank (VAL)) fprintf (file, "{");
    Reverse (prog, file, R);
    if (RTYPE == TYPE_OP && GetOpRank (RVAL) < GetOpRank (VAL)) fprintf (file, "}");

    return COMP_OK;
}

int Reverse_in (FILE *file)
{
    fprintf (file, " ? ");
    return COMP_OK;
}

int Reverse_vardec (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    fprintf (file, "~~~~%s ,.\n", (prog -> var_table [VAL]).name);

    return COMP_OK;
}

int Reverse_funcdec (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    fprintf (file, "^%s {", (prog -> func_table [VAL]).name);
    Reverse_decargs (prog, file, L);
    fprintf (file, "}\n/\n");
    Reverse (prog, file, R);
    fprintf (file, "\\\n");

    return COMP_OK;
}

int Reverse_decargs (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    if (elem == nullptr) return COMP_OK;

    if (TYPE == TYPE_FIC)
    {
        Reverse_decargs (prog, file, L);
        Reverse_decargs (prog, file, R);
    }
    else
    {
        Reverse (prog, file, elem);
        fprintf (file, " ");
    }
    return COMP_OK;
}

int Reverse_call (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    fprintf (file, "<%s {", (prog -> func_table [VAL]).name);

    int first_arg = 1;

    Reverse_callargs (prog, file, L, &first_arg);

    fprintf (file, "}");
    
    return COMP_OK;
}

int Reverse_callargs (Prog_t *prog, FILE *file, TreeElem_t *elem, int *first_arg)
{
    if (elem == nullptr) return COMP_OK;

    if (TYPE == TYPE_FIC)
    {
        Reverse_callargs (prog, file, L, first_arg);
        Reverse_callargs (prog, file, R, first_arg);
    }
    else
    {
        if (*first_arg) *first_arg = 0;
        else fprintf (file, ",.");
        Reverse (prog, file, elem);
    }
    return COMP_OK;
}

int Reverse_return (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    fprintf (file, "> {");
    Reverse (prog, file, L);
    fprintf (file, "} ,.\n");

    return COMP_OK;
}

void Dec_to_rev_tern (int num, char *buf)
{
    if (num == 0)
    {
        *buf = '0';
        *(buf + 1) = '\0';
        return;
    }

    while (num != 0)
    {
        *(buf++) = (char) (num % 3) + '0';
        num /= 3;
    }
    *buf = '\0';
}

int GetOpRank (int op)
{
    if (op == OP_ASSIGN)              return 1;
    if (op == OP_OR)                  return 2;
    if (op == OP_AND)                 return 3;
    if (op >= 8 && op <= 13)          return 4;
    if (op == OP_ADD)                 return 5;
    if (op == OP_SUB)                 return 6;
    if (op == OP_MUL || op == OP_DIV) return 7;
    else                              return 8;
}