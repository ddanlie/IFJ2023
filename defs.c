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
