#ifndef TREE_H
#define TREE_H


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


const char *const LOGFILENAME = "log/treelog.html";
extern FILE *LOG;

const int POISON_VAL = 0xE228F3AE;

struct TreeInfo_t
{
    const char*      name;
    const char* func_name;
    const char* file_name;
    int              line;
};

struct TreeElem_t
{
    int type;
    int value;

    TreeElem_t* parent;
    TreeElem_t*  left;
    TreeElem_t* right;
};

struct Tree_t
{
    TreeInfo_t info;
    int status;
    int err;
    int size;

    TreeElem_t data;
};

enum TREESTATUS
{
    TREE_CREATED       = 0,
    TREE_CONSTRUCTED   = 1,
    TREE_DECONSTRUCTED = 2,
};

enum TREEPOSITIONS
{
    LEFT  = 0,
    RIGHT = 1,
};

enum TREEERRORS
{
    TREE_OK                 =   0,
    TREE_NULLPTR_ARG        =   1,
    TREE_STATUS_ERROR       =   2,
    TREE_ALLOC_ERROR        =   4,
    TREE_INFO_CORRUPTED     =   8,
    TREE_DATA_CORRUPTED     =  16,
    TREE_INCORRECT_SIZE     =  32,
    TREE_INCORRECT_POSITION =  64,
    TREE_INCORRECT_FORMAT   = 128,

};

enum NODE_TYPES
{
    TYPE_HLT     = -1,
    TYPE_FIC     =  0,
    TYPE_NUM     =  1,
    TYPE_VAR     =  2,
    TYPE_IF      =  3,
    TYPE_ELSE    =  4,
    TYPE_WHILE   =  5,
    TYPE_OP      =  6,
    TYPE_VARDEC  =  7,
    TYPE_FUNCDEC =  8,
    TYPE_CALL    =  9,
    TYPE_RETURN  = 10,
};

enum OPERATORS
{
    OP_ADD    =  1,
    OP_SUB    =  2,
    OP_MUL    =  3,
    OP_DIV    =  4,
    OP_SQRT   =  5,
    OP_IN     =  6,
    OP_OUT    =  7,
    OP_EQ     =  8,
    OP_MORE   =  9,
    OP_LESS   = 10,
    OP_MOREEQ = 11,
    OP_LESSEQ = 12,
    OP_NEQ    = 13,
    OP_NOT    = 14,
    OP_OR     = 15,
    OP_AND    = 16,
    OP_ASSIGN = 17,
    OP_POW    = 18,
    OP_SIN    = 100,
};

//DSL --------------------------------------------------------------------

#define L (elem ->   left)
#define R (elem ->  right)
#define P (elem -> parent)

#define TYPE  (elem -> type)
#define VAL   (elem -> value)
#define LVAL  (L    -> value)
#define RVAL  (R    -> value)
#define LTYPE (L    -> type)
#define RTYPE (R    -> type)

#define LL (L ->   left)
#define LR (L ->  right)
#define LP (L -> parent)
#define RL (R ->   left)
#define RR (R ->  right)
#define RP (R -> parent)
#define PL (P ->   left)
#define PR (P ->  right)

// -----------------------------------------------------------------------

#define TreeCtor(tree) Tree_ctor (tree, #tree, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define TreeTxtDump(tree, stream) Tree_txt_dmup (tree, stream, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define TreeDump(tree) Tree_dump (tree, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define TreeVerify(tree)   {int _tree_verify_err = Tree_verify (tree);  \
                            if (_tree_verify_err)                       \
                            {                                           \
                                TreePrintError (tree);                  \
                                return _tree_verify_err;                \
                            }}

#define TreePrintError(tree) Tree_print_error (tree, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define TreeErr(tree, error)  {tree -> err |= error;   \
                                TreePrintError (tree); \
                                return tree -> err; }


int Tree_ctor (Tree_t *tree, const char *name, const char *func_name, const char *file_name, int line);

void Tree_set_psn (TreeElem_t *elem);

void Tree_set_info (Tree_t *tree, const char *name, const char *func_name, const char *file_name, int line);

int TreeDtor (Tree_t *tree);

int TreeFreeData (Tree_t *tree, TreeElem_t *elem);

int Tree_free_data (TreeElem_t *elem, int *size);

TreeElem_t *TreeAllocElem (void);

int TreeAddElem (Tree_t *tree, TreeElem_t *parent, int position, TreeElem_t *newelem);

int TreeAddLeft (Tree_t *tree, TreeElem_t *parent, TreeElem_t *newelem);

int TreeAddRight (Tree_t *tree, TreeElem_t *parent, TreeElem_t *newelem);

int Tree_verify (Tree_t *tree);

int Tree_verify_data (TreeElem_t *elem, int *size);

void Tree_print_error (Tree_t *tree, const char *func_name, const char *file_name, int line);

void TreePrintVal (FILE *stream, TreeElem_t *elem);

void Tree_txt_dmup (Tree_t *tree, FILE *stream, const char *func_name, const char *file_name, int line);

void Tree_print_data (FILE *stream, TreeElem_t *elem);

void Tree_dump (Tree_t *tree, const char *func_name, const char *file_name, int line);

void Tree_generate_img (Tree_t *tree, int imgnum);

void Tree_draw_data (FILE *graph, TreeElem_t *elem, int rank, int *size, int print_adress);

int Tree_get_size (TreeElem_t *elem);

TreeElem_t *CreateNode (int type, int value, TreeElem_t *left, TreeElem_t *right);

TreeElem_t *CreateOp (int op, TreeElem_t *left, TreeElem_t *right);

#endif