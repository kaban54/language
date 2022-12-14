#ifndef LANG_H
#define LANG_H

#include "tree/tree.h"
#include "stack/stack.h"
#include "math.h"
#include "sys/stat.h"


const size_t BASE_TABLE_CAPACITY =  8;
const size_t BASE_CODE_CAPACITY  = 32;
const size_t MAX_NAME_LEN  = 32;

const char *const  WHILE_WORD = "88888888";
const char *const     IF_WORD = "96";
const char *const   ELSE_WORD = "97";
const char *const VARDEC_WORD = "~~~~";

//DSL --------------------------------------------------------------------

#define CURRENT (prog -> code [prog -> index])

#define        IsComma(node) (node -> type == TYPE_FIC && node -> value == FIC_COMMA)
#define    IsSemicolon(node) (node -> type == TYPE_FIC && node -> value == FIC_SEMICOLON)
#define    IsOpenBrace(node) (node -> type == TYPE_FIC && node -> value == FIC_OPENBRACE)
#define   IsCloseBrace(node) (node -> type == TYPE_FIC && node -> value == FIC_CLOSEBRACE)
#define  IsOpenBracket(node) (node -> type == TYPE_FIC && node -> value == FIC_OPENBRACKET)
#define IsCloseBracket(node) (node -> type == TYPE_FIC && node -> value == FIC_CLOSEBRACKET)

#define NUM(x) CreateNode (TYPE_NUM, x, NULL, NULL)
#define VAR(i) CreateNode (TYPE_VAR, i, NULL, NULL)

#define    ADD(left, right) CreateOp (OP_ADD   , left, right)
#define    SUB(left, right) CreateOp (OP_SUB   , left, right)
#define    MUL(left, right) CreateOp (OP_MUL   , left, right)
#define    DIV(left, right) CreateOp (OP_DIV   , left, right)
#define    POW(left, right) CreateOp (OP_POW   , left, right)
#define     EQ(left, right) CreateOp (OP_EQ    , left, right)
#define   MORE(left, right) CreateOp (OP_MORE  , left, right)
#define   LESS(left, right) CreateOp (OP_LESS  , left, right)
#define MOREEQ(left, right) CreateOp (OP_MOREEQ, left, right)
#define LESSEQ(left, right) CreateOp (OP_LESSEQ, left, right)
#define    NEQ(left, right) CreateOp (OP_NEQ   , left, right)
#define     OR(left, right) CreateOp (OP_OR    , left, right)
#define    AND(left, right) CreateOp (OP_AND   , left, right)
#define ASSIGN(left, right) CreateOp (OP_ASSIGN, left, right)
#define    OUT(node)        CreateOp (OP_OUT   , node, NULL)
#define    NOT(node)        CreateOp (OP_NOT   , node, NULL)
#define     IN              CreateOp (OP_IN    , NULL, NULL)

#define FIC(left, right) CreateNode (TYPE_FIC, 0, left, right)

#define IF(cond, iftrue, iffalse) CreateNode (TYPE_IF, 0, cond, CreateNode (TYPE_ELSE, 0, iftrue, iffalse))

#define WHILE(cond, body) CreateNode (TYPE_WHILE, 0, cond, body)

// -----------------------------------------------------------------------

struct Var_t
{
    char name [MAX_NAME_LEN];
    int is_visible;
};

struct Func_t
{
    char name [MAX_NAME_LEN];
};


struct Prog_t
{
    size_t  var_table_size;
    size_t func_table_size;
    size_t       code_size;

    size_t  var_table_capacity;
    size_t func_table_capacity;
    size_t       code_capacity;

    Var_t *var_table;
    Func_t *func_table;
    TreeElem_t *code;

    size_t index;
    Tree_t tree;
};


enum FICNODEVALUES
{
    FIC_COMMA        = 1,
    FIC_SEMICOLON    = 2,
    FIC_OPENBRACKET  = 3,
    FIC_CLOSEBRACKET = 4,
    FIC_OPENBRACE    = 5,
    FIC_CLOSEBRACE   = 6,
};

enum COMPILATIONERRORS
{
    COMP_OK             = 0,
    COMP_COMMENT_ERROR  = 1,
    COMP_UNKNOWN_SYMB   = 2,
    COMP_NAME_LONG      = 3,
    COMP_MULTIPLE_DEC   = 4,
    COMP_NO_DEC         = 5,
    COMP_SYNTAX_ERROR   = 6,
    
};


int ProgCtor (Prog_t *prog);

int ProgDtor (Prog_t *prog);

int ProgAddVar (Prog_t *prog, const char *varname);

int GetVarIndex (Prog_t *prog, const char *name);

int ProgAddNode (Prog_t *prog, int type, int value);

char *ReadProg (char *filename);

size_t GetSize (FILE *inp_file);

int GetCode (Prog_t *prog, char *text);

int Read_num (char **ch_ptr, int *num);

int Read_ternary (char **ch_ptr);

int Read_word (char **ch_ptr, const char *word);

int Skip_comment (char **ch_ptr);

int Start_of_area (Prog_t *prog, Stack_t *stk);

int End_of_area (Prog_t *prog, Stack_t *stk);

int Prog_dec_var (Prog_t *prog, char **ch_ptr, int start_of_area_index);

int Prog_read_var (Prog_t *prog, char **ch_ptr);

int Read_var (char **ch_ptr, char *buf, size_t maxlen);

int BackStrncmp (const char *str1, const char *str2, size_t num);

#endif