//Implementace překladače imperativního jazyka IFJ23
//Danil Domrachev (xdomra00)

#include "semantic.h"


bool varRedefinition(char *var)
{
    if(current_local_level == 0)
    {
        symtb_token *found;
        if(getFromGlobalTable(var, &found))
        {
            if(found->type == VARIABLE || found->type == CONSTANT)
            {
                analysis_error = UNDEF_ERROR;
                return true;
            }
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

bool funcRedefinition(char *func)
{
    if(current_local_level == 0)
    {
        symtb_token *found;
        if(getFromGlobalTable(func, &found))
        {
            if(found->type == FUNCTION)
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
        if(found->lit_type == VARIABLE)
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
        if(found->type == FUNCTION)
            return true;
    }
    analysis_error = UNDEF_ERROR;
    return false;
}


bool compareFunctionsSignature(symtb_token orig, symtb_token called)
{

    bool res = true;
    if(orig.type != called.type)//that would be really strange
        res = false;
    if(orig.lit_type != called.lit_type)//return types
        res = false;
    
    if(0 == strcmp(orig.id_name, "write"))
    {
        return true;
    }
    
    if(orig.funcArgsSize != called.funcArgsSize)
        res = false;

    
    for(int i = 0; i < orig.funcArgsSize; i++)
    {
        switch(called.funcArgTypes[i])
        {
            case NIL_TYPE: case NINT_NIL_TYPE: case NSTRING_NIL_TYPE: case NDOUBLE_NIL_TYPE:
            {
                if(!(orig.funcArgTypes[i] == NSTRING_TYPE || orig.funcArgTypes[i] == NINT_TYPE
                    || orig.funcArgTypes[i] == NDOUBLE_TYPE))
                {
                    res = false;
                }
                break;
            }
            case INT_TYPE:
            {
                if(!(orig.funcArgTypes[i] == NINT_TYPE || orig.funcArgTypes[i] == INT_TYPE))
                {
                    res = false;
                }
                break;
            }
            case NINT_TYPE:
            {
                if(orig.funcArgTypes[i] != NINT_TYPE)
                {
                    res = false;
                }
                break;
            }
            case DOUBLE_TYPE:
            {
                if(!(orig.funcArgTypes[i] == NDOUBLE_TYPE || orig.funcArgTypes[i] == DOUBLE_TYPE))
                {
                    res = false;
                }
                break;
            }
            case NDOUBLE_TYPE:
            {
                if(orig.funcArgTypes[i] != NDOUBLE_TYPE)
                {
                    res = false;
                }
                break;
            }
            case NSTRING_TYPE:
            {
                if(orig.funcArgTypes[i] != NSTRING_TYPE)
                {
                    res = false;
                }
                break;
            }
            case STRING_TYPE:
            {
                if(!(orig.funcArgTypes[i] == NSTRING_TYPE || orig.funcArgTypes[i] == STRING_TYPE))
                {
                    res = false;
                }
                break;
            }
            default:
                res = false;
                break;
        }
        
        if(!res)
            break;
        
        if(0 != strcmp(orig.funcArgnames[i], "_"))
        {
            if(0 != strcmp(orig.funcArgnames[i], called.funcArgnames[i]))
            {
                res = false;
                break;
            }
        }
    }
    
    if(!res)
        analysis_error = FUNC_PARAM_ERROR;
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
            else if(var->lit_type == DOUBLE_TYPE && expr_type == DOUBLE_TYPE)
            {
                return true;
            }
            else if((var->lit_type == DOUBLE_TYPE || var->lit_type == NDOUBLE_TYPE) && expr_type == INT_TYPE && expr_is_literal)
            {
                //generator
                printf("INT2FLOAT %s %s\n", generator_expr_res_name, generator_expr_res_name);
                expr_is_literal = false;
                //generator end
                return true;
            }
            else if(var->lit_type == DOUBLE_TYPE && expr_type == DOUBLE_TYPE)
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
            if(expr_type == NIL_TYPE || expr_type == BOOL_TYPE)
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
            
            if(getFromEverywhere(lxtoken.str.value, &found, NULL))
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
            
            
            //generator
            generator_temp_res_name = generate_expr_var_name();
            move(generator_temp_res_name, el->generator_tmp_name);
            //generator end
            break;
        }
        case 1://ID1 -> ID
        {
            expr_lexeme *el = stackSemiPop(expr_stack);
            stackResetSemiPop(expr_stack);
            if(!setLiteralType(&current_expr_type, el->lxtoken, true))//we do not use el->type as it is not present
                return false;
            
            if(el->lxtoken.lexeme_type == ID)
                expr_is_literal = false;
            
            //generator
            generator_temp_res_name = generate_expr_var_name();
            if(el->lxtoken.lexeme_type == ID)//variable
            {
                char *vn = get_var_name(el->lxtoken.str.value);
                move(generator_temp_res_name, vn);
                free(vn);
            }
            else//constant
            {
                char *ln = get_literal_name(el->lxtoken);
                move(generator_temp_res_name, ln);
                free(ln);
            }
            //generator end
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
                case NINT_TYPE:
                    current_expr_type = INT_TYPE;
                    break;
                case NDOUBLE_TYPE:
                    current_expr_type = DOUBLE_TYPE;
                    break;
                case NSTRING_TYPE:
                    current_expr_type = STRING_TYPE;
                    break;
                default:
                    break;
            }
            
            //generator
            generator_temp_res_name = generate_expr_var_name();
            move(generator_temp_res_name, el->generator_tmp_name);
            //generator end
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
    
            //generator
            generator_temp_res_name = generate_expr_var_name();
            //generator end
            
            if(rule_index == 5)//string concatenation
            {
                if(op1->type == STRING_TYPE && op2->type == STRING_TYPE)
                {
                    current_expr_type = STRING_TYPE;
                    addr3op("CONCAT", generator_temp_res_name, op1->generator_tmp_name, op2->generator_tmp_name);
                    break;
                }
            }
    
            if(op1->type == INT_TYPE && op2->type == INT_TYPE)
            {
                current_expr_type = INT_TYPE;
            }
            else if(op1->type == DOUBLE_TYPE && op2->type == DOUBLE_TYPE )
            {
                current_expr_type = DOUBLE_TYPE;
            }
            else if(op1->lxtoken.lexeme_type != ID || op2->lxtoken.lexeme_type != ID)
            {
                if(op1->type == INT_TYPE && op1->lxtoken.lexeme_type == INT_LIT && op2->type == DOUBLE_TYPE)
                {
                    current_expr_type = DOUBLE_TYPE;
                    //generator
                    printf("INT2FLOAT %s %s\n", op1->generator_tmp_name, op1->generator_tmp_name);
                    //generator end
                }
                else if(op1->type == DOUBLE_TYPE && op2->type == INT_TYPE && op2->lxtoken.lexeme_type == INT_LIT)
                {
                    current_expr_type = DOUBLE_TYPE;
                    //generator
                    printf("INT2FLOAT %s %s\n", op2->generator_tmp_name, op2->generator_tmp_name);
                    //generator end
                }
                else
                {
                    analysis_error = TYPE_COMPAT_ERROR;
                    return false;
                }
            }
            else
            {
                analysis_error = TYPE_COMPAT_ERROR;
                return false;
            }
            
            
            //generator
            //             0   1   2     3      4      5       6
            char *ops[] = {"", "", "", "MUL", "DIV", "PLUS", "MINUS"};
            addr3op(ops[rule_index], generator_temp_res_name, op1->generator_tmp_name, op2->generator_tmp_name);
            //generator end
            
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
                current_expr_type = BOOL_TYPE;
            }
            else if((op1->type == NIL_TYPE || op1->type == NINT_NIL_TYPE || op1->type == NINT_TYPE) &&
                    (op2->type == NIL_TYPE || op2->type == NINT_NIL_TYPE  || op2->type == NINT_TYPE))//cannot compare Int and Int?
            {
                current_expr_type = BOOL_TYPE;
            }
            else if(op1->type == DOUBLE_TYPE && op2->type == DOUBLE_TYPE)
            {
                current_expr_type = BOOL_TYPE;
            }
            else if((op1->type == NIL_TYPE || op1->type == NDOUBLE_NIL_TYPE || op1->type == NDOUBLE_TYPE) &&
                    (op2->type == NIL_TYPE || op2->type == NDOUBLE_NIL_TYPE  || op2->type == NDOUBLE_TYPE))//cannot compare Double and Double?
            {
                current_expr_type = BOOL_TYPE;
            }
            else if(op1->type == INT_TYPE && op1->lxtoken.lexeme_type == INT_LIT && op2->type == DOUBLE_TYPE)//conversion  with literal only
            {
                //generator
                printf("INT2FLOAT %s %s\n", op1->generator_tmp_name, op1->generator_tmp_name);
                //generator end
                current_expr_type = BOOL_TYPE;
            }
            else if(op1->type == DOUBLE_TYPE && op2->type == INT_TYPE && op2->lxtoken.lexeme_type == INT_LIT)//conversion  with literal only
            {
                current_expr_type = BOOL_TYPE;
                //generator
                printf("INT2FLOAT %s %s\n", op2->generator_tmp_name, op2->generator_tmp_name);
                //generator end
            }
            else if(op1->type == STRING_TYPE && op2->type == STRING_TYPE)
            {
                current_expr_type = BOOL_TYPE;
            }
            else if((op1->type == NIL_TYPE || op1->type == NSTRING_NIL_TYPE || op1->type == NSTRING_TYPE) &&
                    (op2->type == NIL_TYPE || op2->type == NSTRING_NIL_TYPE || op2->type == NSTRING_TYPE))//cannot compare String and String?
            {
                current_expr_type = BOOL_TYPE;
            }
            else
            {
                analysis_error = TYPE_COMPAT_ERROR;
                return false;
            }
    
            //generator
            //             0   1   2   3   4   5    6   7      8
            char *ops[] = {"", "", "", "", "", "", "", "EQ", "NEQ"};
            generator_temp_res_name = generate_expr_var_name();
            addr3op(ops[rule_index], generator_temp_res_name, op1->generator_tmp_name, op2->generator_tmp_name);
            //generator end
            
            
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
    
            if(op1->type == INT_TYPE && op1->lxtoken.lexeme_type == INT_LIT && op2->type == DOUBLE_TYPE)//conversion  with literal only
            {
                //generator
                printf("INT2FLOAT %s %s\n", op1->generator_tmp_name, op1->generator_tmp_name);
                //generator end
                current_expr_type = BOOL_TYPE;
            }
            else if(op1->type == DOUBLE_TYPE && op2->type == INT_TYPE && op2->lxtoken.lexeme_type == INT_LIT)//conversion  with literal only
            {
                current_expr_type = BOOL_TYPE;
                //generator
                printf("INT2FLOAT %s %s\n", op2->generator_tmp_name, op2->generator_tmp_name);
                //generator end
            }
            else
            if(op2->type != op1->type)
            {
                analysis_error = TYPE_COMPAT_ERROR;
                return false;
            }
            current_expr_type = BOOL_TYPE;
    
            //generator
            //             0   1   2   3   4   5    6   7  8    9     10    11     12
            char *ops[] = {"", "", "", "", "", "", "", "", "", "LT", "GT", "LTE", "GTE"};
            generator_temp_res_name = generate_expr_var_name();
            addr3op(ops[rule_index], generator_temp_res_name, op1->generator_tmp_name, op2->generator_tmp_name);
            //generator end
    
    
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
    
            //generator
            generator_temp_res_name = generate_expr_var_name();
            addr3op("QQ", generator_temp_res_name, op1->generator_tmp_name, op2->generator_tmp_name);
            //generator end
    

    
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


bool resolveUndefinedFunctions(Stack *undefs, symtb_token defined)
{
    stackResetSemiPop(undefs);
    symtb_token *undef = stackSemiPop(undefs);
    while(undef != NULL)
    {
        if(0 == strcmp(undef->id_name, defined.id_name))
        {
            #ifdef SEM_DBG
                  printf("Undefined function found\n");
            #endif
            
            if(!compareUndefinedFunction(defined, *undef))
            {
                return false;
            }
            else
            {
                #ifdef SEM_DBG
                    printf("Undefined function resolved\n");
                #endif
                stackResetSemiPop(undefs);
                return true;
            }
        }
        undef = stackSemiPop(undefs);
    }
    
    
    stackResetSemiPop(undefs);
    return true;
}

void addFuncVarsToTable(symtb_token func, symtable *stb)
{
    if(func.funcArgsSize == 0)
        return;
    
    symtb_token arg;
    initSymtbToken(&arg);
    for(int i = 0; i < func.funcArgsSize; i++)
    {
        if(0 == strcmp(func.funcLocalArgnames[i], "_"))
            continue;
    
        arg.initialized = true;
        arg.type = CONSTANT;
        symtbTokenCopyName2(&arg, func.funcLocalArgnames[i]);
        arg.lit_type = func.funcArgTypes[i];
    
        symtb_insert(stb, arg.id_name, arg);
        
        clearSymtbToken(&arg);
    }
}


void addWrite()
{
    symtb_token func;
    initSymtbToken(&func);
    func.lit_type = VOID_TYPE;
    func.type = FUNCTION;
    func.initialized = true;
    symtbTokenCopyName2(&func, "write");
    symtb_insert(&global_table, "write", func);
    clearSymtbToken(&func);
}

void addord()
{
    symtb_token func;
    initSymtbToken(&func);
    func.lit_type = INT_TYPE;
    func.type = FUNCTION;
    func.initialized = true;
    symtbTokenCopyName2(&func, "ord");
    
    symtbTokenAddArgType3(&func, STRING_TYPE);
    symtbTokenAddLocalArgName2(&func, "c");
    symtbTokenAddArgName2(&func, "_");
    
    symtb_insert(&global_table, "ord", func);
    clearSymtbToken(&func);
}

void addchr()
{
    symtb_token func;
    initSymtbToken(&func);
    func.lit_type = STRING_TYPE;
    func.type = FUNCTION;
    func.initialized = true;
    symtbTokenCopyName2(&func, "chr");
    
    symtbTokenAddArgType3(&func, INT_TYPE);
    symtbTokenAddLocalArgName2(&func, "i");
    symtbTokenAddArgName2(&func, "_");
    
    symtb_insert(&global_table, "chr", func);
    clearSymtbToken(&func);
}


void addSubstring()
{
    symtb_token func;
    initSymtbToken(&func);
    func.lit_type = NSTRING_TYPE;
    func.type = FUNCTION;
    func.initialized = true;
    symtbTokenCopyName2(&func, "substring");
    
    symtbTokenAddArgType3(&func, STRING_TYPE);
    symtbTokenAddLocalArgName2(&func, "s");
    symtbTokenAddArgName2(&func, "of");
    
    symtbTokenAddArgType3(&func, INT_TYPE);
    symtbTokenAddLocalArgName2(&func, "i");
    symtbTokenAddArgName2(&func, "startingAt");
    
    symtbTokenAddArgType3(&func, INT_TYPE);
    symtbTokenAddLocalArgName2(&func, "j");
    symtbTokenAddArgName2(&func, "endingBefore");
    
    symtb_insert(&global_table, "substring", func);
    clearSymtbToken(&func);
}

void addLength()
{
    symtb_token func;
    initSymtbToken(&func);
    func.lit_type = INT_TYPE;
    func.type = FUNCTION;
    func.initialized = true;
    symtbTokenCopyName2(&func, "length");
    
    symtbTokenAddArgType3(&func, STRING_TYPE);
    symtbTokenAddLocalArgName2(&func, "s");
    symtbTokenAddArgName2(&func, "_");
    
    symtb_insert(&global_table, "length", func);
    clearSymtbToken(&func);
}


void addInt2Double()
{
    symtb_token func;
    initSymtbToken(&func);
    func.lit_type = DOUBLE_TYPE;
    func.type = FUNCTION;
    func.initialized = true;
    symtbTokenCopyName2(&func, "Int2Double");
    
    symtbTokenAddArgType3(&func, INT_TYPE);
    symtbTokenAddLocalArgName2(&func, "term");
    symtbTokenAddArgName2(&func, "_");
    
    symtb_insert(&global_table, "Int2Double", func);
    clearSymtbToken(&func);
}

void addDouble2Int()
{
    symtb_token func;
    initSymtbToken(&func);
    func.lit_type = INT_TYPE;
    func.type = FUNCTION;
    func.initialized = true;
    symtbTokenCopyName2(&func, "Double2Int");
    
    symtbTokenAddArgType3(&func, DOUBLE_TYPE);
    symtbTokenAddLocalArgName2(&func, "term");
    symtbTokenAddArgName2(&func, "_");
    
    symtb_insert(&global_table, "Double2Int", func);
    clearSymtbToken(&func);
}

void addreadString()
{
    symtb_token func;
    initSymtbToken(&func);
    func.lit_type = NSTRING_TYPE;
    func.type = FUNCTION;
    func.initialized = true;
    symtbTokenCopyName2(&func, "readString");
    symtb_insert(&global_table, "readString", func);
    clearSymtbToken(&func);
}

void addreadInt()
{
    symtb_token func;
    initSymtbToken(&func);
    func.lit_type = NINT_TYPE;
    func.type = FUNCTION;
    func.initialized = true;
    symtbTokenCopyName2(&func, "readInt");
    symtb_insert(&global_table, "readInt", func);
    clearSymtbToken(&func);
}


void addreadDouble()
{
    symtb_token func;
    initSymtbToken(&func);
    func.lit_type = NDOUBLE_TYPE;
    func.type = FUNCTION;
    func.initialized = true;
    symtbTokenCopyName2(&func, "readDouble");
    symtb_insert(&global_table, "readDouble", func);
    clearSymtbToken(&func);
}












