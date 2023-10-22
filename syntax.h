//
// Created by Daniel on 21.10.2023.
//

#ifndef SYNTAX_H
#define SYNTAX_H

#include "defs.h"

typedef enum id_type_t
{
    VARIABLE, CONSTANT, FUNCTION
} id_type;

typedef enum literal_type_t
{
    INT, STRING, DOUBLE, VOID
} literal_type;

//symbol table token to construct from lexeme tokens
typedef struct symtb_token_t
{
    char *id_name;
    id_type type;
    literal_type lit_type;//for variable/constant INT,STRING and DOUBLE are legitimate //for function that means what it returns
    bool nillable; //for variable that means - nil can be assigned //for function that means - can return nil
    
} symtb_token;



#endif //SYNTAX_H
