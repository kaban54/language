#include "lang.h"


int ProgCtor (Prog_t *prog)
{
    if (prog == nullptr) return TREE_NULLPTR_ARG;
    if (prog -> var_table || prog -> func_table || prog -> code) return TREE_STATUS_ERROR;

    prog ->  var_table = (Var_t *)      calloc (BASE_TABLE_CAPACITY, sizeof (prog ->  var_table [0]));
    prog -> func_table = (Func_t *)     calloc (BASE_TABLE_CAPACITY, sizeof (prog -> func_table [0]));
    prog ->       code = (TreeElem_t *) calloc (BASE_CODE_CAPACITY , sizeof (prog ->       code [0]));

    if (prog -> var_table == nullptr || prog -> func_table == nullptr || prog -> code == nullptr)
    {
        free (prog ->  var_table);
        free (prog -> func_table);
        free (prog ->       code);
        return TREE_ALLOC_ERROR;
    }

    prog ->  var_table_capacity = BASE_TABLE_CAPACITY;
    prog -> func_table_capacity = BASE_TABLE_CAPACITY;
    prog ->       code_capacity = BASE_CODE_CAPACITY;

    prog ->  var_table_size = 0;
    prog -> func_table_size = 0;
    prog ->       code_size = 0;

    prog -> index = 0;

    TreeCtor (&(prog -> tree));

    return TREE_OK;
}

int ProgDtor (Prog_t *prog)
{
    if (prog == nullptr) return TREE_NULLPTR_ARG;

    free (prog ->  var_table);
    free (prog -> func_table);
    free (prog ->       code);

    prog ->  var_table_size = 0;
    prog -> func_table_size = 0;
    prog ->       code_size = 0;

    prog ->  var_table_capacity = 0;
    prog -> func_table_capacity = 0;
    prog ->       code_capacity = 0;

    prog -> index = 0;

    TreeDtor (&(prog -> tree));

    return TREE_OK;
}

int ProgAddVar (Prog_t *prog, const char *varname)
{
    if (prog == nullptr || varname == nullptr) return TREE_NULLPTR_ARG;
    if (strlen (varname) >= MAX_NAME_LEN) return TREE_INCORRECT_FORMAT;

    if (prog -> var_table_size >= prog -> var_table_capacity)
    {
        size_t cap = prog -> var_table_capacity;
        prog -> var_table = (Var_t *) Recalloc (prog -> var_table, cap, MAX_NAME_LEN, cap * 2);
        if (prog -> var_table == nullptr) return TREE_ALLOC_ERROR;
        prog -> var_table_capacity *= 2;
    }

    strcpy ((prog -> var_table [prog -> var_table_size]).name, varname);
    (prog -> var_table [prog -> var_table_size]).is_visible = 1;
    prog -> var_table_size += 1;

    return TREE_OK;
}

int ProgAddFunc (Prog_t *prog, const char *funcname)
{
    if (prog == nullptr || funcname == nullptr) return TREE_NULLPTR_ARG;
    if (strlen (funcname) >= MAX_NAME_LEN) return TREE_INCORRECT_FORMAT;

    if (prog -> func_table_size >= prog -> func_table_capacity)
    {
        size_t cap = prog -> func_table_capacity;
        prog -> func_table = (Func_t *) Recalloc (prog -> func_table, cap, MAX_NAME_LEN, cap * 2);
        if (prog -> func_table == nullptr) return TREE_ALLOC_ERROR;
        prog -> func_table_capacity *= 2;
    }

    strcpy ((prog -> func_table [prog -> func_table_size]).name, funcname);
    prog -> func_table_size += 1;

    return TREE_OK;
}

int GetVarIndex (Prog_t *prog, const char *name)
{
    // verify
    if (name == nullptr) return TREE_NULLPTR_ARG;

    for (int index = (int) (prog -> var_table_size - 1); index >= 0; index--)
    {
        if ((prog -> var_table [index]).is_visible && strcmp (name, (prog -> var_table [index]).name) == 0) return index;
    }

    return -1;
}

int GetFuncIndex (Prog_t *prog, const char *name)
{
    // verify
    if (name == nullptr) return TREE_NULLPTR_ARG;

    for (int index = 0; index < (int) (prog -> func_table_size); index++)
    {
        if (strcmp (name, (prog -> func_table [index]).name) == 0) return index;
    }

    return -1;
}

int ProgAddNode (Prog_t *prog, int type, int value)
{
    //verify

    if (prog -> code_size >= prog -> code_capacity)
    {
        size_t cap = prog -> code_capacity;
        prog -> code = (TreeElem_t *) Recalloc (prog -> code, cap, sizeof (prog -> code [0]), cap * 2);
        if (prog -> code == nullptr) return TREE_ALLOC_ERROR;
        prog -> code_capacity *= 2;
    }

    prog -> code [prog -> code_size].type   = type;
    prog -> code [prog -> code_size].value  = value;
    prog -> code [prog -> code_size].left   = nullptr;
    prog -> code [prog -> code_size].right  = nullptr;
    prog -> code [prog -> code_size].parent = nullptr;
    prog -> code_size += 1;

    return TREE_OK;
}


char *ReadProg (char *filename)
{
    if (filename == nullptr) return nullptr;

    FILE *file = fopen (filename, "r");
    if (file == nullptr) return nullptr;

    size_t filesize = GetSize (file);

    char *text = (char *) calloc (filesize + 2, sizeof (char));
    if (text == nullptr) return nullptr;

    size_t len = fread (text + 1, sizeof (char), filesize, file);
    *text             = '\0';
    *(text + len + 1) = '\0';

    fclose (file);

    return text;
}

size_t GetSize (FILE *inp_file)
{
    if (inp_file == NULL) return 0;
    struct stat stat_buf = {};

    fstat (fileno (inp_file), &stat_buf);
    return stat_buf.st_size;
}


int GetCode (Prog_t *prog, char *text)
{
    if (prog == nullptr || text == nullptr) return TREE_NULLPTR_ARG;
    if (*(text + 1) == '\0') return COMP_OK;

    char *ch = text + strlen (text + 1);
    int err = COMP_OK;

    Stack_t vis_stk = {};
    StackCtor (&vis_stk, BASE_CAPACITY);

    while (ch > text)
    {
        if (isspace (*ch))
        {
            ch--;
            continue;
        }

        if (*ch == '#')
        {
            err = Back_skip_comment (&ch);
            if (err) return err;
            continue;
        }

        if (*ch == '/')
        {
            err = Start_of_area (prog, &vis_stk);
            if (err) return err;
            ProgAddNode (prog, TYPE_FIC, FIC_OPENBRACE);
            ch--;
            continue;
        }

        if (*ch == '\\')
        {
            err = End_of_area (prog, &vis_stk);
            if (err) return err;
            ProgAddNode (prog, TYPE_FIC, FIC_CLOSEBRACE);
            ch--;
            continue;
        }

        if (*ch == '|' || (*ch >= '0' && *ch <= '2'))
        {
            int num = 0;
            err = Read_num (&ch, &num);
            if (err)
            {
                printf ("Syntax error: incorrect number format.\n");
                return err;
            }
            ProgAddNode (prog, TYPE_NUM, num);
            continue;
        }

        if (*ch == '8')
        {
            err = Read_word (&ch, WHILE_WORD);
            if (err)
            {
                printf ("Syntax error: incorrect while word.\n");
                return err;
            }
            ProgAddNode (prog, TYPE_WHILE, 0);
            continue;
        }

        if (*ch == '6')
        {
            err = Read_word (&ch, IF_WORD);
            if (err)
            {
                printf ("Syntax error: incorrect if word.\n");
                return err;
            }
            ProgAddNode (prog, TYPE_IF, 0);
            continue;
        }

        if (*ch == '7')
        {
            err = Read_word (&ch, ELSE_WORD);
            if (err)
            {
                printf ("Syntax error: incorrect else word.\n");
                return err;
            }
            ProgAddNode (prog, TYPE_ELSE, 0);
            continue;
        }

        if (*ch == ',')
        {
            if (*(ch - 1) == '.')
            {
                ProgAddNode (prog, TYPE_FIC, FIC_SEMICOLON);
                ch--;
            }
            else ProgAddNode (prog, TYPE_OP, OP_LESS);
            ch--;
            continue;
        }

        if (isalpha (*ch) || *ch == '_')
        {
            err = Prog_read_var (prog, &ch);
            if (err) return err;
            continue;
        }

        if (*ch == '~')
        {
            if (*(ch - 1) != '~')
            {
                ProgAddNode (prog, TYPE_OP, OP_ASSIGN);
                ch--;
                continue;
            }
            int start_of_area_index = 0;
            if (vis_stk.size > 0) start_of_area_index = vis_stk.data [vis_stk.size - 1];

            err = Read_word (&ch, VARDEC_WORD);
            if (err)
            {
                printf ("Syntax error: incorrect variable declaration word.\n");
                return err;
            }
            err = Prog_dec_var (prog, &ch, start_of_area_index, 1);
            if (err) return err;
            continue;
        }
        if (*ch == '<')
        {
            err = Prog_read_call (prog, &ch);
            if (err) return err;
            continue;
        }
        if (*ch == '^')
        {
            err = Prog_dec_func (prog, &ch, &vis_stk);
            if (err) return err;
            continue;
        }

        switch (*ch)
        {
        case '(':
            ProgAddNode (prog, TYPE_OP, OP_ADD);
            break;
        
        case ')':
            ProgAddNode (prog, TYPE_OP, OP_SUB);
            break;
        
        case '[':
            ProgAddNode (prog, TYPE_OP, OP_MUL);
            break;

        case ']':
            ProgAddNode (prog, TYPE_OP, OP_DIV);
            break;

        case '?':
            ProgAddNode (prog, TYPE_OP, OP_IN);
            break;
        
        case '!':
            ProgAddNode (prog, TYPE_OP, OP_OUT);
            break;
        
        case '"':
            ProgAddNode (prog, TYPE_OP, OP_EQ);
            break;
        
        case '.':
            ProgAddNode (prog, TYPE_OP, OP_MORE);
            break;
        
        case ':':
            ProgAddNode (prog, TYPE_OP, OP_MOREEQ);
            break;
        
        case ';':
            ProgAddNode (prog, TYPE_OP, OP_LESSEQ);
            break;
        
        case '=':
            ProgAddNode (prog, TYPE_OP, OP_NEQ);
            break;
        
        case '-':
            ProgAddNode (prog, TYPE_OP, OP_NOT);
            break;
        
        case '+':
            ProgAddNode (prog, TYPE_OP, OP_OR);
            break;
        
        case '*':
            ProgAddNode (prog, TYPE_OP, OP_AND);
            break;

        case '{':
            ProgAddNode (prog, TYPE_FIC, FIC_OPENBRACKET);
            break;
        
        case '}':
            ProgAddNode (prog, TYPE_FIC, FIC_CLOSEBRACKET);
            break;
        
        case '>':
            ProgAddNode (prog, TYPE_RETURN, 0);
            break;

        default:
            printf ("Compilation error: unknown symbol \"%c\".\n", *ch);
            return COMP_ERROR;
        }

        ch--;
    }

    if (vis_stk.size != 0)
    {
        printf ("Syntax error: missing \"\\\".\n");
        return COMP_ERROR;
    }

    ProgAddNode (prog, TYPE_FIC, 0);

    StackDtor (&vis_stk);
    return COMP_OK;
}

int Read_num (char **ch_ptr, int *num)
{
    char *ch = *ch_ptr;
    int tern = 0;
    *num = 0;

    while (*ch >= '0' && *ch <= '2')
    {
        tern = Read_ternary (&ch);
        *num += 2 * (int) pow (3, tern);
        if (*ch != '\'') break;
        ch--;
        continue;
    }
    if (*ch != '|') return COMP_ERROR;
    ch--;
    while (*ch >= '0' && *ch <= '2')
    {
        tern = Read_ternary (&ch);
        *num += 1 * (int) pow (3, tern);
        if (*ch != '\'') break;
        ch--;
        continue;
    }

    *ch_ptr = ch;
    return COMP_OK;
}

int Read_ternary (char **ch_ptr)
{
    char *ch = *ch_ptr;
    int ret   = 0;
    int power = 0;
    while (*ch >= '0' && *ch <= '2')
    {
        ret += (*ch - '0') * (int) pow (3, power++);
        ch--;
    }
    *ch_ptr = ch;
    return ret;
}

int Read_word (char **ch_ptr, const char *word)
{
    size_t len = strlen (word);

    if (BackStrncmp (*ch_ptr, word + len - 1, len)) return COMP_ERROR;

    (*ch_ptr) -= len;
    return COMP_OK;
}

int Back_skip_comment (char **ch_ptr)
{
    char *ch = *ch_ptr - 1;

    while (*ch != '#' && *ch != '\0') ch--;

    if (*ch == '\0')
    {
        printf ("Syntax error: no end of comment.\n");
        return COMP_ERROR;
    }

    *ch_ptr = ch - 1;
    return COMP_OK;
}

int Start_of_area (Prog_t *prog, Stack_t *stk)
{
    StackPush (stk, (int) (prog -> func_table_size));
    return COMP_OK;
}

int End_of_area (Prog_t *prog, Stack_t *stk)
{
    if (stk -> size == 0)
    {
        printf ("Syntax error: \"\\\" without \"/\".\n");
        return COMP_ERROR;
    }

    int start_index = 0;
    StackPop (stk, &start_index);
    for (size_t index = start_index; index < prog -> var_table_size; index++)
    {
        (prog -> var_table [index]).is_visible = 0;
    }
    return COMP_OK;
}

int Prog_dec_var (Prog_t *prog, char **ch_ptr, int start_of_area_index, int addnode)
{
    if (!(isalpha (**ch_ptr) || **ch_ptr == '_'))
    {
        printf ("Syntax error: missing variable name in variable declaration.\n");
        return COMP_ERROR;
    }
    char buf [MAX_NAME_LEN] = "";

    int err = Read_var (ch_ptr, buf, MAX_NAME_LEN);
    if (err) return err;

    int index = GetVarIndex (prog, buf);
    if (index >= start_of_area_index)
    {
        printf ("Compilation error: multiple declaration of variable (%s).\n", buf);
        return COMP_ERROR;
    }

    ProgAddVar  (prog, buf);
    if (addnode) ProgAddNode (prog, TYPE_VARDEC, (int) (prog -> var_table_size - 1));

    return COMP_OK;
}

int Prog_dec_func (Prog_t *prog, char **ch_ptr, Stack_t *stk)
{
    *ch_ptr -= 1;

    if (!(isalpha (**ch_ptr) || **ch_ptr == '_'))
    {
        printf ("Syntax error: missing function name in function declaration.\n");
        return COMP_ERROR;
    }

    char buf [MAX_NAME_LEN] = "";

    int err = Read_var (ch_ptr, buf, MAX_NAME_LEN);
    if (err) return err;

    int index = GetFuncIndex (prog, buf);

    if (index >= 0)
    {
        printf ("Compilation error: multiple declaration of function (%s).\n", buf);
        return COMP_ERROR;
    }

    ProgAddFunc (prog, buf);

    ProgAddNode (prog, TYPE_FUNCDEC, (int) (prog -> func_table_size - 1));
    Start_of_area (prog, stk);

    err = Prog_read_func_args (prog, ch_ptr, stk -> data [stk -> size - 1]);
    if (err) return err;

    return COMP_OK;
}

int Prog_read_func_args (Prog_t *prog, char **ch_ptr, int start_of_area_index)
{
    BackSkipSpaces (ch_ptr);
    char *ch = *ch_ptr;
    int err = 0;

    if (*ch != '{')
    {
        printf ("Syntax error: missing arguments in declaration of function.\n");
        return COMP_ERROR;
    }
    ch--;

    while (1)
    {
        BackSkipSpaces (&ch);
        if (*ch == '}') break;

        if (isalpha (*ch) || *ch == '_')
        {
            err = Prog_dec_var (prog, &ch, start_of_area_index, 0);
            if (err) return err;
            Func_add_arg (&(prog -> func_table [prog -> func_table_size - 1]), (int) (prog -> var_table_size - 1));
        }
        else
        {
            printf ("Syntax error: incorrect argumetns format in declaration of function.\n");
            return COMP_ERROR;
        }
    }

    ch--;
    BackSkipSpaces (&ch);
    if (*ch != '/')
    {
        printf ("Compilation error: missing body in declaration of function.\n");
        return COMP_ERROR;
    }

    ProgAddNode (prog, TYPE_FIC, FIC_OPENBRACE);

    *ch_ptr = ch - 1;
    return COMP_OK;
}

int Func_add_arg (Func_t *func, int index)
{
    func -> num_of_args += 1;
    if ((int) (func -> args_capacity) < func -> num_of_args)
    {
        size_t cap = func -> args_capacity;
        if (cap == 0)
        {
            func -> args = (int *) calloc (BASE_ARGS_CAPACITY, sizeof (func -> args [0]));
            func -> args_capacity = BASE_ARGS_CAPACITY;
        }
        else
        {
            func -> args = (int *) Recalloc (func -> args, cap * 2, sizeof (func -> args [0]), cap);
            func -> args_capacity = cap * 2;
        }
        if (func -> args == nullptr) return TREE_ALLOC_ERROR;
    }
    func -> args [func -> num_of_args - 1] = index;

    return COMP_OK;
}

int Prog_read_var (Prog_t *prog, char **ch_ptr)
{
    char buf [MAX_NAME_LEN] = "";

    int err = Read_var (ch_ptr, buf, MAX_NAME_LEN);
    if (err) return err;

    int index = GetVarIndex (prog, buf);
    if (index < 0)
    {
        printf ("Compilation error: varialbe (%s) is not declared.\n", buf);
        return COMP_ERROR;
    }
    ProgAddNode (prog, TYPE_VAR, index);

    return COMP_OK;
}

int Prog_read_call (Prog_t *prog, char **ch_ptr)
{
    *ch_ptr -= 1;

    if (!(isalpha (**ch_ptr) || **ch_ptr == '_'))
    {
        printf ("Syntax error: missing function name in call operator.\n");
        return COMP_ERROR;
    }
    char buf [MAX_NAME_LEN] = "";

    int err = Read_var (ch_ptr, buf, MAX_NAME_LEN);
    if (err) return err;

    int index = GetFuncIndex (prog, buf);
    if (index < 0)
    {
        printf ("Compilation error: function (%s) is not declared.\n", buf);
        return COMP_ERROR;
    }

    ProgAddNode (prog, TYPE_CALL, index);

    return COMP_OK;
}

int Read_var (char **ch_ptr, char *buf, size_t maxlen)
{
    char *ch = *ch_ptr;
    size_t index = 0;

    while (isalpha (*ch) || *ch == '_')
    {
        if (index >= maxlen)
        {
            printf ("Syntax error: variable name is too long.\n");
            return COMP_ERROR;
        }
        buf [index] = *ch;
        ch--;
        index++;
    }

    *ch_ptr = ch;
    return COMP_OK;
}

int BackStrncmp (const char *str1, const char *str2, size_t num)
{
    if (str1 == nullptr) return -1;
    if (str2 == nullptr) return  1;

    for (size_t index = 0; index < num; index++)
    {
        if (*(str1 - index) != *(str2 - index)) return (*(str1 - index) - *(str2 - index));
    }

    return 0;
}

void BackSkipSpaces (char **ch_ptr)
{
    char *ch = *ch_ptr;

    while (isspace (*ch)) ch--;

    *ch_ptr = ch;
}

int GetTree (Prog_t *prog)
{
    //verify

    prog -> index = 0;
    prog -> tree.data.left = GetProg (prog);
    if (prog -> tree.data.left == nullptr) return COMP_ERROR;

    prog -> tree.size = Tree_get_size (prog -> tree.data.left);

    prog -> tree.data.left -> parent = &(prog -> tree.data);

    TreeVerify (&(prog -> tree));

    return COMP_OK;
}

int SaveProg (Prog_t *prog, const char *filename)
{
    if (prog == nullptr || filename == nullptr) return TREE_NULLPTR_ARG;
    TreeVerify (&(prog -> tree));

    FILE *file = fopen (filename, "w");
    if (file == nullptr) return TREE_NULLPTR_ARG;

    Save_var_table  (prog, file);
    Save_func_table (prog, file);
    Save_tree       (prog, file);

    fclose (file);

    return TREE_OK;
}

void Save_var_table (Prog_t *prog, FILE *file)
{
    fprintf (file, "#VAR TABLE#\n");
    fprintf (file, "%lld\n", prog -> var_table_size);
    for (size_t index = 0; index < prog -> var_table_size; index++)
    {
        fprintf (file, "\t%s\n", (prog -> var_table [index]).name);
    }
    fprintf (file, "\n");
}

void Save_func_table (Prog_t *prog, FILE *file)
{
    fprintf (file, "#FUNC TABLE#\n");
    fprintf (file, "%lld\n", prog -> func_table_size);
    for (size_t index = 0; index < prog -> func_table_size; index++)
    {
        Func_t func = prog -> func_table [index];
        fprintf (file, "\t%s %d", func.name, func.num_of_args);
        for (int argnum = 0; argnum < func.num_of_args; argnum++)
        {
            fprintf (file, " %s", (prog -> var_table [func.args [argnum]]).name);
        }
        fprintf (file, "\n");
    }
    fprintf (file, "\n");
}

void Save_tree (Prog_t *prog, FILE *file)
{
    fprintf (file, "#TREE#\n");

    if ((prog -> tree).data.left) Print_node (file, (prog -> tree).data.left, 0);
}

void Print_node (FILE *file, TreeElem_t *elem, int num_of_spaces)
{
    for (int i = 0; i < num_of_spaces; i++) fputc (' ', file);

    fprintf (file, "{ %d %d ", TYPE, VAL);
    if (L)
    {
        fprintf (file, "\n");
        Print_node (file, L, num_of_spaces + 1);
    }
    if (R)
    {
        fprintf (file, "\n");
        Print_node (file, R, num_of_spaces + 1);
    }
    if (L || R)
    {
        fprintf (file, "\n");
        for (int i = 0; i < num_of_spaces; i++) fputc (' ', file);
    }
    fprintf (file, "}");
}
