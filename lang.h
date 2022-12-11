#ifndef LANG_H
#define LANG_H

#include "tree/tree.h"


const size_t BASE_TABLE_CAPACITY =  8;
const size_t BASE_CODE_CAPACITY  = 32;
const size_t MAX_NAME_LEN  = 32;


struct Var_t
{
    char name [MAX_NAME_LEN];
    int is_declared;
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

    Tree_t tree;
}


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

};

#endif