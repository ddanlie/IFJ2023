#include "semantic.h"


bool varRedefinition(char *var)
{
    if(current_local_level == 0)
    {
        symtb_token *found;
        if(getFromGlobalTable(var, &found))
        {
            analysis_error = UNDEF_ERROR;
            return true;
        }
    }
    else
    {
        symtable *table_found;
        if(getFromLocalTables(var, NULL, &table_found))
        {
            if(table_found->local_level == current_local_level)
            {
                analysis_error = UNDEF_ERROR;
                return true;
            }
        }
    }
    
    return false;
}


bool isVarDefined(char *var)
{
    symtb_token *found;
    if(getFromGlobalTable(var, &found))
    {
        return true;
    }
    
    symtable *table_found;
    if(getFromLocalTables(var, NULL, &table_found))
    {
        if(table_found->local_level > current_local_level)//higher level -> deeper level
        {
            analysis_error = VAR_INIT_ERROR;
            return false;
        }
        else
        {
            return true;
        }
    }
}

bool isFuncDefined(char *func)
{
    symtb_token *found;
    if(getFromGlobalTable(func, &found))
    {
        return true;
    }
    analysis_error = UNDEF_ERROR;
    return false;
}

bool compareFunctionsSignature(symtb_token f1, symtb_token f2)
{
    bool res = true;
    if(f1.type != f2.type)//that would be really strange
        res = false;
    if(f1.funcArgsSize != f2.funcArgsSize)
        res = false;
    if(f1.lit_type != f2.lit_type)//return types
        res = false;
    
    for(int i = 0; i < f1.funcArgsSize; i++)
    {
        if(f1.funcArgTypes != f2.funcArgTypes)
        {
            res = false;
            break;
        }
        
        if(0 != strcmp(f1.funcArgnames[i], "_"))
        {
            if(0 != strcmp(f1.funcArgnames[i], f2.funcArgnames[i]))
            {
                res = false;
                break;
            }
        }
    }
    
    if(!res)
    {
        analysis_error = FUNC_PARAM_ERROR;
    }
    return res;
}

bool compareUndefinedFunction(symtb_token defined, symtb_token undefined)
{
    bool res = true;
    if(defined.type != undefined.type)//that would be really strange
        res = false;
    if(defined.funcArgsSize != undefined.funcArgsSize)
        res = false;
    if(defined.lit_type != undefined.lit_type)//return types
    {
        //convert conditions for ease to understand what types are compatible
        if((undefined.lit_type == INT_TYPE || undefined.lit_type == NINT_TYPE || undefined.lit_type == NINT_NIL_TYPE) &&
            (defined.lit_type == INT_TYPE || defined.lit_type == NINT_TYPE))
        {}
        else if((undefined.lit_type == DOUBLE_TYPE || undefined.lit_type == NDOUBLE_TYPE || undefined.lit_type == NDOUBLE_NIL_TYPE) &&
            (defined.lit_type == DOUBLE_TYPE || defined.lit_type == NDOUBLE_TYPE))
        {}
        else if((undefined.lit_type == STRING_TYPE || undefined.lit_type == NSTRING_TYPE || undefined.lit_type == NSTRING_NIL_TYPE) &&
                  (defined.lit_type == STRING_TYPE || defined.lit_type == NSTRING_TYPE))
        {}
        else if(undefined.lit_type == VOID_TYPE)
        {}
        else
            res = false;
    }
    
    for(int i = 0; i < defined.funcArgsSize; i++)
    {
        if((undefined.funcArgTypes[i] == INT_TYPE)
            && (defined.funcArgTypes[i] == INT_TYPE || defined.funcArgTypes[i] == NINT_TYPE))
        {}
        else if((undefined.funcArgTypes[i] == NINT_TYPE || undefined.funcArgTypes[i] == NIL_TYPE)
                && (defined.funcArgTypes[i] == NINT_TYPE))
        {}
        else  if((undefined.funcArgTypes[i] == DOUBLE_TYPE)
                && (defined.funcArgTypes[i] == DOUBLE_TYPE || defined.funcArgTypes[i] == NDOUBLE_TYPE))
        {}
        else if((undefined.funcArgTypes[i] == NDOUBLE_TYPE || undefined.funcArgTypes[i] == NIL_TYPE)
                && (defined.funcArgTypes[i] == NDOUBLE_TYPE))
        {}
        else if((undefined.funcArgTypes[i] == STRING_TYPE)
                && (defined.funcArgTypes[i] == STRING_TYPE || defined.funcArgTypes[i] == NSTRING_TYPE))
        {}
        else if((undefined.funcArgTypes[i] == NSTRING_TYPE || undefined.funcArgTypes[i] == NIL_TYPE)
                && (defined.funcArgTypes[i] == NSTRING_TYPE))
        {}
        else
        {
            res = false;
            break;
        }
        
        if(0 != strcmp(undefined.funcArgnames[i], "_"))
        {
            if(0 != strcmp(undefined.funcArgnames[i], defined.funcArgnames[i]))
            {
                res = false;
                break;
            }
        }
    }
    
    if(!res)
    {
        analysis_error = FUNC_PARAM_ERROR;
    }
    return res;
}

bool compareIDtoFuncReturn(symtb_token var, symtb_token f)
{
    if(var.lit_type == f.lit_type)
        return true;
    if((var.lit_type == NINT_TYPE || var.lit_type == NINT_NIL_TYPE) &&
         (f.lit_type == NINT_TYPE || f.lit_type == INT_TYPE))
    {}
    else if((var.lit_type == NDOUBLE_TYPE || var.lit_type == NDOUBLE_NIL_TYPE) &&
            (f.lit_type == NDOUBLE_TYPE || f.lit_type == DOUBLE_TYPE))
    {}
    else if((var.lit_type == NSTRING_TYPE || var.lit_type == NSTRING_NIL_TYPE) &&
            (f.lit_type == NSTRING_TYPE || f.lit_type == STRING_TYPE))
    {}
    else
    {
        analysis_error = TYPE_COMPAT_ERROR;
        return false;
    }
    
    return true;
}


void addVarToFrame(symtb_token var)
{
    if(current_local_level == 0)//global
    {
        symtb_insert(&global_table, var.id_name, var);
    }
    else
    {
        symtb_insert(&temporary_table, var.id_name, var);
    }
}


bool vardefCompareTypeExpr(symtb_token *var, literal_type expr_type)
{
    if(var->lit_type != UNDEF_TYPE)//is there a type?
    {
        if(expr_type != UNDEF_TYPE)//is there an expression?
        {
            //does expression correspond to type?
            if(var->lit_type == INT_TYPE && expr_type == INT_TYPE)
            {
                return true;
            }
            else if(var->lit_type == DOUBLE_TYPE && expr_type == INT_TYPE)
            {
                return true;
            }
            else if(var->lit_type == STRING_TYPE && expr_type == STRING_TYPE)
            {
                return true;
            }
            else if(var->lit_type == NSTRING_TYPE &&
                    (expr_type == STRING_TYPE || expr_type == NSTRING_TYPE || expr_type == NSTRING_NIL_TYPE || expr_type == NIL_TYPE))
            {
                return true;
            }
            else if(var->lit_type == NINT_TYPE &&
                    (expr_type == INT_TYPE || expr_type == NINT_TYPE || expr_type == NINT_NIL_TYPE || expr_type == NIL_TYPE))
            {
                return true;
            }
            else if(var->lit_type == NDOUBLE_TYPE &&
                    (expr_type == DOUBLE_TYPE || expr_type == NDOUBLE_TYPE || expr_type == NDOUBLE_NIL_TYPE || expr_type == NIL_TYPE))
            {
                return true;
            }
            else
            {
                analysis_error = TYPE_COMPAT_ERROR;
                return false;
            }
        }
        else//no expression
        {
            return true;
        }
    }
    else//no type
    {
        if(expr_type != UNDEF_TYPE)//is there an expression
        {
            //can you derive the type from the expression?
            if(expr_type == NIL_TYPE)
            {
                analysis_error = TYPE_DERIV_ERROR;
                return false;
            }
            
            var->lit_type = expr_type;
            return true;
        }
        else//no expression
        {
            analysis_error = SYNTAX_ERROR;
            return false;
        }
    }
}


bool setLiteralType(literal_type *type, lex_token lxtoken, bool checkInit)
{
    switch(lxtoken.lexeme_type)
    {
        case ID://find variable and set its type as current type
        {
            symtb_token *found;
            if(getFromGlobalTable(lxtoken.str.value, &found))
            {
                if(checkInit && !found->initialized)
                {
                    analysis_error = VAR_INIT_ERROR;
                    return false;
                }
                
                *type = found->lit_type;
            }
            else
            {
                analysis_error = VAR_INIT_ERROR;
                return false;
            }
            break;
        }
        case INT_LIT:
        {
            *type = INT_TYPE;
            break;
        }
        case DOUBLE_LIT:
        {
            *type = DOUBLE_TYPE;
            break;
        }
        case STRING_LIT:
        {
            *type = STRING_TYPE;
            break;
        }
        case NIL:
        {
            *type = NIL_TYPE;
            break;
        }
        default:
            analysis_error = SEMANTIC_OTHER_ERROR;
            return false;
    }
    
    return true;
}

bool ruleTypeCheck(int rule_index, Stack *expr_stack)
{
    stackResetSemiPop(expr_stack);
    switch(rule_index)
    {
        case 0://ID1 -> RBR1 ID1 LBR1
        {
            stackSemiPop(expr_stack);
            expr_lexeme *el = stackSemiPop(expr_stack);
            stackResetSemiPop(expr_stack);
            
            current_expr_type = el->type;
            break;
        }
        case 1://ID1 -> ID
        {
            expr_lexeme *el = stackSemiPop(expr_stack);
            stackResetSemiPop(expr_stack);
            if(!setLiteralType(&current_expr_type, el->lxtoken, true))//we do not use el->type as it is not present
                return false;
            break;
        }
        case 2://ID1 -> EXCLAM ID1
        {
            stackSemiPop(expr_stack);
            expr_lexeme *el = stackSemiPop(expr_stack);
            stackResetSemiPop(expr_stack);
            switch(el->type)
            {
                case NIL_TYPE:
                case NINT_NIL_TYPE:
                case NDOUBLE_NIL_TYPE:
                case NSTRING_NIL_TYPE:
                    analysis_error = TYPE_COMPAT_ERROR;
                    return false;
                default:
                    break;
            }
            break;
        }
        case 3://ID1 -> ID1 MUL ID1
        case 4://ID1 -> ID1 DIV ID1
        case 5://ID1 -> ID1 PLUS ID1
        case 6://ID1 -> ID1 MINUS ID1
        {
            expr_lexeme *op2 = stackSemiPop(expr_stack);
            stackSemiPop(expr_stack);
            expr_lexeme *op1 = stackSemiPop(expr_stack);
            stackResetSemiPop(expr_stack);
    
            if(rule_index == 5)//string concatenation
            {
                if(op1->type == STRING_TYPE && op2->type == STRING_TYPE)
                {
                    current_expr_type = STRING_TYPE;
                    break;
                }
            }
            
            if(op1->type == INT_TYPE && op2->type == INT_TYPE)
            {
                current_expr_type = INT_TYPE;
            }
            else if(op1->type == DOUBLE_TYPE && op2->type == DOUBLE_TYPE)
            {
                current_expr_type = DOUBLE_TYPE;
            }
            else if(op1->type == INT_TYPE && op1->lxtoken.lexeme_type == INT_LIT && op2->type == DOUBLE_TYPE)
            {
                current_expr_type = DOUBLE_TYPE;
            }
            else if(op1->type == DOUBLE_TYPE && op2->type == INT_TYPE && op2->lxtoken.lexeme_type == INT_LIT)
            {
                current_expr_type = DOUBLE_TYPE;
            }
            else
            {
                analysis_error = TYPE_COMPAT_ERROR;
                return false;
            }
            break;
        }
        case 7://ID1 -> ID1 EQ ID1
        case 8://ID1 -> ID1 NEQ ID1
        {
            expr_lexeme *op2 = stackSemiPop(expr_stack);
            stackSemiPop(expr_stack);
            expr_lexeme *op1 = stackSemiPop(expr_stack);
            stackResetSemiPop(expr_stack);
            
            if(op1->type == INT_TYPE && op2->type == INT_TYPE)
            {
                current_expr_type = UNDEF_TYPE;
            }
            else if((op1->type == NIL_TYPE || op1->type == NINT_NIL_TYPE || op1->type == NINT_TYPE) &&
                    (op2->type == NIL_TYPE || op2->type == NINT_NIL_TYPE  || op2->type == NINT_TYPE))//cannot compare Int and Int?
            {
                current_expr_type = UNDEF_TYPE;
            }
            else if(op1->type == DOUBLE_TYPE && op2->type == DOUBLE_TYPE)
            {
                current_expr_type = UNDEF_TYPE;
            }
            else if((op1->type == NIL_TYPE || op1->type == NDOUBLE_NIL_TYPE || op1->type == NDOUBLE_TYPE) &&
                    (op2->type == NIL_TYPE || op2->type == NDOUBLE_NIL_TYPE  || op2->type == NDOUBLE_TYPE))//cannot compare Double and Double?
            {
                current_expr_type = UNDEF_TYPE;
            }
            else if(op1->type == INT_TYPE && op1->lxtoken.lexeme_type == INT_LIT && op2->type == DOUBLE_TYPE)//conversion  with literal only
            {
                current_expr_type = UNDEF_TYPE;
            }
            else if(op1->type == DOUBLE_TYPE && op2->type == INT_TYPE && op2->lxtoken.lexeme_type == INT_LIT)//conversion  with literal only
            {
                current_expr_type = UNDEF_TYPE;
            }
            else if(op1->type == STRING_TYPE && op2->type == STRING_TYPE)
            {
                current_expr_type = UNDEF_TYPE;
            }
            else if((op1->type == NIL_TYPE || op1->type == NSTRING_NIL_TYPE || op1->type == NSTRING_TYPE) &&
                    (op2->type == NIL_TYPE || op2->type == NSTRING_NIL_TYPE || op2->type == NSTRING_TYPE))//cannot compare String and String?
            {
                current_expr_type = UNDEF_TYPE;
            }
            else
            {
                analysis_error = TYPE_COMPAT_ERROR;
                return false;
            }
            break;
        }
        case 9://ID1 -> ID1 LE ID1
        case 10://ID1 -> ID1 GT ID1
        case 11://ID1 -> ID1 LEQ ID1
        case 12://ID1 -> ID1 GEQ ID1
        {
            expr_lexeme *op2 = stackSemiPop(expr_stack);
            stackSemiPop(expr_stack);
            expr_lexeme *op1 = stackSemiPop(expr_stack);
            stackResetSemiPop(expr_stack);
            
            if(op2->type != op1->type)
            {
                analysis_error = TYPE_COMPAT_ERROR;
                return false;
            }
            break;
        }
        case 13://ID1 -> ID1 QQ ID1
        {
            expr_lexeme *op2 = stackSemiPop(expr_stack);
            stackSemiPop(expr_stack);
            expr_lexeme *op1 = stackSemiPop(expr_stack);
            stackResetSemiPop(expr_stack);
            
            if((op1->type == NIL_TYPE || op1->type == NDOUBLE_TYPE || op1->type == NDOUBLE_NIL_TYPE || op1->type == DOUBLE_TYPE) && op2->type == DOUBLE_TYPE)
            {
                current_expr_type = DOUBLE_TYPE;
            }
            else if((op1->type == NIL_TYPE || op1->type == NINT_TYPE || op1->type == NINT_NIL_TYPE || op1->type == INT_TYPE) && op2->type == INT_TYPE)
            {
                current_expr_type = INT_TYPE;
            }
            else if((op1->type == NIL_TYPE || op1->type == NSTRING_TYPE || op1->type == NSTRING_NIL_TYPE || op1->type == STRING_TYPE) && op2->type == STRING_TYPE)
            {
                current_expr_type = STRING_TYPE;
            }
            else
            {
                analysis_error = TYPE_COMPAT_ERROR;
                return false;
            }
            break;
        }
        default:
            stackResetSemiPop(expr_stack);
            analysis_error = SEMANTIC_OTHER_ERROR;
            return false;
    }
    stackResetSemiPop(expr_stack);
    return true;
}