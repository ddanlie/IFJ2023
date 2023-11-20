#include "syntax.h"

FILE *readfile;

ret_t analysis_error;

lex_token current_lex_token;
lex_token previous_lex_token;
symtb_token current_symtb_token;
symtb_token current_called_function;
symtb_token current_defined_function;
Stack *undefined_functions;
symtable global_table;
Stack *local_tables;
symtable temporary_table;
int current_local_level;
lexeme current_expr_lexeme;
Stack *add_later_stack;
literal_type current_expr_type;
char *generator_temp_res_name;
char *generator_expr_res_name;
bool expr_is_literal;

void blockStuffBefore();
void blockStuffAfter();
void iterBlockStuffBefore(char *lbl);
bool GLOBAL_COMMAND_LIST();
bool LOCAL_COMMAND_LIST();
bool FUNC_COMMAND_LIST();
bool BLOCK();
bool LOCAL_COMMAND();

#define RULE_BOX 5
#define RULE_PADDING ELSE
const int rules_count = 14;
//first lexeme - expanding non terminal (example E -> (E) is {ID1, LBR1, ID1, RBR1}
//RULE_PADDING is a breakpoint and extra padding for table to have same amount of elements in each rule
//EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID,  LBR1, RBR1, UNDEF
//rules have to be reversed for work with stack
lexeme expr_rule_table[][RULE_BOX] = {
        {ID1, RBR1, ID1, LBR1,  RULE_PADDING},//0
        {ID1, ID,               RULE_PADDING, RULE_PADDING, RULE_PADDING},//1
        {ID1, EXCLAM, ID1,      RULE_PADDING, RULE_PADDING},//2
        {ID1, ID1, MUL, ID1,    RULE_PADDING},//3
        {ID1, ID1, DIV, ID1,    RULE_PADDING},//4
        {ID1, ID1, PLUS, ID1,   RULE_PADDING},//5
        {ID1, ID1, MINUS, ID1,  RULE_PADDING},//6
        {ID1, ID1, EQ, ID1,     RULE_PADDING},//7
        {ID1, ID1, NEQ, ID1,    RULE_PADDING},//8
        {ID1, ID1, LE, ID1,     RULE_PADDING},//9
        {ID1, ID1, GT, ID1,     RULE_PADDING},//10
        {ID1, ID1, LEQ, ID1,    RULE_PADDING},//11
        {ID1, ID1, GEQ, ID1,    RULE_PADDING},//12
        {ID1, ID1, QQ, ID1,     RULE_PADDING}//13
};

char precedence_table[LEXEMES_COUNT][LEXEMES_COUNT];

bool isExprLexeme(lexeme l)
{
    switch(l)
    {
        case EXCLAM: case MUL: case DIV: case PLUS:
        case MINUS: case EQ: case NEQ: case LE:
        case GT: case LEQ: case GEQ: case QQ:
        case ID: case LBR1: case RBR1: case UNDEF:
        {
            return true;
        }
        default:
            return false;
    }
}




expr_lexeme top(Stack *st)
{
    stackResetSemiPop(st);
    expr_lexeme *elem = stackSemiPop(st);
    while(st->semiPopCounter != 0)
    {
        if(isExprLexeme(elem->exp_lex))//exp_lex can be ELSE (which means there is cpecChar '<' or '>') or ID1 which is non-terminal
        {
            stackResetSemiPop(st);
            return *elem;
        }
        elem = stackSemiPop(st);
    }
}

void correct_current_lexeme(lex_token incorrect_tkn)
{
    switch(incorrect_tkn.lexeme_type)
    {
        case INT_LIT:
        case STRING_LIT:
        case DOUBLE_LIT:
        case NIL:
            current_expr_lexeme = ID;
            break;
        default:
            current_expr_lexeme = isExprLexeme(incorrect_tkn.lexeme_type) ? incorrect_tkn.lexeme_type : UNDEF;
            break;
    }
}
//if lexeme is not in grammar change it to UNDEF '$'
//careful! ID can be INT_LIT,STRING_LIT or DOUBLE_LIT. Control it before semantic check
void expr_read_move()
{
    read_move();
    correct_current_lexeme(current_lex_token);
}

void exprStackPushElem(Stack *expr_stack, const lex_token *lxtoken, lexeme expr_lex, char specChar, literal_type type, char *generator_tmp_name)
{
    lex_token elem;
    clearLexToken(&elem);
    initLexToken(&elem);
    if(lxtoken != NULL)//if it is NULL - push dummy instead of lex_token
        copyLexToken(*lxtoken, &elem);
    stackPush(expr_stack, &(expr_lexeme){elem, expr_lex, specChar, type, generator_tmp_name});
}

void exprStackDestroy(Stack *expr_stack)
{
    expr_lexeme *el = stackPop(expr_stack);
    while(el != NULL)
    {
        freeLexToken(&el->lxtoken);
        if(el->generator_tmp_name != NULL)
        {
            free(el->generator_tmp_name);
            el->generator_tmp_name = NULL;
        }
        free(el);
        el = stackPop(expr_stack);
    }
    stackDestroy(expr_stack);
}

void printExprStack(Stack *st)
{
    printf("\nExpr Stack: ");
    printf("\n-----------\n");
    stackResetSemiPop(st);
    expr_lexeme *elem = stackSemiPop(st);
    while(st->semiPopCounter != 0)
    {
        printf("(%c)\n", elem->specChar);
        printLexeme(elem->exp_lex);
        printf("\n");
        //printLexToken(elem->lxtoken);
        elem = stackSemiPop(st);
    }
    printf("\n-----------\n");
}
//read first token of expr before use!
//expressions end up reading next lexeme token
//In this grammar ID1 '_' is considered as non-terminal 'E', and UNDEF is considered as '$'
//ELSE lexeme is used to show that lexeme does not make sense (actually it can be any out-of-grammar character)
//careful! ID can be INT_LIT,STRING_LIT or DOUBLE_LIT. Control it before semantic check
bool EXPR()
{
    expr_is_literal = true;
    generator_expr_res_name = NULL;
    generator_temp_res_name = NULL;
    current_expr_type = UNDEF_TYPE;
    
    expr_lexeme expr_lex_helper;
    
    Stack *expr_stack = stackInit(sizeof(expr_lexeme));
    exprStackPushElem(expr_stack, NULL, UNDEF, '\0', UNDEF_TYPE, NULL);//push $ on top
    
    if(previous_lex_token.lexeme_type == ID)//odd case when we don't know was it function call or expression. so we read token ahead
    {
        correct_current_lexeme(previous_lex_token);
        expr_lex_helper = top(expr_stack);//let a = top. a will be $ so according to the table $ < id.
        switch(precedence_table[expr_lex_helper.exp_lex][current_expr_lexeme])//table[a][b]
        {
            case le://'<'
            {
                exprStackPushElem(expr_stack, NULL, ELSE, le, UNDEF_TYPE, NULL);//change a to a<  //we know we are at the very bottom and a = '$'
                exprStackPushElem(expr_stack, &previous_lex_token, current_expr_lexeme, '\0', UNDEF_TYPE, generate_expr_var_name());//push b (b = ID)
                //we don't read next b as it is already in current_lex_token
                break;
            }
            default:
                exprStackDestroy(expr_stack);
                analysis_error = ANOTHER_ERROR;
                fprintf(stderr, "WE SHOULD NEVER BE HERE\n");
                return false;
        }
    }
    
    bool out = false;
    correct_current_lexeme(current_lex_token);
    do
    {
        expr_lex_helper = top(expr_stack);//let a = top
        //let b (aka current_expr_lexeme) = actual input character
        switch(precedence_table[expr_lex_helper.exp_lex][current_expr_lexeme])//table[a][b]
        {
            case eq://'='
            {
                exprStackPushElem(expr_stack, &current_lex_token, current_expr_lexeme, '\0', UNDEF_TYPE, generate_expr_var_name());//push b
                expr_read_move();//read next b
                break;
            }
            case le://'<'
            {
                Stack *tmp_st = stackInit(sizeof(expr_lexeme*));//copy 'expr_lexeme' pointers to this stack before changing a to a<
                expr_lexeme *elem = stackPop(expr_stack);
                while(elem->exp_lex != expr_lex_helper.exp_lex)//first lexeme we will find is 'a' because it is read from 'top'
                {
                    stackPush(tmp_st, &elem);
                    elem = stackPop(expr_stack);
                }
                //change a to a<
                exprStackPushElem(expr_stack, &(elem->lxtoken), elem->exp_lex, '\0', UNDEF_TYPE, elem->generator_tmp_name);//push a
                freeLexToken(&elem->lxtoken);
                free(elem);
                exprStackPushElem(expr_stack, NULL, ELSE, le, UNDEF_TYPE, NULL);//push <
                //return characters back
                expr_lexeme **expr_lex_helper_3 = stackPop(tmp_st);//we got pointer to pointer
                while(expr_lex_helper_3 != NULL)
                {
                    exprStackPushElem(expr_stack, &((*expr_lex_helper_3)->lxtoken),
                                      (*expr_lex_helper_3)->exp_lex, '\0', (*expr_lex_helper_3)->type, (*expr_lex_helper_3)->generator_tmp_name);
                    freeLexToken(&(*expr_lex_helper_3)->lxtoken);//we have to delete it because it is copied inside the push function.
                    free(*expr_lex_helper_3);
                    free(expr_lex_helper_3);
                    expr_lex_helper_3 = stackPop(tmp_st);
                }
                stackDestroy(tmp_st);
    
                exprStackPushElem(expr_stack, &current_lex_token, current_expr_lexeme, '\0', UNDEF_TYPE, generate_expr_var_name());//push b
                expr_read_move();//read next b
                break;
            }
            case gr://'>'
            {
                int rule_idx = -1;
                for(int i = 0; i < rules_count; i++)
                {
                    stackResetSemiPop(expr_stack);
                    expr_lexeme *expr_lex_helper_2 = stackSemiPop(expr_stack);
                    rule_idx = i;
                    int j = 1;
                    while(expr_lex_helper_2->specChar != le)
                    {
                        if(expr_rule_table[i][j] != expr_lex_helper_2->exp_lex)
                        {
                            rule_idx = -1;
                            break;
                        }
                        j++;
                        expr_lex_helper_2 = stackSemiPop(expr_stack);
                    }
                    
                    if(rule_idx != -1)
                        break;
                    
                }
                
                if(rule_idx == -1)
                {
                    analysis_error = SYNTAX_ERROR;
                    exprStackDestroy(expr_stack);
                    return false;
                }
    
                //semantic
                if(!ruleTypeCheck(rule_idx, expr_stack))//we get 'current_expr_type' from here
                {
                    exprStackDestroy(expr_stack);
                    return false;
                }
                //semantic end
                
                expr_lexeme *expr_lex_helper_3;
                expr_lexeme *tmp;
                int i = 1;
                while(expr_rule_table[rule_idx][i] != RULE_PADDING)//change '<y' to 'A' from 'A -> y' rule
                {
                    i++;
                    expr_lex_helper_3 = stackPop(expr_stack);
                    if(expr_rule_table[rule_idx][i] == RULE_PADDING)
                    {
                        tmp = expr_lex_helper_3;//we need to save the last token for the case it was ID1 -> ID rule. So later we know what type of literal ID was
                    }
                    else
                    {
                        freeLexToken(&(expr_lex_helper_3->lxtoken));
                        if(expr_lex_helper_3->generator_tmp_name != NULL)
                        {
                            free(expr_lex_helper_3->generator_tmp_name);
                            expr_lex_helper_3->generator_tmp_name = NULL;
                        }
                        free(expr_lex_helper_3);
                    }
                    
                }
                expr_lex_helper_3 = stackPop(expr_stack);//pop '<' sign
                freeLexToken(&(expr_lex_helper_3->lxtoken));
                if(expr_lex_helper_3->generator_tmp_name != NULL)
                {
                    free(expr_lex_helper_3->generator_tmp_name);
                    expr_lex_helper_3->generator_tmp_name = NULL;
                }
                free(expr_lex_helper_3);
                
                exprStackPushElem(expr_stack, &tmp->lxtoken, expr_rule_table[rule_idx][0], '\0', current_expr_type, generator_temp_res_name);//push 'A'  from 'A -> y'
    
                freeLexToken(&tmp->lxtoken);
                if(tmp->generator_tmp_name != NULL)
                {
                    free(tmp->generator_tmp_name);
                    tmp->generator_tmp_name = NULL;
                }
                free(tmp);
                break;
            }
            default:
            {
                if(current_expr_lexeme == UNDEF)
                {
                    out = true;
                }
                else if(eoln_flag)//go finish rule wrap with >
                {
                    current_expr_lexeme = UNDEF;
                }
                else
                {
                    exprStackDestroy(expr_stack);
                    analysis_error = SYNTAX_ERROR;
                    return false;
                }
            }
        }
#ifdef SYNTAX_DBG
    printExprStack(expr_stack);
#endif
    }while(!out);
    
    bool itsok = true;
    stackResetSemiPop(expr_stack);
    expr_lexeme *expr_lex_helper_2 = stackSemiPop(expr_stack);
    if(expr_lex_helper_2->exp_lex != ID1)
        itsok = false;
    //generator
    generator_expr_res_name = "GF@___$expr_res$___";
    move(generator_expr_res_name, generator_temp_res_name);
//    char *s = generator_temp_res_name;
//    generator_temp_res_name = malloc(sizeof(char)*(strlen(s)+1));
//    strcpy(generator_temp_res_name, s);
    //generator end
    
    
    expr_lex_helper_2 = stackSemiPop(expr_stack);
    if(expr_lex_helper_2->exp_lex != UNDEF)
        itsok = false;
    exprStackDestroy(expr_stack);
    
    return itsok;
}

bool TERM()
{
    switch(current_lex_token.lexeme_type)
    {
        case INT_LIT:
        case DOUBLE_LIT:
        case STRING_LIT:
        case NIL:
        case ID:
            return true;
        default:
            return false;
    }
}

//this rule contains unwanted behaviour it is modificated so that grammar works correctly
//problem - (id: id ...)  argname and argument are ids.
//solution - function calling this rule knows it is going to read extra token, so it will not call read_move() if needed
bool FCALL_PARAM_NAME()
{
    read_move();//external move to colon
    if(currLexTokenIs(COLON))//if colon - definitely argname. previous token contains id
    {
        symtbTokenAddArgName(&current_called_function, previous_lex_token);
        read_move();
        read_move();
    }
    else
    {
        //do nothing.
    }
    
    return true;
}



bool FCALL_PARAM_LIST_2()
{
    if(currLexTokenIs(RBR1))
        return true;

    if(!currLexTokenIs(COMMA))
        return false;

    read_move();
    switch(current_lex_token.lexeme_type)
    {
        case ID:
        {
            //dummy
            lex_token tmp;
            clearLexToken(&tmp);
            initLexToken(&tmp);
            tmp.lexeme_type = ID1;
            addToStr(&tmp, '_');
            symtbTokenAddArgName(&current_called_function, tmp);//FCALL_PARAM_NAME() changes argname. if not - we have dummy.
            symtbTokenAddLocalArgName(&current_called_function, tmp);
            initLexToken(&tmp);
            //dummy end
            
            if(!FCALL_PARAM_NAME())//function reads extra lexeme because of grammar mistake
                return false;
            
            //FCALL_PARAM_NAME
            copyLexToken(current_lex_token, &tmp);
            copyLexToken(previous_lex_token, &current_lex_token); //set prev as curr
            //FCALL_PARAM_NAME end
    
            if(!TERM())//use prev as curr!
                return false;
    
            //find id and get type
            if(currLexTokenIs(ID))
            {
                symtb_token *tkn;
                if(!getFromEverywhere(current_lex_token.str.value,  &tkn, NULL) || !(tkn->initialized))
                {
                    //undefined variable
                    analysis_error = VAR_INIT_ERROR;
                    return false;
                }
                symtbTokenAddArgType2(&current_called_function, *tkn);
            }
            else
            {
                symtbTokenAddArgType(&current_called_function, current_lex_token);
            }
    
            //generator
            func_call_put_param(current_lex_token, current_called_function, false);
            //generator end
            
            
            //FCALL_PARAM_NAME
            copyLexToken(tmp, &current_lex_token);//stop using prev as curr
            freeLexToken(&tmp);
            //FCALL_PARAM_NAME end
            break;
        }
        case INT_LIT:
        case DOUBLE_LIT:
        case STRING_LIT:
        case NIL:
        {
            lex_token id1lexemeDummy;
            clearLexToken(&id1lexemeDummy);
            initLexToken(&id1lexemeDummy);
            id1lexemeDummy.lexeme_type = ID1;
            addToStr(&id1lexemeDummy, '_');
        
            symtbTokenAddArgName(&current_called_function, id1lexemeDummy);
            symtbTokenAddLocalArgName(&current_called_function, id1lexemeDummy);
        
            symtbTokenAddArgType(&current_called_function, current_lex_token);
        
            freeLexToken(&id1lexemeDummy);
    
            //generator
            func_call_put_param(current_lex_token, current_called_function, false);
            //generator end
            
            read_move();//see FCALL_PARAM_NAME(). we need to read extra lexeme
            break;
        }
        default:
            return false;
    }
    //read_move();
    return FCALL_PARAM_LIST_2();
}


bool FCALL_PARAM_LIST()
{
    if(currLexTokenIs(RBR1))//zero arguments
        return true;
    
    switch(current_lex_token.lexeme_type)
    {
        case ID:
        {
            //dummy
            lex_token tmp;
            clearLexToken(&tmp);
            initLexToken(&tmp);
            tmp.lexeme_type = ID1;
            addToStr(&tmp, '_');
            symtbTokenAddLocalArgName(&current_called_function, tmp);
            symtbTokenAddArgName(&current_called_function, tmp);//FCALL_PARAM_NAME() changes argname. if not - we have dummy.
            initLexToken(&tmp);
            //dummy end
            if(!FCALL_PARAM_NAME())
                return false;
    
            //FCALL_PARAM_NAME
            copyLexToken(current_lex_token, &tmp);
            copyLexToken(previous_lex_token, &current_lex_token); //set prev as curr
            //FCALL_PARAM_NAME end

            if(!TERM())//use prev as curr
                return false;
    
            if(currLexTokenIs(ID))
            {
                symtb_token *tkn;
                if(!getFromEverywhere(current_lex_token.str.value,  &tkn, NULL) || !(tkn->initialized))
                {
                    //undefined variable
                    analysis_error = VAR_INIT_ERROR;
                    return false;
                }
                symtbTokenAddArgType2(&current_called_function, *tkn);
            }
            else
            {
                symtbTokenAddArgType(&current_called_function, current_lex_token);
            }
    
            //generator
            func_call_put_param(current_lex_token, current_called_function, false);
            //generator end
            
            //FCALL_PARAM_NAME
            copyLexToken(tmp, &current_lex_token);//stop using prev as curr
            freeLexToken(&tmp);
            //FCALL_PARAM_NAME end
    

    
    
    
            break;
        }
        case INT_LIT:
        case DOUBLE_LIT:
        case STRING_LIT:
        case NIL:
        {
            lex_token id1lexemeDummy;
            clearLexToken(&id1lexemeDummy);
            initLexToken(&id1lexemeDummy);
            id1lexemeDummy.lexeme_type = ID1;
            addToStr(&id1lexemeDummy, '_');
            
            symtbTokenAddArgName(&current_called_function, id1lexemeDummy);
            symtbTokenAddLocalArgName(&current_called_function, id1lexemeDummy);
            
            symtbTokenAddArgType(&current_called_function, current_lex_token);
    
            freeLexToken(&id1lexemeDummy);
    
            //generator
            func_call_put_param(current_lex_token, current_called_function, false);
            //generator end
            
            
            read_move();//see FCALL_PARAM_NAME(). we need to read extra lexeme
            break;
        }
        default:
            return false;
    }
    //read_move();//read ',' or ')' or another
    if(!FCALL_PARAM_LIST_2())
        return false;
    
    return true;
}

bool FUNC_CALL()
{
    //semantic
    clearSymtbToken(&current_called_function);
    current_called_function.type = FUNCTION;
    symtbTokenCopyName(&current_called_function, previous_lex_token);//previous = 'ID'  current = '('
    symtb_token *found_function;
    current_called_function.initialized = true;
    if(!getFromGlobalTable(previous_lex_token.str.value, &found_function))//if function with ID is not found
        current_called_function.initialized = false;
    //semantic end
    
    //generator
    lex_token tmp;
    clearLexToken(&tmp);
    initLexToken(&tmp);
    copyLexToken(previous_lex_token, &tmp);
    //generator end
    
    
    read_move();
    if(!FCALL_PARAM_LIST())
        return false;
    if(!currLexTokenIs(RBR1))
        return false;
    read_move();
    
    //generator
    printf("CALL %s\n", tmp.str.value);
    clearLexToken(&tmp);
    
    
    func_call_put_param(current_lex_token, current_symtb_token, true);
    //generator end
    
    
    
    //semantic
    if(current_called_function.initialized)
    {
        current_called_function.lit_type = found_function->lit_type;
        if(!compareFunctionsSignature(*found_function, current_called_function))
            return false;
    }
    else
    {
        if(current_symtb_token.lit_type == UNDEF)//it was just function call f(...)
            current_called_function.lit_type = VOID_TYPE;
        else//it is ID = f(...)
            current_called_function.lit_type = current_symtb_token.lit_type;
        
        symtb_token topush;
        initSymtbToken(&topush);
        copySymtbToken(&topush, current_called_function);
        stackPush(undefined_functions, &topush);
    }
    //semantic end
    
    
    
    return true;
}

bool ID_ASSIGN()
{
    if(currLexTokenIs(LBR1))
    {
        if(!FUNC_CALL())
            return false;
    }
    else
        if(!EXPR())
            return false;
    return true;
}

bool ASSIGN()
{
    if(currLexTokenIs(ID))
    {
        read_move();
        if(!ID_ASSIGN())
            return false;
    }
    else
        if(!EXPR())
            return false;
        
    return true;
}

bool OPT_VAR_EXPR()
{
    return ASSIGN();
}

bool TYPE()
{
    switch(current_lex_token.lexeme_type)
    {
        case INT:
        case DOUBLE:
        case STRING:
        case NILINT:
        case NILDOUBLE:
        case NILSTRING:
            return true;
        default:
            return false;
    }
}


bool OPT_VAR_TYPE()
{
    return TYPE();
}


bool VAR_ENTITY()
{
    if(currLexTokenIs(VAR))
    {
        //semantic
        current_symtb_token.type = VARIABLE;
        //semantic end
        return true;
    }
    else
    if(currLexTokenIs(LET))
    {
        //semantic
        current_symtb_token.type = CONSTANT;
        //semantic end
        return true;
    }
    else
        return false;
}

// 'let/var' 'id' ': type/e' '= ASSIGN/e'
bool VARDEF()
{
    clearSymtbToken(&current_symtb_token);
    
    if(!VAR_ENTITY())
        return false;
    read_move();//read ID after LET/VAR
    if(!currLexTokenIs(ID))
        return false;
    
    //semantic
    if(varRedefinition(current_lex_token.str.value))
        return false;
    symtbTokenCopyName(&current_symtb_token, current_lex_token);
    //semantic end
    
    bool at_least_one_flag = false;
    read_move();//read ':' or '=' or another lexeme
    if(currLexTokenIs(COLON))
    {
        at_least_one_flag = true;
        read_move();
        if(!OPT_VAR_TYPE())
            return false;
        
        //semantic
        symtbTokenSetTypefromKw(&current_symtb_token, current_lex_token);
        //semantic end
        
        read_move();//read '=' or another lexeme
    }
    
    if(currLexTokenIs(AS))
    {
        //semantic
        clearSymtbToken(&current_called_function);
        //semantic end
        read_move();
        if(!OPT_VAR_EXPR())
            return false;
        
        //semantic
        current_symtb_token.initialized = true;
        //semantic end
        
        
        at_least_one_flag = true;
        
    }
    
    if(!at_least_one_flag)
        return false;
    
    //semantic
    if(!vardefCompareTypeExpr(&current_symtb_token, current_expr_type))
        if(!vardefCompareTypeExpr(&current_symtb_token, current_called_function.lit_type))
            return false;
    
    
    addVarToFrame(current_symtb_token);
    
    //generator
    char *varname = get_var_name(current_symtb_token.id_name);
    defvar(varname);
    if(generator_expr_res_name != NULL && current_expr_type != BOOL_TYPE && current_expr_type != UNDEF_TYPE)
    {
        move(varname, generator_expr_res_name);
        generator_expr_res_name = NULL;
    }
    else if(current_called_function.lit_type != UNDEF_TYPE)
    {
        move(varname, "GF@%retval");
    }
    free(varname);
    //generator end
    
    clearSymtbToken(&current_symtb_token);
    //semantic end
    
    return true;
}

bool PARAM_ID()
{
    bool res =  currLexTokenIs(ID) || currLexTokenIs(ID1);
    //semantic
    if(res)
        symtbTokenAddArgName(&current_defined_function, current_lex_token);
    //semantic end
    return res;
}


bool PARAM_LIST_2()
{
    if(currLexTokenIs(RBR1))
        return true;
    if(!currLexTokenIs(COMMA))
        return false;
    read_move();
    if(!PARAM_ID())
        return false;
    read_move();
    if(!currLexTokenIs(ID))
        return false;
    //semantic
    symtbTokenAddLocalArgName(&current_defined_function, current_lex_token);
    //semantic end
    read_move();
    if(!currLexTokenIs(COLON))
        return false;
    read_move();
    if(!TYPE())
        return false;
    //semantic
    symtbTokenAddArgType(&current_defined_function, current_lex_token);
    //semantic end
    read_move();
    if(!PARAM_LIST_2())
        return false;
    
    return true;
}

bool PARAM_LIST()
{
    if(currLexTokenIs(RBR1))
        return true;
    if(!PARAM_ID())
        return false;
    read_move();
    if(!currLexTokenIs(ID))
        return false;
    //semantic
    symtbTokenAddLocalArgName(&current_defined_function, current_lex_token);
    //semantic end
    read_move();
    if(!currLexTokenIs(COLON))
        return false;
    read_move();
    if(!TYPE())
        return false;
    //semantic
    symtbTokenAddArgType(&current_defined_function, current_lex_token);
    //semantic end
    read_move();
    if(!PARAM_LIST_2())
        return false;
    
    return true;
}

bool RET_EXPR()
{
    if(EXPR())
    {
        //generator
        move("GF@%retval", generator_expr_res_name);
        generator_expr_res_name = NULL;
        //generator end
        
        
        //semantic. check if expr type corresponds to return type
        if(current_defined_function.lit_type == VOID_TYPE)
        {
            analysis_error = RET_VAL_ERROR;
            return false;
        }
        
        if(current_defined_function.lit_type == NINT_TYPE
            && (current_expr_type == INT_TYPE || current_expr_type == NINT_TYPE
                || current_expr_type == NINT_NIL_TYPE || current_expr_type == NIL_TYPE))
        {}
        else if(current_defined_function.lit_type == NDOUBLE_TYPE
                && (current_expr_type == DOUBLE_TYPE || current_expr_type == NDOUBLE_TYPE
                || current_expr_type == NDOUBLE_NIL_TYPE || current_expr_type == NIL_TYPE))
        {}
        else if(current_defined_function.lit_type == NSTRING_TYPE
                && (current_expr_type == STRING_TYPE || current_expr_type == NSTRING_TYPE
                    || current_expr_type == NSTRING_NIL_TYPE || current_expr_type == NIL_TYPE))
        {}
        else if(current_defined_function.lit_type == INT_TYPE
                && current_expr_type == INT_TYPE)
        {}
        else if(current_defined_function.lit_type == DOUBLE_TYPE
                && current_expr_type == DOUBLE_TYPE)
        {}
        else if(current_defined_function.lit_type == STRING_TYPE
                && current_expr_type == STRING_TYPE)
        {}
        else
        {
            analysis_error = RET_VAL_ERROR;
            return false;
        }
        //semantic end
        
    }
    else
    {
        //semantic. check if return type corresponds to void
        if(analysis_error != NO_ERROR)
            return false;
        if(current_defined_function.lit_type != VOID_TYPE)
        {
            analysis_error = RET_VAL_ERROR;
            return false;
        }
        //semantic end
    }
    
    //generator
    printf("RETURN\n");
    //generator end
    
    return true;
}

bool FUNC_COMMAND()
{
    switch(current_lex_token.lexeme_type)
    {
        case ID:
        case VAR:
        case IF:
        case LET:
        case WHILE:
            return LOCAL_COMMAND();
        case RETURN:
        {
            read_move();
            if(!RET_EXPR())
                return false;
            break;
        }
        default:
            return false;
    }
    
    return true;
}

bool FUNC_BLOCK()
{
    
    //generator
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    //generator ends
    
    
    //semantic
    //stackPush(local_tables, &temporary_table);//curr level is always = 0
    symtb_clear(temporary_table);
    temporary_table = symtb_init(SYMTABLE_INIT_SIZE);
    current_local_level += 1;
    temporary_table.local_level = current_local_level;
    
    
//    symtb_token *later = stackPop(add_later_stack);
//    while(later != NULL)
//    {
//        symtb_insert(&temporary_table, later->id_name, *later);
//        clearSymtbToken(later);
//        free(later);
//        later = stackPop(add_later_stack);
//    }
    
    addFuncVarsToTable(current_defined_function, &temporary_table);
    //semantic end
    
    //generator
    funcdef_define_temp_params(current_defined_function);
    //generator end
    
    
    if(!currLexTokenIs(LBR2))
        return false;
    read_move();
    if(!FUNC_COMMAND_LIST())
        return false;
    if(!currLexTokenIs(RBR2))
        return false;
    
    
    
   
    
    
    //semantic
    current_local_level -= 1;
    symtb_clear(temporary_table);
    temporary_table = symtb_init(SYMTABLE_INIT_SIZE);
    //semantic end
    
    //generator
    printf("POPFRAME\n");
    //generator end
    
    return true;
}

bool FUNC_COMMAND_LIST_2()
{
    if(currLexTokenIs(RBR2))
        return true;
    else if(eoln_flag)
    {
        if(!FUNC_COMMAND_LIST())
            return false;
    }
    else if(currLexTokenIs(LBR2))
    {
        read_move();
        if(!FUNC_BLOCK())
            return false;
        read_move();
        if(!FUNC_COMMAND_LIST())
            return false;
    }
    else
        return false;
    
    return true;
    
}

bool FUNC_COMMAND_LIST()
{
    if(currLexTokenIs(RBR2))
        return true;
    
    switch(current_lex_token.lexeme_type)
    {
        case ID:
        case VAR:
        case IF:
        case LET:
        case WHILE:
        case RETURN:
            if(!FUNC_COMMAND())
                return false;
            //all commands read one extra lexeme, no need for read_move()
            if(!FUNC_COMMAND_LIST_2())
                return false;
            break;
        case LBR2:
            read_move();
            if(!FUNC_BLOCK())
                return false;
            read_move();
            if(!FUNC_COMMAND_LIST_2())
                return false;
            break;
        default:
            return false;
    }
    return true;
}

bool RETURN_FORM()
{
    if(currLexTokenIs(RARROW))
    {
        read_move();
        if(!TYPE())
            return false;
        //semantic
        symtbTokenSetTypefromKw(&current_defined_function, current_lex_token);
        //semantic end
        read_move();
    }
    return true;
}

bool FUNCDEF()
{
    if(!currLexTokenIs(ID))
        return false;
    
    //generator
    lex_token func_name_lbl;
    clearLexToken(&func_name_lbl);
    initLexToken(&func_name_lbl);
    copyLexToken(current_lex_token, &func_name_lbl);
    //generator end
    
    
    //semantic
    if(funcRedefinition(current_lex_token.str.value))
        return false;
    
    clearSymtbToken(&current_defined_function);
    current_defined_function.type = FUNCTION;
    symtbTokenCopyName(&current_defined_function, current_lex_token);
    //semantic end
    
    read_move();
    if(currLexTokenIs(LBR1))
    {
        read_move();
        if(!PARAM_LIST())
            return false;
    }
    else
    {
        return false;
    }
    read_move();//read '->' or '{' or wrong lexeme
    if(!RETURN_FORM())
        return false;
    
    //semantic
    if(!resolveUndefinedFunctions(undefined_functions, current_defined_function))
        return false;
    addVarToFrame(current_defined_function);
    //semantic end
    
    
    //generator
    
    char *funcdefend = generate_label();
    printf("JUMP %s\n", funcdefend);
    printf("LABEL %s\n", func_name_lbl.str.value);
    freeLexToken(&func_name_lbl);
    //generator end
    
    if(!FUNC_BLOCK())
        return false;
    
    //generator
    printf("LABEL %s\n", funcdefend);
    free(funcdefend);
    //generator end
    
    
    //semantic
    clearSymtbToken(&current_defined_function);
    //semantic end
    read_move();
    return true;
}

bool VAR_ASSIGN()
{
    if(!ASSIGN())
        return false;
    return true;
}

bool ID_COMMAND()
{
    //previous lexeme has ID

    if(currLexTokenIs(AS))
    {
        //semantic
        clearSymtbToken(&current_symtb_token);
        symtb_token *foundvar;
        if(!getFromEverywhere(previous_lex_token.str.value, &foundvar, NULL))
        {
            analysis_error = VAR_INIT_ERROR;
            return false;
        }
        if(foundvar->type == CONSTANT)
        {
            analysis_error = SEMANTIC_OTHER_ERROR;
            return false;
        }
        copySymtbToken(&current_symtb_token, *foundvar);
    
        clearSymtbToken(&current_called_function);
        //semantic end
        
        read_move();
        if(!VAR_ASSIGN())
            return false;
        
        //semantic
        foundvar->initialized = true;
        if(current_called_function.type == UNDEF_ID)//it was the expression we assigned to variable
        {
            vardefCompareTypeExpr(&current_symtb_token, current_expr_type);//type is not copied from expression it is assignment command
            //generator
            char *varname = get_var_name(current_symtb_token.id_name);
            move(varname, generator_expr_res_name);
            generator_expr_res_name = NULL;
            free(varname);
            //generator end
        }
        else//it was function call
        {
            if(!compareIDtoFuncReturn(current_symtb_token, current_called_function))
                return false;
    
            //generator
            if(current_called_function.lit_type != VOID_TYPE)
            {
                char *varname = get_var_name(current_symtb_token.id_name);
                move(varname, "GF@%retval");
                free(varname);
            }
            //generator end
            
            
            clearSymtbToken(&current_called_function);
        }
        clearSymtbToken(&current_symtb_token);

        //semantic end
    }
    else if(currLexTokenIs(LBR1))
    {
        if(!FUNC_CALL())
            return false;
        //read_move();//command must read next lexeme
        
        
        //generator
        if(current_called_function.lit_type != VOID_TYPE)
        {
            if(current_symtb_token.id_name != NULL)
            {
                char *varname = get_var_name(current_symtb_token.id_name);
                move(varname, "GF@%retval");
                free(varname);
            }
        }
        //generator end
    
        //semantic
        clearSymtbToken(&current_called_function);
        clearSymtbToken(&current_symtb_token);
        //semantic end
    }
    else
        return false;
    
    return true;
}

bool BRANCH_CANDIDATE()
{
    if(currLexTokenIs(LET))
    {
        read_move();
        if(!currLexTokenIs(ID))
            return false;
        
        //semantic
        symtb_token *found;
        if(!getFromEverywhere(current_lex_token.str.value, &found, NULL))
        {
            analysis_error = VAR_INIT_ERROR;
            return false;
        }
        if(!found->initialized)
        {
            analysis_error = VAR_INIT_ERROR;
            return false;
        }
        //semantic end
        
        
        read_move();//read one more lexeme because EXPR() below reads one more too
    }
    else if(EXPR())
    {
        //semantic
        if(current_expr_type != BOOL_TYPE)
        {
            analysis_error = TYPE_COMPAT_ERROR;
            return false;
        }
        //semantic end
    }
    else
    {
        return false;
    }
    return true;
}

bool BRANCH()
{
    
    blockStuffBefore();
    
    if(!BRANCH_CANDIDATE())
        return false;
    
    
    //generator
    char *elseLabel = generate_label();
    if(generator_expr_res_name != NULL)
    {
        addr3op("NOT", generator_expr_res_name, generator_expr_res_name, NULL);
        printf("JUMPIFEQ %s %s %s\n", elseLabel, generator_expr_res_name, "bool@true");
        generator_expr_res_name = NULL;
    }
    //generator end
    
    
    //previous token сan be ID
    //semantic
    bool push_orig = false;
    symtb_token unnilled;
    symtb_token orig;
    if(prevLexTokenIs(ID))
    {
        push_orig = true;
        symtb_token *found;
        getFromEverywhere(previous_lex_token.str.value, &found, NULL);
        initSymtbToken(&orig);
        initSymtbToken(&unnilled);
        copySymtbToken(&unnilled, *found);
        copySymtbToken(&orig, *found);
        switch(found->lit_type)
        {
            case NINT_TYPE:
                unnilled.lit_type = INT_TYPE;
                break;
            case NSTRING_TYPE:
                unnilled.lit_type = STRING_TYPE;
                break;
            case NDOUBLE_TYPE:
                unnilled.lit_type = DOUBLE_TYPE;
                break;
            default:
                break;
        }
        stackPush(add_later_stack, &unnilled);
        
        //generator
        char *vname = get_var_name(previous_lex_token.str.value);
        printf("JUMPIFEQ %s %s %s\n", elseLabel, vname, "nil@nil");
        free(vname);
        //generator end
    }
    //semantic end
    
    if(!currLexTokenIs(LBR2))
        return false;
    read_move();
    if(!BLOCK())
        return false;
    read_move();
    if(!currLexTokenIs(ELSE))
        return false;
    
    
    //generator
    char *exitLabel = generate_label();
    jump(exitLabel);
    printf("LABEL %s\n", elseLabel);
    //generator end
    
    //semantic
    if(push_orig)//push inside else block
        stackPush(add_later_stack, &orig);
    //semantic end
    
    
    read_move();
    if(!currLexTokenIs(LBR2))
        return false;
    read_move();
    if(!BLOCK())
        return false;
    
    read_move();//command must read next lexeme
    
    //generator
    printf("LABEL %s\n", exitLabel);
    free(elseLabel);
    free(exitLabel);
    //generator end
    
    blockStuffAfter();
    
    return true;
}


bool ITERATION()
{
    //generator
    char *whilelbl = generate_label();
    char *exitlbl = generate_label();
    //iterBlockStuffBefore(whilelbl);
    blockStuffBefore();
    printf("LABEL %s\n", whilelbl);
    
    //generator end
    
    blockStuffBefore();
    if(!EXPR())
        return false;
    
    blockStuffAfter();
    
    if(current_expr_type != BOOL_TYPE)
    {
        analysis_error = TYPE_COMPAT_ERROR;
        return false;
    }
    
    //generator
    addr3op("NOT", generator_expr_res_name, generator_expr_res_name, NULL);
    printf("JUMPIFEQ %s %s %s\n", exitlbl, generator_expr_res_name, "bool@true");
    generator_expr_res_name = NULL;
    //generator end
    
    
    
    if(!currLexTokenIs(LBR2))
        return false;
    read_move();
    if(!BLOCK())
        return false;
    
    
    
    read_move();//command must read next lexeme
    
    
    //generator
    jump(whilelbl);
    printf("LABEL %s\n", exitlbl);
    free(whilelbl);
    free(exitlbl);
    blockStuffAfter();
    //generator end
    
    
    
    return true;
}

bool LOCAL_COMMAND()
{
    switch(current_lex_token.lexeme_type)
    {
        case ID:
            read_move();
            return ID_COMMAND();
        case LET:
        case VAR:
            return VARDEF();
        case IF:
            read_move();
            return BRANCH();
        case WHILE:
            read_move();
            return ITERATION();
        default:
            return false;
    }
}

bool GLOBAL_COMMAND()
{
    switch(current_lex_token.lexeme_type)
    {
        case ID:
        case VAR:
        case IF:
        case LET:
        case WHILE:
            return LOCAL_COMMAND();
        case FUNC:
            read_move();
            return FUNCDEF();
        default:
            return false;
    }
}


bool LOCAL_COMMAND_LIST_2()
{
    if(currLexTokenIs(RBR2))
        return true;
    else if(eoln_flag)
    {
        if(!LOCAL_COMMAND_LIST())
            return false;
    }
    else if(currLexTokenIs(LBR2))
    {
        read_move();
        if(!BLOCK())
            return false;
        read_move();
        if(!LOCAL_COMMAND_LIST())
            return false;
    }
    else
        return false;
    
    return true;
}


bool LOCAL_COMMAND_LIST()
{
    if(currLexTokenIs(RBR2))
        return true;
    switch(current_lex_token.lexeme_type)
    {
        case ID:
        case VAR:
        case IF:
        case LET:
        case WHILE:
            if(!LOCAL_COMMAND())
                return false;
            //all commands read one extra lexeme, no need for read_move()
            if(!LOCAL_COMMAND_LIST_2())
                return false;
            break;
        case LBR2:
            read_move();
            if(!BLOCK())
                return false;
            read_move();
            if(!LOCAL_COMMAND_LIST_2())
                return false;
            break;
        default:
            return false;
    }
    return true;
}


void iterBlockStuffBefore(char *lbl)
{
    if(current_local_level == 0)
    {
        symtb_clear(temporary_table);
        printf("LABEL %s\n", lbl);
    }
    else
    {
        stackPush(local_tables, &temporary_table);
        //generator
        pass_vars_to_global();
        printf("LABEL %s\n", lbl);
        printf("PUSHFRAME\n");
        printf("CREATEFRAME\n");
        //generator end
    }
    
    //semantic
    temporary_table = symtb_init(SYMTABLE_INIT_SIZE);
    current_local_level += 1;
    temporary_table.local_level = current_local_level;
    
    symtb_token *later = stackPop(add_later_stack);
    while(later != NULL)
    {
        //generator
        char *vname_before_insert = get_var_name(later->id_name);
        // printf("--------- vname before = %s\n ---------", vname_before_insert);
        //generator end
        
        symtb_insert(&temporary_table, later->id_name, *later);
        
        //generator
        char *vname = get_var_name(later->id_name);
        //printf("--------- vname after = %s\n---------", vname_before_insert);
        defvar(vname);
        move(vname, vname_before_insert);
        free(vname);
        free(vname_before_insert);
        //generator end
        
        clearSymtbToken(later);
        free(later);
        later = stackPop(add_later_stack);
    }
    //semantic end
}

void blockStuffBefore()
{
    if(current_local_level == 0)
    {
        symtb_clear(temporary_table);
    }
    else
    {
        stackPush(local_tables, &temporary_table);
        //generator
        pass_vars_to_global();
        printf("PUSHFRAME\n");
        printf("CREATEFRAME\n");
        //generator end
    }
    
    //semantic
    temporary_table = symtb_init(SYMTABLE_INIT_SIZE);
    current_local_level += 1;
    temporary_table.local_level = current_local_level;
    
    symtb_token *later = stackPop(add_later_stack);
    while(later != NULL)
    {
        //generator
        char *vname_before_insert = get_var_name(later->id_name);
        // printf("--------- vname before = %s\n ---------", vname_before_insert);
        //generator end
        
        symtb_insert(&temporary_table, later->id_name, *later);
        
        //generator
        char *vname = get_var_name(later->id_name);
        //printf("--------- vname after = %s\n---------", vname_before_insert);
        defvar(vname);
        move(vname, vname_before_insert);
        free(vname);
        free(vname_before_insert);
        //generator end
        
        clearSymtbToken(later);
        free(later);
        later = stackPop(add_later_stack);
    }
    //semantic end
}

void blockStuffAfter()
{
    //semantic
    current_local_level -= 1;
    symtb_clear(temporary_table);
    symtable *popped = stackPop(local_tables);
    if(popped != NULL)
        temporary_table = *popped;
    else
        temporary_table = symtb_init(SYMTABLE_INIT_SIZE);
    //semantic end
    
    //generator

    if(current_local_level == 0)
    {
        printf("CREATEFRAME\n");
        func_call_put_param(current_lex_token, current_symtb_token, true);
    }
    else
    {
        printf("POPFRAME\n");
        return_passed_vars();
    }
   
    //generator end
}



bool BLOCK()
{
    blockStuffBefore();
    
    if(!LOCAL_COMMAND_LIST())
        return false;
    if(!currLexTokenIs(RBR2))
        return false;
    
    blockStuffAfter();
    
    return true;
}


bool GLOBAL_COMMAND_LIST_2()
{
    if(end_of_file_flag)
        return true;
    else if(eoln_flag)
    {
        if(!GLOBAL_COMMAND_LIST())
            return false;
    }
    else if(currLexTokenIs(LBR2))
    {
        read_move();
        if(!BLOCK())
            return false;
        read_move();
        if(!GLOBAL_COMMAND_LIST())
            return false;
    }
    else
        return false;
    
    return true;
}

bool GLOBAL_COMMAND_LIST()
{
    if(end_of_file_flag)
        return true;
    switch(current_lex_token.lexeme_type)
    {
        case ID:
        case VAR:
        case IF:
        case LET:
        case WHILE:
        case FUNC:
            if(!GLOBAL_COMMAND())
                return false;
            //all commands read one extra lexeme, no need for read_move()
            if(!GLOBAL_COMMAND_LIST_2())
                return false;
            break;
        case LBR2:
            read_move();
            if(!BLOCK())
                return false;
            read_move();
            if(!GLOBAL_COMMAND_LIST_2())
                return false;
            break;
        default:
            return false;
    }
    return true;
}

bool PROGRAM()
{
    read_move();
    return GLOBAL_COMMAND_LIST();
}

bool currLexTokenIs(lexeme lexeme_value)
{
    return current_lex_token.lexeme_type == lexeme_value;
}

bool prevLexTokenIs(lexeme lexeme_value)
{
    return previous_lex_token.lexeme_type == lexeme_value;
}


void initPrecedenceTable()
{
    //our table is large and sparse because of lexemes that are not used.
    // This approach makes work easier as we use lexeme enum as indexes
    for(int i = 0; i < LEXEMES_COUNT; i++)
    {
        for(int j = 0; j < LEXEMES_COUNT; j++)
        {
            precedence_table[i][j] = '\0';
        }
    }
    
    //EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID, LBR1, RBR1, UNDEF
    //first lexeme - columnt index
    const int grtbrows = 14;
    const int grtbcols = 17;
    int grtb_colscnt[] = {14, 14, 14, 12, 12, 4, 4, 4, 4, 4, 4, 3, 15, 15};
    lexeme padding = ELSE;
    lexeme greater_table[14][17] = {
            { EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, RBR1, UNDEF},
            { MUL, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, RBR1, UNDEF},
            { DIV, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, RBR1, UNDEF},
            { PLUS, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, RBR1, UNDEF},
            { MINUS, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, RBR1, UNDEF},
            { EQ, QQ, RBR1, UNDEF},
            { NEQ, QQ, RBR1, UNDEF},
            { LE, QQ, RBR1, UNDEF},
            { GT, QQ, RBR1, UNDEF},
            { LEQ, QQ, RBR1, UNDEF},
            { GEQ, QQ, RBR1, UNDEF},
            { QQ, RBR1, UNDEF},
            { ID, EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, RBR1, UNDEF},
            { RBR1, EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, RBR1, UNDEF}
    };
    
    //EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID, LBR1, RBR1, UNDEF
    const int letbrows = 13;
    const int letbcols = 17;
    int letb_colscnt[] = {4, 4, 6, 6, 8, 8, 8, 8, 8, 8, 15, 15, 15};
    lexeme less_table[13][17] = {
            { MUL, EXCLAM, ID, LBR1},
            { DIV, EXCLAM, ID, LBR1},
            { PLUS, EXCLAM, MUL, DIV, ID, LBR1},
            { MINUS, EXCLAM, MUL, DIV, ID, LBR1},
            { EQ, EXCLAM, MUL, DIV, PLUS, MINUS, ID, LBR1},
            { NEQ, EXCLAM, MUL, DIV, PLUS, MINUS, ID, LBR1},
            { LE, EXCLAM, MUL, DIV, PLUS, MINUS, ID, LBR1},
            { GT, EXCLAM, MUL, DIV, PLUS, MINUS, ID, LBR1},
            { LEQ, EXCLAM, MUL, DIV, PLUS, MINUS, ID, LBR1},
            { GEQ, EXCLAM, MUL, DIV, PLUS, MINUS, ID, LBR1},
            { QQ, EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID, LBR1},
            { LBR1, EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID, LBR1},
            { UNDEF, EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID, LBR1}
    };
    
    
    //fill the table
    for(int i = 0; i < grtbrows; i++)
    {
        for(int j = 1; j < grtb_colscnt[i]; j++)
        {
            precedence_table[greater_table[i][0]][greater_table[i][j]] = gr;
        }
    }
    
    for(int i = 0; i < letbrows; i++)
    {
        for(int j = 1; j < letb_colscnt[i]; j++)
        {
            precedence_table[less_table[i][0]][less_table[i][j]] = le;
        }
    }
    
    precedence_table[LBR1][RBR1] = eq;
    precedence_table[RBR1][LBR1] = eq;
}

void read_move()
{
    copyLexToken(current_lex_token, &previous_lex_token);
    ret_t result = getNextToken(&current_lex_token, readfile);
    if(result == LEXICAL_ERROR)
    {
        fprintf(stderr, "LEXICAL ERROR\n");
        err_exit(LEXICAL_ERROR);
    }
}


void add_builtin_funcs()
{
    addInt2Double();
    addDouble2Int();
    addreadString();
    addreadInt();
    addreadDouble();
    addLength();
    addSubstring();
    addord();
    addchr();
    addWrite();
}

void init_analyzer(FILE *input_file)
{
    readfile = input_file;
    analysis_error = SYNTAX_ERROR;
    initPrecedenceTable();
    initSymtbToken(&current_symtb_token);
    initSymtbToken(&current_defined_function);
    clearLexToken(&previous_lex_token);
    clearLexToken(&current_lex_token);
    initLexToken(&previous_lex_token);
    initLexToken(&current_lex_token);
    temporary_table = symtb_init(SYMTABLE_INIT_SIZE);
    global_table = symtb_init(SYMTABLE_INIT_SIZE);
    local_tables = stackInit(sizeof(symtable));
    add_later_stack = stackInit(sizeof(symtb_token));
    current_local_level = 0;
    global_table.local_level = 0;
    temporary_table.local_level = 0;
    initSymtbToken(&current_called_function);
    undefined_functions = stackInit(sizeof(symtb_token));
    add_builtin_funcs();
    expr_is_literal = false;
    //generator
    prepare();
    //generator end
}

ret_t prepared_return(ret_t ret)
{
    freeLexToken(&current_lex_token);
    freeLexToken(&previous_lex_token);
    clearSymtbToken(&current_symtb_token);
    clearSymtbToken(&current_defined_function);
    symtb_clear(global_table);
    symtb_clear(temporary_table);
    stackResetSemiPop(local_tables);
    symtable *local_table = stackSemiPop(local_tables);
    while(local_table != NULL)
    {
        symtb_clear(*local_table);
        local_table = stackSemiPop(local_tables);
    }
    stackDestroy(local_tables);
    current_local_level = 0;
    //we suppose that all undefined functions (symtb_tokens) are freed and stack is empty. We free them only in an emerency
    stackResetSemiPop(undefined_functions);
    symtb_token *undef_func = stackSemiPop(undefined_functions);
    while(undef_func != NULL)
    {
        clearSymtbToken(undef_func);
        undef_func = stackSemiPop(undefined_functions);
    }
    symtb_token *later = stackSemiPop(add_later_stack);
    while(later != NULL)
    {
        clearSymtbToken(later);
        later = stackSemiPop(undefined_functions);
    }
    stackDestroy(add_later_stack);
    return ret;
}

void err_exit(ret_t ret)
{
    exit(prepared_return(ret));
}

bool getFromEverywhere(char *id, symtb_token **found, symtable **table_found)
{
    if(getFromLocalTables(id, found, table_found))
    {
        return true;
    }
    else
    if(getFromGlobalTable(id, found))
    {
        if(table_found != NULL)
            *table_found = &global_table;
        return true;
    }
    else
    {
        return false;
    }
}

bool getFromGlobalTable(char *id, symtb_token **found)
{
    symtb_node *node = symtb_find(global_table,  id, NULL);
    if(node != NULL)
        *found = &node->token;
    return node != NULL;
}

bool getFromLocalTables(char *id, symtb_token **token_found, symtable **table_found)
{
    symtb_node *node = symtb_find(temporary_table, id, NULL);
    if(node != NULL)
    {
        if(token_found != NULL)
            *token_found = &node->token;
        if(table_found != NULL)
            *table_found = &temporary_table;
        return true;
    }
    
    stackResetSemiPop(local_tables);
    symtable *local_table = stackSemiPop(local_tables);
    
    while(local_table != NULL)
    {
        node = symtb_find(*local_table, id, NULL);
        if(node != NULL)
        {
            if(token_found != NULL)
                *token_found = &node->token;
            if(table_found != NULL)
                *table_found = local_table;
            stackResetSemiPop(local_tables);
            return true;
        }
        local_table = (symtable*)stackSemiPop(local_tables);
    }
    
    if(token_found != NULL)
        *token_found = NULL;
    if(table_found != NULL)
        *table_found = NULL;
    return false;
}


ret_t analyze_syntax(FILE *input_file)
{
    init_analyzer(input_file);
    if(PROGRAM())
        return prepared_return(NO_ERROR);
    return prepared_return(analysis_error);
}