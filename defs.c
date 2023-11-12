#include "defs.h"


void initSymtbToken(symtb_token *token)
{
    token->id_name = NULL;
    token->type = UNDEF_ID;
    token->lit_type = UNDEF_TYPE;
    token->funcArgs = NULL;
    token->funcArgnames = NULL;
    token->funcArgsSize = 0;
    token->initialized = false;
}

void clearSymtbToken(symtb_token *token)
{
    if(token->id_name != NULL)
    {
        free(token->id_name);
        token->id_name = NULL;
    }

    if(token->funcArgs != NULL)
    {
        free(token->funcArgs);
        token->funcArgs = NULL;
    }
    
    for(int i = 0; i < token->funcArgsSize; i++)
    {
        free(token->funcArgnames[i]);
    }
    if(token->funcArgnames != NULL)
    {
        free(token->funcArgnames);
        token->funcArgnames = NULL;
    }
    
}

int isDigit(char c){   //přidat do defs
    if(c >= '0' && c <= '9')
        return 1;
    return 0;
}

int isAlpha(char c){
    if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return 1;
    return 0;
}

int isHex(char c)
{
    if(isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
        return 1;
    return 0;
}
