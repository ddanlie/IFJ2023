//Implementace překladače imperativního jazyka IFJ23
//Danil Domrachev (xdomra00)
//Matouš Huczala (xhucza02)
//Elena Ivanova (xivano08)
//Martin Rybnikář (xrybni10)

#include "defs.h"


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


void strZerosFill(char *str, int start_index, int end_index)
{
    for(int i = start_index; i <= end_index; i++)
        str[i] = '\0';
}


