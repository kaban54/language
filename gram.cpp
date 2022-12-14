#include "lang.h"


TreeElem_t *GetProg (Prog_t *prog)
{
    return GetBody (prog);
}


TreeElem_t *GetBody (Prog_t *prog)
{
    if (!IsOpenBrace (CURRENT)) return nullptr;
    prog -> index ++;

    TreeElem_t *elem = FIC (NULL, NULL);
    TreeElem_t *ret  = elem;

    while (1)
    {
        if (IsCloseBrace (CURRENT)) break;

        L = GetIf (prog);
        if (!L) L = GetWhile (prog);
        if (!L) L = GetBody  (prog);
        if (!L) L = GetDec   (prog);
        if (!L)
        {
            L = GetExp (prog);
            if (!L || !IsSemicolon (CURRENT))
            {
                if (!IsSemicolon (CURRENT)) printf ("Syntax error: missing semicolon after expression.\n");
                Tree_free_data (ret, NULL);
                return nullptr;
            }
            prog -> index ++;
        }

        LP = elem;
        R = FIC (NULL, NULL);
        RP = elem;
        elem = R;
    }
    return ret;
}

TreeElem_t *GetDec (Prog_t *prog)
{
    if (!IsVardec (CURRENT)) return nullptr;
    if (!IsSemicolon (NEXT))
    {
        printf ("Syntax error: missing semicolon after variable declaration.\n");
        return nullptr;
    }
    int var = CURRENT.value;
    prog -> index += 2;
    return VARDEC (var);
}

TreeElem_t *GetIf (Prog_t *prog)
{
    if (!IsIf (CURRENT)) return nullptr;

    prog -> index ++;

    TreeElem_t *iftrue = GetBody (prog);
    if (iftrue == nullptr)
    {
        printf ("Syntax error: no body in if operator.\n");
        return nullptr;
    }

    TreeElem_t *cond = GetCond (prog);
    if (cond == nullptr)
    {
        printf ("Syntax error: no condition in if operator.\n");
        Tree_free_data (iftrue, NULL);
        return nullptr;
    }

    TreeElem_t *iffalse = nullptr;
    if (IsElse (CURRENT))
    {
        prog -> index ++;
        iffalse = GetBody (prog);
        if (iffalse == nullptr)
        {
            printf ("Syntax error: no body after else.\n");
            Tree_free_data (iftrue, NULL);
            Tree_free_data (cond  , NULL);
            return nullptr;
        }
    }

    return IF (cond, iftrue, iffalse);
}

TreeElem_t *GetWhile (Prog_t *prog)
{
    if (!IsWhile (CURRENT)) return nullptr;

    prog -> index ++;

    TreeElem_t *cond = GetCond (prog);
    if (cond == nullptr)
    {
        printf ("Syntax error: no condition in while operator.\n");
        return nullptr;
    }

    TreeElem_t *body = GetBody (prog);
    if (cond == nullptr)
    {
        printf ("Syntax error: no body in while operator.\n");
        Tree_free_data (cond, NULL);
        return nullptr;
    }

    return WHILE (cond, body);
}

TreeElem_t *GetCond (Prog_t *prog)
{
    if (!IsOpenBracket (CURRENT)) return nullptr;
    prog -> index ++;

    TreeElem_t *ret = GetExp (prog);
    if (ret == nullptr) return nullptr;

    if (!IsCloseBracket (CURRENT))
    {
        printf ("Syntax error: no close bracket.\n");
        Tree_free_data (ret, NULL);
        return nullptr;
    }
    prog -> index ++;
    return ret;
}

TreeElem_t *GetExp (Prog_t *prog)
{
    if (IsVar (CURRENT) && IsAssign (NEXT))
    {
        TreeElem_t *ret = ASSIGN (VAR (CURRENT.value), NULL);
        
        prog -> index += 2;

        TreeElem_t *exp = GetExp (prog);
        if (exp == nullptr)
        {
            Tree_free_data (ret, NULL);
            return nullptr;
        }

        ret -> right  = exp;
        exp -> parent = ret;
        return ret;
    }

    TreeElem_t *ret = GetAnd (prog);
    if (ret == nullptr) return nullptr;

    TreeElem_t *elem = nullptr;

    while (IsOr (CURRENT))
    {
        prog -> index ++;
        elem = GetAnd (prog);
        if (elem == nullptr)
        {
            Tree_free_data (ret, NULL);
            return nullptr;
        }
        ret = OR (ret, elem);
    }
    return ret;
}

TreeElem_t *GetAnd (Prog_t *prog)
{
    TreeElem_t *ret = GetComp (prog);
    if (ret == nullptr) return nullptr;

    TreeElem_t *elem = nullptr;

    while (IsAnd (CURRENT))
    {
        prog -> index ++;
        elem = GetComp (prog);
        if (elem == nullptr)
        {
            Tree_free_data (ret, NULL);
            return nullptr;
        }
        ret = AND (ret, elem);
    }
    return ret;
}

TreeElem_t *GetComp (Prog_t *prog)
{
    TreeElem_t *ret = GetSum (prog);
    if (ret == nullptr) return nullptr;

    TreeElem_t *elem = nullptr;

    while (IsComp (CURRENT))
    {
        ret = CreateOp (CURRENT.value, ret, NULL);

        prog -> index ++;

        elem = GetSum (prog);
        if (elem == nullptr)
        {
            Tree_free_data (ret, NULL);
            return nullptr;
        }
        ret -> right = elem;
        P = ret;
    }
    return ret;
}

TreeElem_t *GetSum (Prog_t *prog)
{
    TreeElem_t *ret = GetTerm (prog);
    if (ret == nullptr) return nullptr;

    TreeElem_t *elem = nullptr;

    while (IsAddOrSub (CURRENT))
    {
        ret = CreateOp (CURRENT.value, ret, NULL);

        prog -> index ++;
        
        elem = GetTerm (prog);
        if (elem == nullptr)
        {
            Tree_free_data (ret, NULL);
            return nullptr;
        }
        ret -> right = elem;
        P = ret;
    }
    return ret;
}

TreeElem_t *GetTerm (Prog_t *prog)
{
    TreeElem_t *ret = GetSqrt (prog);
    if (ret == nullptr) return nullptr;

    TreeElem_t *elem = nullptr;

    while (IsMulOrDiv (CURRENT))
    {
        ret = CreateOp (CURRENT.value, ret, NULL);

        prog -> index ++;
        
        elem = GetSqrt (prog);
        if (elem == nullptr)
        {
            Tree_free_data (ret, NULL);
            return nullptr;
        }
        ret -> right = elem;
        P = ret;
    }
    return ret;
}

TreeElem_t *GetSqrt (Prog_t *prog)
{
    if (IsSqrt (CURRENT))
    {
        prog -> index ++;

        TreeElem_t *elem = GetBrack (prog);
        if (elem == nullptr) return nullptr;

        return SQRT (elem);
    }

    return GetBrack (prog);
}

TreeElem_t *GetBrack (Prog_t *prog)
{
    TreeElem_t *ret = GetCond (prog);
    if (ret) return ret;

    return GetFunc (prog);
}

TreeElem_t *GetFunc (Prog_t *prog)
{
    if (IsIn (CURRENT))
    {
        prog -> index ++;
        return IN;
    }
    if (IsOut (CURRENT))
    {
        prog -> index ++;
        TreeElem_t *elem = GetCond (prog);
        if (elem == nullptr) return nullptr;
        return OUT (elem);
    }
    if (IsVar (CURRENT))
    {
        int var = CURRENT.value;
        prog -> index ++;
        return VAR (var);
    }
    if (IsNum (CURRENT))
    {
        int num = CURRENT.value;
        prog -> index ++;
        return NUM (num);
    }

    printf ("Syntax error: incorrect expression format.\n");
    return nullptr;
}