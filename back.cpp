#include "lang.h"

int LoadProg (Prog_t *prog, const char *filename)
{
    if (prog == nullptr || filename == nullptr) return TREE_NULLPTR_ARG;
    TreeVerify (&(prog -> tree));

    FILE *file = fopen (filename, "r");
    if (file == nullptr) return TREE_NULLPTR_ARG;

    size_t filesize = GetSize (file);

    char *text = (char *) calloc (filesize + 1, sizeof (char));
    if (text == nullptr) return TREE_NULLPTR_ARG;

    size_t len = fread (text, sizeof (char), filesize, file);
    *(text + len) = '\0';

    char *ch = text;

    ch = Load_var_table  (prog, ch);
    ch = Load_func_table (prog, ch);
    ch = Load_tree       (prog, ch);

    fclose (file);
    free   (text);

    TreeVerify (&(prog -> tree));
    return TREE_OK;
}

char *Load_var_table (Prog_t *prog, char *ch)
{
    ch = SkipSpacesAndComments (ch);

    int symbs_read  = 0;
    int num_of_vars = 0;

    sscanf (ch, "%d%n", &num_of_vars, &symbs_read);
    ch += symbs_read;
    char varname [MAX_NAME_LEN] = "";
    for (; num_of_vars > 0; num_of_vars--)
    {
        ch = SkipSpacesAndComments (ch);
        sscanf (ch, "%s%n", varname, &symbs_read);
        ch += symbs_read;

        ProgAddVar (prog, varname);
    }
    return ch;
}

char *Load_func_table (Prog_t *prog, char *ch)
{
    ch = SkipSpacesAndComments (ch);

    int symbs_read  = 0;
    int num_of_funcs = 0;

    sscanf (ch, "%d%n", &num_of_funcs, &symbs_read);
    ch += symbs_read;
    char funcname [MAX_NAME_LEN] = "";
    char  varname [MAX_NAME_LEN] = "";
    int num_of_args = 0;

    for (; num_of_funcs > 0; num_of_funcs--)
    {
        ch = SkipSpacesAndComments (ch);
        sscanf (ch, "%s%n", funcname, &symbs_read);
        ch += symbs_read;
        ch = SkipSpacesAndComments (ch);
        sscanf (ch, "%d%n", &num_of_args, &symbs_read);
        ch += symbs_read;

        ProgAddFunc (prog, funcname);
        Func_t *func = &(prog -> func_table [prog -> func_table_size - 1]);

        for (;num_of_args > 0; num_of_args--)
        {
            ch = SkipSpacesAndComments (ch);
            sscanf (ch, "%s%n", varname, &symbs_read);
            ch += symbs_read;

            Func_add_arg (func, GetVarIndex (prog, varname));
        }
    }
    return ch;
}

char *Load_tree  (Prog_t *prog, char *ch)
{
    (prog -> tree).data.left = Read_node (&ch);
    (prog -> tree).data.left -> parent = &((prog -> tree).data);
    (prog -> tree).size = Tree_get_size ((prog -> tree).data.left);
    return ch;
}

TreeElem_t *Read_node (char **ch_ptr)
{
    char *ch = *ch_ptr;
    ch = SkipSpacesAndComments (ch);
    if (*ch != '{') return nullptr;
    ch++;
    ch = SkipSpacesAndComments (ch);

    int type       = 0;
    int value      = 0;
    int symbs_read = 0;

    sscanf (ch, "%d%n", &type, &symbs_read);
    ch += symbs_read;
    ch = SkipSpacesAndComments (ch);

    sscanf (ch, "%d%n", &value, &symbs_read);
    ch += symbs_read;

    TreeElem_t *elem = CreateNode (type, value, NULL, NULL);
    L = Read_node (&ch);
    R = Read_node (&ch);

    if (L) LP = elem;
    if (R) RP = elem;

    ch = SkipSpacesAndComments (ch);
    if (*ch != '}')
    {
        Tree_free_data (elem, NULL);
        return nullptr;
    }
    *ch_ptr = ch + 1;
    return elem;
}

char *SkipSpacesAndComments (char *ch)
{
    char *old_ch = nullptr;
    while (old_ch != ch)
    {
        old_ch = ch;
        ch = SkipSpaces  (ch);
        ch = SkipComment (ch);
    }
    return ch;
}

char *SkipSpaces (char *ch)
{
    if (ch == nullptr) return nullptr;
    while (isspace (*ch)) ch++;
    return ch;
}

char *SkipComment (char *ch)
{
    if (ch == nullptr) return nullptr;
    if (*ch != '[') return ch;
    ch++;

    while (*ch != ']' && *ch != '\0') ch++;
    if (*ch == ']') return ch + 1;
    return ch;
}


int GenerateAsm (Prog_t *prog, const char *filename)
{
    Get_var_indexes (prog);
    TreeDump (&(prog -> tree));
    
    FILE *file = fopen (filename, "w");
    if (file == nullptr) return TREE_NULLPTR_ARG;

    Compile_prog (prog, file);

    fclose (file);

    return TREE_OK;
}

int Compile_prog (Prog_t *prog, FILE *file)
{
    fprintf (file, "#ACCURACY 1000\n");
    fprintf (file, "PUSH 0\n");
    fprintf (file, "POP rdx\n");
    fprintf (file, "PUSH %d\n", prog -> vars_in_main);
    fprintf (file, "POP rcx\n");
    fprintf (file, "JMP main\n");

    if ((prog -> tree).data.left) Compile (prog, file, (prog -> tree).data.left);

    return COMP_OK;
}

int Get_var_indexes (Prog_t *prog)
{
    TreeElem_t *elem = (prog -> tree).data.left;

    int count = 0;
    size_t func_count = 0;

    while (elem)
    {
        if (!L || !IsVardec (*L)) break;
        count++;
        elem = R;
    }

    while (elem)
    {
        if (func_count >= prog -> func_table_size) break;

        if (L && IsFuncdec (*L))
        {
            func_count ++;
            Count_func_vars (prog, L);
        }

        elem = R;
    }

    VAL = FIC_START;

    while (elem)
    {
        if (L) Count_vars (prog , L, &count, 1);
        elem = R;
    }

    prog -> vars_in_main = count;

    return COMP_OK;
}

void Count_func_vars (Prog_t *prog, TreeElem_t *func)
{
    if (!IsFuncdec (*func)) return;

    int count = 0;

    TreeElem_t *elem = func -> left;
    while (elem)
    {
        if (R) (prog -> var_table [R -> value]).index_in_func = ++count;
        elem = L;
    }

    Count_vars (prog, func -> right, &count, 0);

    (prog -> func_table [func -> value]).num_of_vars = count;

    return;
}

void Count_vars (Prog_t *prog, TreeElem_t *elem, int *count, int is_main)
{
    if (elem == nullptr) return;

    if (is_main && IsReturn (*elem)) 
    {
        TYPE = TYPE_HLT;
        return;
    }

    if (IsVardec (*elem))
    {
        if (is_main) (prog -> var_table [elem -> value]).index_in_func = -(++(*count));
        else         (prog -> var_table [elem -> value]).index_in_func =   ++(*count) ;
    }
    if (L) Count_vars (prog, L, count, is_main);
    if (R) Count_vars (prog, R, count, is_main);
}


int Compile (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    if (elem == nullptr) return COMP_OK;

    switch (TYPE)
    {
    case TYPE_FIC:
        return Compile_fic (prog, file, elem);

    case TYPE_NUM:
        return Compile_num (file, elem);

    case TYPE_VAR:
        return Compile_var (prog, file, elem);
    
    case TYPE_IF:
        return Compile_if (prog, file, elem);
        
    case TYPE_WHILE:
        return Compile_while (prog, file, elem);

    case TYPE_OP:
        return Compile_op (prog, file, elem);
    
    case TYPE_VARDEC:
        return Compile_vardec (prog, file, elem);
    
    case TYPE_FUNCDEC:
        return Compile_funcdec (prog, file, elem);
    
    case TYPE_CALL:
        return Compile_call (prog, file, elem);
    
    case TYPE_RETURN:
        return Compile_return (prog, file, elem);

    case TYPE_HLT:
        return Compile_hlt (file);

    default:
        return COMP_ERROR;
    }
}

int Compile_fic (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    if (VAL == FIC_START) fprintf (file, "main:\n");
    if (L)
    {   
        Compile (prog, file, L);
        if (LTYPE == TYPE_OP || LTYPE == TYPE_NUM || LTYPE == TYPE_VAR || LTYPE == TYPE_CALL) fprintf (file, "POP rax\n");
    }
    if (R) Compile (prog, file, R);
    return COMP_OK;;
}

int Compile_num (FILE *file, TreeElem_t *elem)
{   
    fprintf (file, "PUSH %d\n", VAL);
    return COMP_OK;
}

int Compile_var (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    int index_in_func = (prog -> var_table [VAL]).index_in_func;

    if (index_in_func > 0) fprintf (file, "PUSH [rdx + %d]\n",  index_in_func);
    else                   fprintf (file, "PUSH [%d]\n"      , -index_in_func);
    return COMP_OK;
}

int Compile_if (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    Compile (prog, file, L);

    int label1 = prog -> label;
    int label2 = label1 + 1;

    prog -> label += 2;

    fprintf (file, "PUSH 0\n"
                   "JE l%d\n", label1);
    
    Compile (prog, file, RL);

    fprintf (file, "JMP l%d\n"
                   "l%d:\n", label2, label1);
    
    if (RR) Compile (prog, file, RR);

    fprintf (file, "l%d:\n", label2);

    return COMP_OK;
}

int Compile_while (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    int label1 = prog -> label;
    int label2 = label1 + 1;

    prog -> label += 2;

    fprintf (file, "l%d:\n", label1);

    Compile (prog, file, L);

    fprintf (file, "PUSH 0\n"
                   "JE l%d\n", label2);
    
    Compile (prog, file, R);

    fprintf (file, "JMP l%d\n"
                   "l%d:\n", label1, label2);

    return COMP_OK;
}

int Compile_op (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    if (IsAssign (*elem)) return Compile_assign (prog, file, elem);
    if (VAL == OP_IN)     return Compile_in     (file);

    Compile (prog, file, L);

    if (VAL == OP_SQRT) return Compile_sqrt (file);
    if (VAL == OP_OUT)  return Compile_out  (file);
    if (VAL == OP_NOT)  return Compile_not  (prog, file);

    Compile (prog, file, R);

    if (IsArithm (*elem)) return Compile_arithm (file ,elem);
    if (IsComp   (*elem)) return Compile_comp   (prog, file, elem);
    if (IsLogic  (*elem)) return Compile_logic  (prog, file, elem);

    return COMP_ERROR;
}

int Compile_assign (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    Compile (prog, file, R);
    fprintf (file, "POP rax\n"
                   "PUSH rax\n"
                   "PUSH rax\n");

    int index_in_func = (prog -> var_table [LVAL]).index_in_func;

    if (index_in_func > 0) fprintf (file, "POP [rdx + %d]\n",  index_in_func);
    else                   fprintf (file, "POP [%d]      \n", -index_in_func);
    return COMP_OK;
}

int Compile_sqrt (FILE *file)
{
    fprintf (file, "SQRT\n");
    return COMP_OK;
}

int Compile_in (FILE *file)
{
    fprintf (file, "IN\n");
    return COMP_OK;
}

int Compile_out (FILE *file)
{
    fprintf (file, "POP rax\n"
                   "PUSH rax\n"
                   "PUSH rax\n"
                   "OUT\n");
    return COMP_OK;
}

int Compile_not (Prog_t *prog, FILE *file)
{
    fprintf (file, "PUSH 0\n"
                   "JE l%d\n"
                   "PUSH 0\n"
                   "JMP l%d\n"
                   "l%d:\n"
                   "PUSH 1\n"
                   "l%d:\n", prog -> label, prog -> label + 1, prog -> label, prog -> label + 1);
    prog -> label += 2;
    return COMP_OK;
}

int Compile_arithm (FILE *file, TreeElem_t *elem)
{
    switch (VAL)
    {
    case OP_ADD:
        fprintf (file, "ADD\n");
        break;
    
    case OP_SUB:
        fprintf (file, "SUB\n");
        break;
    
    case OP_MUL:
        fprintf (file, "MUL\n");
        break;

    case OP_DIV:
        fprintf (file, "DIV\n");
        break;
    
    default:
        break;
    }
    return COMP_OK;
}

int Compile_comp (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    char jump [4] = "";

    switch (VAL)
    {
    case OP_EQ:
        strcpy (jump, "JE");
        break;
    
    case OP_MORE:
        strcpy (jump, "JA");
        break;
    
    case OP_MOREEQ:
        strcpy (jump, "JAE");
        break;
    
    case OP_LESS:
        strcpy (jump, "JB");
        break;
    
    case OP_LESSEQ:
        strcpy (jump, "JBE");
        break;
    
    case OP_NEQ:
        strcpy (jump, "JNE");
        break;

    default:
        break;
    }

    fprintf (file, "%s l%d\n"
                   "PUSH 0\n"
                   "JMP l%d\n"
                   "l%d:\n"
                   "PUSH 1\n"
                   "l%d:\n", jump, prog -> label, prog -> label + 1, prog -> label, prog -> label + 1);
    prog -> label += 2;

    return COMP_OK;
}


int Compile_logic (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    char jump [4] = "JNE";
    int num = 0;
    
    if (VAL == OP_AND)
    {
        strcpy (jump, "JE");
        num = 1;
    }

    fprintf (file, "POP rax\n"
                   "PUSH 0\n"
                   "%s l%d\n"
                   "PUSH 0\n"
                   "PUSH rax\n"
                   "%s l%d\n"
                   "PUSH %d\n"
                   "JMP l%d\n"
                   "l%d:\n"
                   "PUSH %d\n"
                   "l%d:", jump, prog -> label, jump, prog -> label, num, prog -> label + 1, prog -> label, !num, prog -> label + 1);
    prog -> label += 2;

    return COMP_OK;
}

int Compile_vardec (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    fprintf (file, "PUSH 0\n");

    int index_in_func = (prog -> var_table [VAL]).index_in_func;

    if (index_in_func > 0) fprintf (file, "POP [rdx + %d]\n",  index_in_func);
    else                   fprintf (file, "POP [%d]      \n", -index_in_func);

    return COMP_OK;
}

int Compile_funcdec (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    fprintf (file, "f%d:\n", VAL);

    fprintf (file, "PUSH %d\n"
                   "POP rcx\n", (prog -> func_table [VAL]).num_of_vars);

    for (int index = 1; index <= (prog -> func_table [VAL]).num_of_args; index++)
    {
        fprintf (file, "POP [rdx + %d]\n", index);
    }
    Compile (prog, file, R);

    return COMP_OK;
}

int Compile_call (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    fprintf (file, "PUSH rcx\n");
    Compile (prog, file, L);
    fprintf (file, "PUSH rdx\n"
                   "PUSH 1\n"
                   "PUSH rcx\n"
                   "ADD\n"
                   "ADD\n"
                   "POP rdx\n"
                   "CALL f%d\n"
                   "POP rax\n"
                   "POP rcx\n"
                   "PUSH rdx\n"
                   "PUSH rcx\n"
                   "PUSH 1\n"
                   "ADD\n"
                   "SUB\n"
                   "POP rdx\n"
                   "PUSH rax\n", VAL);
    return COMP_OK;
}

int Compile_return (Prog_t *prog, FILE *file, TreeElem_t *elem)
{
    Compile (prog, file, L);
    fprintf (file, "RET\n");

    return COMP_OK;
}

int Compile_hlt (FILE *file)
{
    fprintf (file, "HLT\n");
    return COMP_OK;
}