#include "semantic.h"

bool varRedefinition()
{

    if(current_local_level == 0)
    {
        symtb_token found;
        initSymtbToken(&found);
        if(getFromGlobalTable(current_lex_token.str.value, &found))
        {
            clearSymtbToken(&found);
            analysis_error = UNDEF_ERROR;
            return true;
        }
        clearSymtbToken(&found);
    }
    else
    {
        symtable *table_found;
        if(getFromLocalTables(current_lex_token.str.value, NULL, &table_found))
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


bool isVarDefined()
{
    symtb_token found;
    initSymtbToken(&found);
    if(getFromGlobalTable(current_lex_token.str.value, &found))
    {
        clearSymtbToken(&found);
        return true;
    }
    clearSymtbToken(&found);
    
    symtable *table_found;
    if(getFromLocalTables(current_lex_token.str.value, NULL, &table_found))
    {
        if(table_found->local_level > current_local_level)//higher level -> deeper level
        {
            analysis_error = UNDEF_ERROR;
            return false;
        }
        else
        {
            return true;
        }
    }
}