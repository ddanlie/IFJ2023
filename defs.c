#include "defs.h"


void clearSymtbToken(symtb_token token)
{
    free(token.id_name);
    for(int i = 0; i < token.funcArgnamesSize; i++)
    {
        free(token.funcArgnames[i]);
    }
    free(token.funcArgnames);
}

int isNumber(char c){   //přidat do defs
    if(c >= '0' && c <= '9')
        return 1;
    return 0;
}

int isAlpha(char c){
    if(c >= 'a' && c <= 'z')
        return 1;
    else if(c >= 'A' && c <= 'Z')
        return 1;
    return 0;
}