#include "syntax.h"


FILE *readfile;

lex_token current_lex_token;
lex_token previous_lex_token;
symtb_token current_symtb_token;
symtb_token current_called_function;
Stack *undefined_functions;

symtable global_table;
Stack *local_tables;

int current_local_level;

const char gr = '>';
const char le = '<';
const char eq = '=';
char precedence_table[LEXEMES_COUNT][LEXEMES_COUNT];

ret_t analysis_error;


bool GLOBAL_COMMAND_LIST();
bool LOCAL_COMMAND_LIST();
bool FUNC_COMMAND_LIST();
bool BLOCK();
bool LOCAL_COMMAND();


//read first token of expr before use!
//expressions end up reading next lexeme token
//In this grammar ID1 '_' is considered as non-terminal 'E'.
bool EXPR()
{
    
    return true;
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
            
            if(!FCALL_PARAM_NAME())//function reads extra lexeme because of grammar error
                return false;
            
            //FCALL_PARAM_NAME
            copyLexToken(current_lex_token, &tmp);
            copyLexToken(previous_lex_token, &current_lex_token);
            //FCALL_PARAM_NAME end
    
            if(!TERM())//does not use previous token!
                return false;
    
            //find id and get type
            symtb_token tkn;
            initSymtbToken(&tkn);
            if(!getFromGlobalTable(current_lex_token.str.value, &tkn))
            {
                if(!getFromLocalTables(current_lex_token.str.value, &tkn, NULL) || !(tkn.initialized))
                {
                    //undefined variable
                    analysis_error = VAR_INIT_ERROR;
                    return false;
                }
            }
            symtbTokenAddArgType2(&current_called_function, tkn);
    
            clearSymtbToken(&tkn);
            
            //FCALL_PARAM_NAME
            copyLexToken(tmp, &current_lex_token);
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
            
            copyLexToken(current_lex_token, &tmp);
            copyLexToken(previous_lex_token, &current_lex_token); //set prev as curr
    
            if(!TERM())//use prev as curr
                return false;
    
            symtb_token tkn;
            initSymtbToken(&tkn);
            if(!getFromGlobalTable(current_lex_token.str.value, &tkn))//use prev as curr
            {
                if(!getFromLocalTables(current_lex_token.str.value, &tkn, NULL) || !(tkn.initialized))//use prev as curr
                {
                    //undefined variable
                    analysis_error = VAR_INIT_ERROR;
                    return false;
                }
            }
            symtbTokenAddArgType2(&current_called_function, tkn);
    
            clearSymtbToken(&tkn);
            
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
            
            read_move();//see FCALL_PARAM_NAME(). we need to read extra lexeme
            break;
        }
        default:
            return false;
    }
   // read_move();//read ',' or ')' or another
    if(!FCALL_PARAM_LIST_2())
        return false;
    
    return true;
}

bool FUNC_CALL()
{
    //semantic
    symtbTokenCopyName(&current_called_function, previous_lex_token);//previous = 'ID'  current = '('
    symtb_token defined_function;
    initSymtbToken(&defined_function);
    if(!getFromGlobalTable(previous_lex_token.str.value, &defined_function))//if function with ID is not found
        current_called_function.initialized = false;
    //semantic end
    
    read_move();
    if(!FCALL_PARAM_LIST())
        return false;
    if(!currLexTokenIs(RBR1))
        return false;
    
    
    //semantic
    
    //check if called function corresponds to rules by case:
    //Case 1: ID = f(...)  - If function is initialized:
    //                          Assign ID type to function return type. Compare function definition to the function called.
    //                       If function is not initialized
    //                          Assign ID type to function return type
    //
    //Case 2: f(...)       - If function is initialized:
    //                          Compare function definition to the function called excepting return type
    //
    //
    if(!current_called_function.initialized)//if function is not initialized put it to 'undefined_functions'
    {
    
    }
    //semantic end
    clearSymtbToken(&defined_function);
    clearSymtbToken(&current_called_function);
    initSymtbToken(&current_called_function);
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
            current_symtb_token.lit_type = INT_TYPE;
            break;
        case DOUBLE:
            current_symtb_token.lit_type = DOUBLE_TYPE;
            break;
        case STRING:
            current_symtb_token.lit_type = STRING_TYPE;
            break;
        case NILINT:
            current_symtb_token.lit_type = NINT_TYPE;
            break;
        case NILDOUBLE:
            current_symtb_token.lit_type = NDOUBLE_TYPE;
            break;
        case NILSTRING:
            current_symtb_token.lit_type = NSTRING_TYPE;
            break;
        default:
            return false;
    }
    return true;
}


bool OPT_VAR_TYPE()
{
    return TYPE();
}


bool VAR_ENTITY()
{
    if(currLexTokenIs(VAR))
    {
        current_symtb_token.type = VARIABLE;
        return true;
    }
    else
    if(currLexTokenIs(LET))
    {
        current_symtb_token.type = CONSTANT;
        return true;
    }
    else
        return false;
}

// 'let/var' 'id' ': type/e' '= ASSIGN/e'
bool VARDEF()
{
    if(!VAR_ENTITY())
        return false;
    read_move();//read ID after LET/VAR
    if(!currLexTokenIs(ID))
        return false;
    //semantic
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
        read_move();//read '=' or another lexeme
    }
    
    if(currLexTokenIs(AS))
    {
        at_least_one_flag = true;
        read_move();
        if(!OPT_VAR_EXPR())
            return false;
        
        //semantic
        
        //end
    }
    
    if(!at_least_one_flag)
        return false;
    
    //semantic
    //check if variable is not redefined and add to local/global table
    
    //semantic end
    
    return true;
}

bool PARAM_ID()
{
    return currLexTokenIs(ID) || currLexTokenIs(ID1);
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
    read_move();
    if(!currLexTokenIs(COLON))
        return false;
    read_move();
    if(!TYPE())
        return false;
    read_move();
    if(!PARAM_LIST_2())
        return false;
}

bool PARAM_LIST()
{
    if(!PARAM_ID())
        return false;
    read_move();
    if(!currLexTokenIs(ID))
        return false;
    read_move();
    if(!currLexTokenIs(COLON))
        return false;
    read_move();
    if(!TYPE())
        return false;
    read_move();
    if(!PARAM_LIST_2())
        return false;
    
    return true;
}

bool RET_EXPR()
{
    if(EXPR())
    {
        //semantic. check if expr type corresponds to return type
        //semantic end
        
    }
    else
    {
        //semantic. check if return type corresponds to void
    
        //semantic end
    }
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
        }
        default:
            return false;
    }
    
    return true;
}

bool FUNC_BLOCK()
{
    if(!currLexTokenIs(LBR2))
        return false;
    read_move();
    if(!FUNC_COMMAND_LIST())
        return false;
    if(!currLexTokenIs(RBR2))
        return false;
    
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
        if(!currLexTokenIs(TYPE()))
            return false;
        read_move();
    }
    return true;
}

bool FUNCDEF()
{
    if(!currLexTokenIs(ID))
        return false;
    
    read_move();
    
    if(!currLexTokenIs(LBR1))
    {
        read_move();
        if(!PARAM_LIST())
            return false;
    }
    read_move();//read '->' or '{' or wrong lexeme
    if(!RETURN_FORM())
        return false;
    if(!FUNC_BLOCK())
        return false;

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
    getFromGlobalTable(previous_lex_token.str.value, &current_symtb_token);
    if(currLexTokenIs(AS))
    {
        if(!VAR_ASSIGN())
            return false;
    }
    else if(currLexTokenIs(LBR1))
    {
        if(!FUNC_CALL())
            return false;
        read_move();//command must read next lexeme
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
        read_move();//read one more lexeme because EXPR() below reads one more too
    }
    else if(!EXPR())
    {
        return false;
    }
    return true;
}

bool BRANCH()
{
    if(!BRANCH_CANDIDATE())
        return false;
    if(!BLOCK())
        return false;
    read_move();
    if(!currLexTokenIs(ELSE))
        return false;
    read_move();
    if(!BLOCK())
        return false;
    
    read_move();//command must read next lexeme
    
    return true;
}


bool ITERATION()
{
    if(!EXPR())
        return false;
    
    if(!BLOCK())
        return false;
    
    read_move();//command must read next lexeme
    
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


bool BLOCK()
{
    if(!LOCAL_COMMAND_LIST())
        return false;
    if(!currLexTokenIs(RBR2))
        return false;
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
    
    //EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID, NIL, LBR1, RBR1, UNDEF
    //first lexeme - columnt index
    const int grtbrows = 15;
    const int grtbcols = 18;
    int grtb_colscnt[] = {14, 14, 12, 12, 4, 4, 4, 4, 4, 4, 3, 15, 3, 2, 12};
    lexeme padding = ELSE;
    lexeme greater_table[15][18] = {
            { EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, RBR1, UNDEF},
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
            { NIL, RBR1, UNDEF},
            { LBR1, UNDEF},
            { RBR1, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, UNDEF}
    };
    
    //EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID, NIL, LBR1, RBR1, UNDEF
    const int letbrows = 14;
    const int letbcols = 18;
    int letb_colscnt[] = {4, 4, 6, 6, 9, 9, 8, 8, 8, 8, 15, 15, 12, 17};
    lexeme less_table[14][18] = {
            { MUL, EXCLAM, ID, LBR1},
            { DIV, EXCLAM, ID, LBR1},
            { PLUS, EXCLAM, MUL, DIV, ID, LBR1},
            { MINUS, EXCLAM, MUL, DIV, ID, LBR1},
            { EQ, EXCLAM, MUL, DIV, PLUS, MINUS, ID, NIL, LBR1},
            { NEQ, EXCLAM, MUL, DIV, PLUS, MINUS, ID, NIL, LBR1},
            { LE, EXCLAM, MUL, DIV, PLUS, MINUS, ID, LBR1},
            { GT, EXCLAM, MUL, DIV, PLUS, MINUS, ID, LBR1},
            { LEQ, EXCLAM, MUL, DIV, PLUS, MINUS, ID, LBR1},
            { GEQ, EXCLAM, MUL, DIV, PLUS, MINUS, ID, LBR1},
            { QQ, EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID, LBR1},
            { LBR1, EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID, NIL},
            { RBR1, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, UNDEF},
            { UNDEF, EXCLAM, MUL, DIV, PLUS, MINUS, EQ, NEQ, LE, GT, LEQ, GEQ, QQ, ID, NIL, LBR1, RBR1}
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

void init_analyzer(FILE *input_file)
{
    readfile = input_file;
    analysis_error = SYNTAX_ERROR;
    initSymtbToken(&current_symtb_token);
    clearLexToken(&previous_lex_token);
    clearLexToken(&current_lex_token);
    initLexToken(&previous_lex_token);
    initLexToken(&current_lex_token);
    global_table = symtb_init(SYMTABLE_INIT_SIZE);
    local_tables = stackInit(sizeof(symtable));
    current_local_level = 0;//global level
    global_table.local_level = current_local_level;
    initSymtbToken(&current_called_function);
    undefined_functions = stackInit(sizeof(symtb_token));
}

ret_t prepared_return(ret_t ret)
{
    freeLexToken(&current_lex_token);
    freeLexToken(&previous_lex_token);
    clearSymtbToken(&current_symtb_token);
    symtb_clear(global_table);
    
    stackResetSemiPop(local_tables);
    symtable *local_table = stackSemiPop(local_tables);
    if(local_table != NULL)
    {
        while(local_tables->semiPopCounter != 0)
        {
            symtb_clear(*local_table);
            local_table = stackSemiPop(local_tables);
        }
    }
    stackDestroy(local_tables);
    current_local_level = 0;
    //we suppose that all undefined functions (symtb_tokens) are freed and stack is empty. We free them only in an emerency
    stackResetSemiPop(undefined_functions);
    symtb_token *undef_func = stackSemiPop(undefined_functions);
    if(undef_func != NULL)
    {
        while(undefined_functions->semiPopCounter != 0)
        {
            clearSymtbToken(undef_func);
            undef_func = stackSemiPop(undefined_functions);
        }
    }
    stackDestroy(undefined_functions);
    return ret;
}

void err_exit(ret_t ret)
{
    exit(prepared_return(ret));
}

////check if called function is in tables. If not - get func name for adding to undefined_functions later. If yes - get that function
//void searchCalledFunction(lex_token function)
//{
//    if(!getFromGlobalTable(function.str.value, &current_called_function))
//    {
//        symtbTokenCopyName(&current_called_function, function);
//        current_called_function.initialized = false;
//    }
//}

bool getFromGlobalTable(char *id, symtb_token *found)
{
    symtb_node node = symtb_find(global_table,  id, NULL);
    found = NULL;
    if(!node.deleted)
        *found = node.token;
    return !node.deleted;
}

bool getFromLocalTables(char *id, symtb_token *token_found, symtable **table_found)
{
    stackResetSemiPop(local_tables);
    symtable *local_table = stackSemiPop(local_tables);
    if(local_table == NULL)
    {
        token_found = NULL;
        table_found = NULL;
        return false;
    }
    clearSymtbToken(&current_called_function);
    symtb_node node = symtb_find(*local_table, id, NULL);
    if(!node.deleted)
    {
        *token_found = node.token;
        if(table_found != NULL)
            *table_found = local_table;
        stackResetSemiPop(local_tables);
        return true;
    }
    while(local_tables->semiPopCounter != 0)
    {
        local_table = (symtable*)stackSemiPop(local_tables);
        node = symtb_find(*local_table, id, NULL);
        if(!node.deleted)
        {
            *token_found = node.token;
            if(table_found != NULL)
                *table_found = local_table;
            stackResetSemiPop(local_tables);
            return true;
        }
    }
    
    return false;
}



ret_t analyze_syntax(FILE *input_file)
{
    init_analyzer(input_file);
    if(PROGRAM())
        return prepared_return(NO_ERROR);
    return prepared_return(analysis_error);
}