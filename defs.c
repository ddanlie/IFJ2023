#include "defs.h"


void initSymtbToken(symtb_token *token)
{
    token->id_name = NULL;
    token->type = UNDEF_ID;
    token->lit_type = UNDEF_TYPE;
    token->funcArgTypes = NULL;
    token->funcArgnames = NULL;
    token->funcLocalArgnames = NULL;
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

    if(token->funcArgTypes != NULL)
    {
        free(token->funcArgTypes);
        token->funcArgTypes = NULL;
    }
    
    for(int i = 0; i < token->funcArgsSize; i++)
    {
        free(token->funcArgnames[i]);
        free(token->funcLocalArgnames[i]);
    }
    if(token->funcArgnames != NULL)
    {
        free(token->funcArgnames);
        token->funcArgnames = NULL;
    }
    if(token->funcLocalArgnames != NULL)
    {
        free(token->funcLocalArgnames);
        token->funcLocalArgnames = NULL;
    }
    
}

int isDigit(char c){   //přidat do defs
    if(c >= '0' && c <= '9')
        return 1;
    return 0;
}


void checkArgsSetSize(symtb_token *dst)
{
    if(dst->funcLocalArgnames[dst->funcArgsSize] != NULL
    && dst->funcArgnames[dst->funcArgsSize] != NULL
    && dst->funcArgTypes[dst->funcArgsSize] != UNDEF_TYPE)
        dst->funcArgsSize++;
}

void symtbTokenCopyName(symtb_token *dst, lex_token src)
{
    if(dst->id_name == NULL)
        dst->id_name = malloc(sizeof(char)*(src.str.len+1));
    if(strlen(dst->id_name) < src.str.len)
        dst->id_name = realloc(dst->id_name, sizeof(char)*(src.str.len+1));
    strcpy(dst->id_name, src.str.value);
}

void symtbTokenAddArgName(symtb_token *dst,  lex_token src)
{
    static int args_capacity;
    if(dst->funcArgnames == NULL)
    {
        args_capacity = 10;
        dst->funcArgnames = malloc(sizeof(char *) * args_capacity);
        for(int i = 0; i < args_capacity; i++)
            dst->funcArgnames[i] = NULL;
    }
    
    dst->funcArgnames[dst->funcArgsSize] = malloc(sizeof(char) * (src.str.len + 1));
    strcpy(dst->funcArgnames[dst->funcArgsSize], src.str.value);
    
    checkArgsSetSize(dst);
    
    if(dst->funcArgsSize == args_capacity)
    {
        args_capacity *= 2;
        dst->funcArgnames = realloc(dst->funcArgnames, sizeof(char*)*args_capacity);
        for(int i = dst->funcArgsSize; i < args_capacity; i++)
            dst->funcArgnames[i] = NULL;
    }
}

void symtbTokenAddArgType(symtb_token *dst, lex_token src)
{
    static int args_capacity;
    if(dst->funcArgTypes == NULL)
    {
        args_capacity = 10;
        dst->funcArgTypes = malloc(sizeof(literal_type)*args_capacity);
        for(int i = 0; i < args_capacity; i++)
            dst->funcArgTypes[i] = UNDEF_TYPE;
    }
    
    //2 cases: 1. when calling function, 2. when defining function
    switch(src.lexeme_type)
    {
        case INT:
        case INT_LIT:
            dst->funcArgTypes[dst->funcArgsSize] = INT_TYPE;
            break;
        case DOUBLE:
        case DOUBLE_LIT:
            dst->funcArgTypes[dst->funcArgsSize] = DOUBLE_TYPE;
            break;
        case STRING:
        case STRING_LIT:
            dst->funcArgTypes[dst->funcArgsSize] = STRING_TYPE;
            break;
        case NILSTRING:
            dst->funcArgTypes[dst->funcArgsSize] = NSTRING_TYPE;
            break;
        case NILDOUBLE:
            dst->funcArgTypes[dst->funcArgsSize] = NDOUBLE_TYPE;
            break;
        case NILINT:
            dst->funcArgTypes[dst->funcArgsSize] = NINT_TYPE;
            break;
        case NIL:
            dst->funcArgTypes[dst->funcArgsSize] = UNDEF_TYPE;
            break;
        default:
            return;
    }
    checkArgsSetSize(dst);
    
    if(dst->funcArgsSize == args_capacity)
    {
        args_capacity *= 2;
        dst->funcArgTypes = realloc(dst->funcArgTypes, sizeof(literal_type) * args_capacity);
    }
}

void symtbTokenAddLocalArgName(symtb_token *dst,  lex_token src)
{
    static int args_capacity;
    if(dst->funcLocalArgnames == NULL)
    {
        args_capacity = 10;
        dst->funcLocalArgnames = malloc(sizeof(char *) * args_capacity);
        for(int i = 0; i < args_capacity; i++)
            dst->funcLocalArgnames[i] = NULL;
    }
    
    dst->funcLocalArgnames[dst->funcArgsSize] = malloc(sizeof(char) * (src.str.len + 1));
    strcpy(dst->funcLocalArgnames[dst->funcArgsSize], src.str.value);
    
    checkArgsSetSize(dst);
    
    if(dst->funcArgsSize == args_capacity)
    {
        args_capacity *= 2;
        dst->funcLocalArgnames = realloc(dst->funcLocalArgnames, sizeof(char*)*args_capacity);
        for(int i = dst->funcArgsSize; i < args_capacity; i++)
            dst->funcLocalArgnames[i] = NULL;
    }
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
