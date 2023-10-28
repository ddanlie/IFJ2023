
#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef short int ret_t;//error return type

typedef enum id_type_t
{
    VARIABLE, CONSTANT, FUNCTION
} id_type;

typedef enum literal_type_t
{
    INT_TYPE, STRING_TYPE, DOUBLE_TYPE,
    NINT_TYPE, NSTRING_TYPE, NDOUBLE_TYPE, //nillable int/string/double
    VOID_TYPE
} literal_type;

//symbol table token to construct from lexeme tokens
typedef struct symtb_token_t
{
    char *id_name;//id is always a pointer to a heap (malloc), so it can be freed
    id_type type;
    literal_type lit_type;//for variable/constant INT/NINT,STRING/NSTRING and DOUBLE/NDOUBLE are legitimate //for function that means what it returns
    literal_type *funcArgs;//array order corresponds to funcArgnames
    char **funcArgnames;//array order corresponds to funcArgs //array and its elements are always pointers to a heap so they can be freed
    int funcArgnamesSize;
} symtb_token;

void clearSymtbToken(symtb_token);

#endif //DEFS_H
