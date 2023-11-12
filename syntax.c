#include "syntax.h"

bool GLOBAL_COMMAND_LIST()
{


}

bool GLOBAL_COMMAND_LIST()
{


}

bool GLOBAL_COMMAND_LIST()
{


}

bool GLOBAL_COMMAND_LIST()
{


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

bool VARDEF()
{
    if(!VAR_ENTITY())
        return false;
    read_move();//read ID after LET/VAR
    if(!currLexTokenIs(ID))
        return false;
    
}

bool FUNCDEF()
{


}

bool LOCAL_COMMAND()
{
    switch(current_lex_token.lexeme_type)
    {
        case ID:
            return ID_COMMAND();
        case LET:
        case VAR:
            return VARDEF();
        case IF:
            return BRANCH();
        case WHILE:
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
        case LBR2:
            return LOCAL_COMMAND();
        case FUNC:
            return FUNCDEF();
        default:
            return false;
    }
}

bool GLOBAL_COMMAND_LIST()
{
    read_move();
    switch(current_lex_token.lexeme_type)
    {
        case ID:
        case VAR:
        case IF:
        case LET:
        case WHILE:
        case LBR2:
        case FUNC:
            return GLOBAL_COMMAND() && GLOBAL_COMMAND_LIST();
        default:
            return false;
    }
}

bool PROGRAM()
{
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


static bool first_read = true;

ret_t read_move()
{
    copyLexToken(current_lex_token, &previous_lex_token);
    ret_t result = getNextToken(&current_lex_token, readfile);
    if(result == -1)//end of file
        return -1;
    else
    if(result == LEXICAL_ERROR)
    {
        fprintf(stderr, "LEXICAL ERROR\n");
        err_exit(LEXICAL_ERROR);
    }
    else
        return NO_ERROR;
}

void init_analyzer(FILE *input_file)
{
    readfile = input_file;
    initSymtbToken(&current_symtb_token);
    initLexToken(&previous_lex_token);
    initLexToken(&current_lex_token);
    global_table = symtb_init(SYMTABLE_INIT_SIZE);
    local_tables = stackInit(sizeof(symtable));
}

ret_t prepared_return(ret_t ret)
{
    first_read = true;
    
    clearLexToken(&current_lex_token);
    clearLexToken(&previous_lex_token);
    clearSymtbToken(&current_symtb_token);
    symtb_clear(global_table);
    
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
    return ret;
}

void err_exit(ret_t ret)
{
    exit(prepared_return(ret));
}


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
    symtb_node node = symtb_find(*local_table, id, NULL);
    if(!node.deleted)
    {
        *token_found = node.token;
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
    return prepared_return(SYNTAX_ERROR);
}