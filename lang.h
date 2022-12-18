#ifndef LANG_H
#define LANG_H

#include "tree/tree.h"
#include "stack/stack.h"
#include "math.h"
#include "sys/stat.h"

const size_t BUFSIZE = 256;
const size_t BASE_TABLE_CAPACITY =  8;
const size_t BASE_CODE_CAPACITY  = 32;
const size_t BASE_ARGS_CAPACITY  =  2;
const size_t MAX_NAME_LEN        = 32;

const char *const  WHILE_WORD = "88888888";
const char *const     IF_WORD = "96";
const char *const   ELSE_WORD = "97";
const char *const VARDEC_WORD = "~~~~";
const char *const   SQRT_WORD = "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";

const char *const REV_OP_WORDS [] = {"", "(", ")", "[", "]", SQRT_WORD, "?", "!", "\"", ".", ",", ":", ";", "=", "-", "+", "*", "~", };

const char *const TEMPFILENAME = "temp.txt";

//DSL --------------------------------------------------------------------

#define CURRENT (prog -> code [prog -> index    ])
#define NEXT    (prog -> code [prog -> index + 1])

#define     IsNum(node) ((node).type == TYPE_NUM)
#define     IsVar(node) ((node).type == TYPE_VAR)
#define      IsIf(node) ((node).type == TYPE_IF)
#define    IsElse(node) ((node).type == TYPE_ELSE)
#define   IsWhile(node) ((node).type == TYPE_WHILE)
#define  IsVardec(node) ((node).type == TYPE_VARDEC)
#define  IsReturn(node) ((node).type == TYPE_RETURN)
#define    IsCall(node) ((node).type == TYPE_CALL)
#define IsFuncdec(node) ((node).type == TYPE_FUNCDEC)

#define        IsComma(node) ((node).type == TYPE_FIC && (node).value == FIC_COMMA)
#define    IsSemicolon(node) ((node).type == TYPE_FIC && (node).value == FIC_SEMICOLON)
#define    IsOpenBrace(node) ((node).type == TYPE_FIC && (node).value == FIC_OPENBRACE)
#define   IsCloseBrace(node) ((node).type == TYPE_FIC && (node).value == FIC_CLOSEBRACE)
#define  IsOpenBracket(node) ((node).type == TYPE_FIC && (node).value == FIC_OPENBRACKET)
#define IsCloseBracket(node) ((node).type == TYPE_FIC && (node).value == FIC_CLOSEBRACKET)
#define    IsEndOfProg(node) ((node).type == TYPE_FIC && (node).value == 0)

#define   IsAssign(node) ((node).type == TYPE_OP &&  (node).value == OP_ASSIGN)
#define       IsOr(node) ((node).type == TYPE_OP &&  (node).value == OP_OR)
#define      IsAnd(node) ((node).type == TYPE_OP &&  (node).value == OP_AND)
#define IsAddOrSub(node) ((node).type == TYPE_OP && ((node).value == OP_ADD || (node).value == OP_SUB))
#define IsMulOrDiv(node) ((node).type == TYPE_OP && ((node).value == OP_MUL || (node).value == OP_DIV))
#define     IsSqrt(node) ((node).type == TYPE_OP &&  (node).value == OP_SQRT)
#define       IsIn(node) ((node).type == TYPE_OP &&  (node).value == OP_IN)
#define      IsOut(node) ((node).type == TYPE_OP &&  (node).value == OP_OUT)

#define     IsComp(node) ((node).type == TYPE_OP &&  (node).value >=  8 && (node).value <= 13 )
#define   IsArithm(node) ((node).type == TYPE_OP &&  (node).value >=  1 && (node).value <=  4 )
#define    IsLogic(node) ((node).type == TYPE_OP && ((node).value == 15 || (node).value == 16))


#define NUM(x) CreateNode (TYPE_NUM, x, NULL, NULL)
#define VAR(i) CreateNode (TYPE_VAR, i, NULL, NULL)

#define     OR(left, right) CreateOp (OP_OR    , left, right)
#define    AND(left, right) CreateOp (OP_AND   , left, right)
#define ASSIGN(left, right) CreateOp (OP_ASSIGN, left, right)
#define    OUT(node)        CreateOp (OP_OUT   , node, NULL)
#define    NOT(node)        CreateOp (OP_NOT   , node, NULL)
#define   SQRT(node)        CreateOp (OP_SQRT  , node, NULL)
#define     IN              CreateOp (OP_IN    , NULL, NULL)


#define FIC(left, right)           CreateNode (TYPE_FIC    , 0    , left, right)
#define IF(cond, iftrue, iffalse)  CreateNode (TYPE_IF     , 0    , cond, CreateNode (TYPE_ELSE, 0, iftrue, iffalse))
#define WHILE(cond, body)          CreateNode (TYPE_WHILE  , 0    , cond, body)
#define VARDEC(var)                CreateNode (TYPE_VARDEC , var  , NULL, NULL)
#define RETURN(node)               CreateNode (TYPE_RETURN , 0    , node, NULL)
#define CALL(index)                CreateNode (TYPE_CALL   , index, NULL, NULL)
#define FUNCDEC(index, args, body) CreateNode (TYPE_FUNCDEC, index, args, body)

// -----------------------------------------------------------------------

struct Var_t
{
    char name [MAX_NAME_LEN];
    int is_visible;
    int index_in_func;
};

struct Func_t
{
    char name [MAX_NAME_LEN];
    int num_of_args;
    int num_of_vars;
    int *args;
    size_t args_capacity;
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

    int vars_in_main;
    int label;
};


enum FICNODEVALUES
{
    FIC_COMMA        = 1,
    FIC_SEMICOLON    = 2,
    FIC_OPENBRACKET  = 3,
    FIC_CLOSEBRACKET = 4,
    FIC_OPENBRACE    = 5,
    FIC_CLOSEBRACE   = 6,
    FIC_START        = 7,
    FIC_CALL         = 8,
};

enum COMPRETVALS
{
    COMP_OK    = 0,
    COMP_ERROR = 1,
};


int ProgCtor (Prog_t *prog);

int ProgDtor (Prog_t *prog);

int ProgAddVar (Prog_t *prog, const char *varname);

int ProgAddFunc (Prog_t *prog, const char *funcname);

int GetVarIndex (Prog_t *prog, const char *name);

int GetFuncIndex (Prog_t *prog, const char *name);

int ProgAddNode (Prog_t *prog, int type, int value);

char *ReadProg (const char *filename);

size_t GetSize (FILE *inp_file);

int GetCode (Prog_t *prog, char *text);

int Read_num (char **ch_ptr, int *num);

int Read_ternary (char **ch_ptr);

int Read_word (char **ch_ptr, const char *word);

int Back_skip_comment (char **ch_ptr);

int Start_of_area (Prog_t *prog, Stack_t *stk);

int End_of_area (Prog_t *prog, Stack_t *stk);

int Prog_dec_var (Prog_t *prog, char **ch_ptr, int start_of_area_index, int addnode);

int Prog_dec_func (Prog_t *prog, char **ch_ptr, Stack_t *stk);

int Prog_read_func_args (Prog_t *prog, char **ch_ptr, int start_of_area_index);

int Func_add_arg (Func_t *func, int index);

int Prog_read_call (Prog_t *prog, char **ch_ptr);

int Prog_read_var (Prog_t *prog, char **ch_ptr);

int Read_var (char **ch_ptr, char *buf, size_t maxlen);

int BackStrncmp (const char *str1, const char *str2, size_t num);

void BackSkipSpaces (char **ch_ptr);

int GetTree (Prog_t *prog);

int SaveProg (Prog_t *prog, const char *filename);

void Save_var_table (Prog_t *prog, FILE *file);

void Save_func_table (Prog_t *prog, FILE *file);

void Save_tree (Prog_t *prog, FILE *file);

void Print_node (FILE *file, TreeElem_t *elem, int num_of_spaces);


TreeElem_t *GetProg (Prog_t *prog);

TreeElem_t *GetFuncdec (Prog_t *prog);

TreeElem_t *GetBody (Prog_t *prog);

TreeElem_t *GetBodyWithoutBraces (Prog_t *prog);

TreeElem_t *GetDec (Prog_t *prog);

TreeElem_t *GetReturn (Prog_t *prog);

TreeElem_t *GetIf (Prog_t *prog);

TreeElem_t *GetWhile (Prog_t *prog);

TreeElem_t *GetCond (Prog_t *prog);

TreeElem_t *GetExp (Prog_t *prog);

TreeElem_t *GetAnd (Prog_t *prog);

TreeElem_t *GetComp (Prog_t *prog);

TreeElem_t *GetSum (Prog_t *prog);

TreeElem_t *GetTerm (Prog_t *prog);

TreeElem_t *GetSqrt (Prog_t *prog);

TreeElem_t *GetBrack (Prog_t *prog);

TreeElem_t *GetFunc (Prog_t *prog);

TreeElem_t *GetCall (Prog_t *prog);


int LoadProg (Prog_t *prog, const char *filename);

char *Load_var_table (Prog_t *prog, char *ch);

char *Load_func_table (Prog_t *prog, char *ch);

char *Load_tree  (Prog_t *prog, char *ch);

TreeElem_t *Read_node (char **ch_ptr);

char *SkipSpacesAndComments (char *ch);

char *SkipSpaces (char *ch);

char *SkipComment (char *ch);

int GenerateAsm (Prog_t *prog, const char *filename);

int Compile_prog (Prog_t *prog, FILE *file);

int Get_var_indexes (Prog_t *prog);

void Count_func_vars (Prog_t *prog, TreeElem_t *func);

void Count_vars (Prog_t *prog, TreeElem_t *elem, int *count, int is_main);

void Replace_fic_with_call (TreeElem_t *elem);

int Compile (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_fic (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_num (FILE *file, TreeElem_t *elem);

int Compile_var (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_if (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_while (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_op (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_assign (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_sqrt (FILE *file);

int Compile_in (FILE *file);

int Compile_out (FILE *file);

int Compile_not (Prog_t *prog, FILE *file);

int Compile_arithm (FILE *file, TreeElem_t *elem);

int Compile_comp (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_logic (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_vardec (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_funcdec (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_call (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_return (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Compile_hlt (FILE *file);


int GenerateText (Prog_t *prog, const char *filename);

int ReverseFile (const char *in_name, const char *out_name);

int Reverse (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_fic (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_num (FILE *file, TreeElem_t *elem);

int Reverse_var (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_if (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_while (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_op (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_onearg (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_twoarg (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_in (FILE *file);

int Reverse_vardec (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_funcdec (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_decargs (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_call (Prog_t *prog, FILE *file, TreeElem_t *elem);

int Reverse_callargs (Prog_t *prog, FILE *file, TreeElem_t *elem, int *first_arg);

int Reverse_return (Prog_t *prog, FILE *file, TreeElem_t *elem);

void Dec_to_rev_tern (int num, char *buf);

int GetOpRank (int op);

#endif