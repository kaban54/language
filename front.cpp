#include "lang.h"


int ProgCtor (Prog_t *prog)
{
    if (prog == nullptr) return TREE_NULLPTR_ARG;
    if (prog -> var_table || prog -> func_table || prog -> code || prog -> tree) return TREE_STATUS_ERROR;

    prog ->  var_table = calloc (BASE_TABLE_CAPACITY, sizeof (prog ->  var_table [0]));
    prog -> func_table = calloc (BASE_TABLE_CAPACITY, sizeof (prog -> func_table [0]));
    prog ->       code = calloc (BASE_CODE_CAPACITY , sizeof (prog ->       code [0]));

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
        prog -> var_table = Recalloc (prog -> var_table, cap, MAX_NAME_LEN, cap * 2);
        if (prog -> var_table == nullptr) return TREE_ALLOC_ERROR;
        prog -> var_table_capacity *= 2;
    }

    strcpy ((prog -> var_table [prog -> var_table_size]).name, varname);
    prog -> var_table_size += 1;

    return TREE_OK;
}

int GetVarIndex (Prog_t *prog, const char *name)
{
    // verify
    if (name == nullptr) return TREE_NULLPTR_ARG;

    int inedx = 0;

    while (strcmp (name, (prog -> var_table [inedx]).name) && index < prog -> var_table_size) index++;

    if (index == prog -> var_table_size) ProgAddVar (prog, name);

    return index;
}

int ProgAddNode (Prog_t *prog, int type, int value)
{
    //verify

    if (prog -> code_size >= prog -> code_capacity)
    {
        size_t cap = prog -> code_capacity;
        prog -> code = Recalloc (prog -> code, cap, sizeof (prog -> code [0]), cap * 2);
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

    char *text = (char *) calloc (filesize + 1, sizeof (char));
    if (text == nullptr) return nullptr;

    size_t len = fread (text, sizeof (char), filesize, file);
    *(text + len) = '\0';

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

    int err = COMP_OK;

    while (text != '\0')
    {
        if (isspace (*text))
        {
            text++;
            continue;
        }

        if (isdigit (*text))
        {
            ProgAddNode (prog, TYPE_NUM, GetNum (&text));
            continue;
        }

        switch (*text)
        {
        case '#':
            if (SkipComment (&text)) return COMP_COMMENT_ERROR;
            break;


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
        
        case '%':
            ProgAddNode (prog, TYPE_OP, OP_POW);
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
        
        case ',':
            ProgAddNode (prog, TYPE_OP, OP_LESS);
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
        
        case '\'':
            ProgAddNode (prog, TYPE_OP, OP_ASSIGN);
            break;


        case '~':
            ProgAddNode (prog, TYPE_VARDEC, -1);
            break;

        case '^':
            ProgAddNode (prog, TYPE_FUNCDEC, -1);
            break;
        
        case '<':
            ProgAddNode (prog, TYPE_CALL, -1);
            break;
        
        case '>':
            ProgAddNode (prog, TYPE_RETURN, 0);
            break;


        case '|':
            ProgAddNode (prog, TYPE_FIC, FIC_COMMA);
            break;
        
        case '/':
            ProgAddNode (prog, TYPE_FIC, FIC_OPENBRACE);
            break;
        
        case '\\':
            ProgAddNode (prog, TYPE_FIC, FIC_CLOSEBRACE);
            break;
        
        case '{':
            ProgAddNode (prog, TYPE_FIC, FIC_OPENBRACKET);
            break;
        
        case '}':
            ProgAddNode (prog, TYPE_FIC, FIC_CLOSEBRACKET);
            break;
        

        case '.':
            if (*(text + 1) == ',')
            {
                ProgAddNode (prog, TYPE_FIC, FIC_SEMICOLON);
                text++;
            }
            else ProgAddNode (prog, TYPE_OP, OP_MORE);
            break;

        default:
            if (!isalpha (*text) && *text != '_')
            {
                printf ("Compilation error: unknown symbol \"%c\".\n", *text);
                return COMP_UNKNOWN_SYMB;
            }

            Prog_add_name (prog, &text);
            continue;
        }

        text++;
    }
}

int GetNum (char **text)
{
    if (text == nullptr || *text == nullptr) return 0;
    char *str = *text;

    int ret = 0;
    while (isdigit (*str)) ret = ret * 10 + *(str++) - '0';
    
    *text = str;
    return ret;
}

int SkipComment (char **text)
{
    if (text == nullptr || *text == nullptr) return TREE_NULLPTR_ARG;
    char *str = *text;

    while (*str != '#' && *str != '\0') str++;

    if (*str == '\0')
    {
        printf ("Compilation error: no end of comment.\n");
        return COMP_COMMENT_ERROR;
    }

    *text = str;
    return COMP_OK;
}

int Prog_add_name (Prog_t *prog, char **text)
{
    char *str = *text;
    char ch = 0;

    if (strncmp ("text", "__", 2) == 0)
    {
        ch = *(str + 2);
        if (ch == '_')
        {
            ch = *(str + 3);
            if (ch == '_')
            {
                ch = *(str + 4);
                if (!isalnum (ch) && ch != '_')
                {
                    ProgAddNode (prog, TYPE_WHILE, 0);
                    (*text) += 4;
                    return COMP_OK;
                }
            }
            else if (!isalnum (ch))
            {
                ProgAddNode (prog, TYPE_ELSE, 0);
                (*text) += 3;
                return COMP_OK;
            }
        }
        else if (!isalnum (ch))
        {
            ProgAddNode (prog, TYPE_IF, 0);
            (*text) += 2;
            return COMP_OK;
        }
    }

    char buf [MAX_NAME_LEN] = "";
    GetName (text, buf, MAX_NAME_LEN);

    if (isalnum (*text) || *text == '_')
    {
        printf ("Compilation Error: name is too long.\n");
        return COMP_NAME_LONG;
    }

    int prev_type = (prog -> code [prog -> code_size - 1]).type;

    if (prev_type == TYPE_FUNCDEC || prev_type == TYPE_CALL)
    {
        // to do
        (prog -> code [prog -> code_size - 1]).value = -1;
    }
    else if (prev_type == TYPE_VARDEC) (prog -> code [prog -> code_size - 1]).value = GetVarIndex (prog, buf);
    else ProgAddNode (prog, TYPE_VAR, GetVarIndex (prog, buf));

    return COMP_OK;
}

void GetName (char **text, char *buf, size_t maxlen)
{
    if (buf == nullptr || text == nullptr || *text == nullptr) return;

    char *str = *text;
    if (!isalpha (*str) && *str != '_') return;

    size_t index = 0;
    for (; index < maxlen - 1; index++)
    {
        if (!isalnum(str [index]) && str [index] != '_') break;
        buf [index] = str [index];
    }

    buf [index] = '\0';
    *text = str + index;
}

void *Recalloc (void *memptr, size_t num, size_t size, size_t old_num)
{
    memptr = realloc (memptr, num * size);
    if (memptr == NULL) return NULL;

    if (num > old_num) memset ((void *) ((char *) memptr + old_num * size), 0, (num - old_num) * size);

    return memptr;
}



int GetTree (Prog_t *prog)
{
    //verify

    size_t index = 0;
    TreeElem_t *elem = 
}