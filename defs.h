
#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define LEXICAL_ERROR 1       // Lexical analysis error (faulty structure of current lexeme)
#define SYNTAX_ERROR 2        // Syntax analysis error
#define UNDEF_FUNC_ERROR 3    // Undefined function or variable redefinition
#define FUNC_PARAM_ERROR 4    // Incorrect function parameters or return type
#define VAR_INIT_ERROR 5      // Use of undefined or uninitialized variable
#define RET_VAL_ERROR 6       // Missing/extra expression in return statement
#define TYPE_COMPAT_ERROR 7   // Type compatibility error in arithmetic, string, or relational expressions
#define TYPE_DERIV_ERROR 8    // Type of variable or parameter cannot be inferred from the expression
#define SEMANTIC_OTHER_ERROR 9 // Other semantic errors
#define COMPILER_INTERNAL_ERROR 99 // Internal compiler error, e.g., memory allocation error
#define NO_ERROR 0

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

int isDigit(char c);
int isAlpha(char c);
int isHex(char c);
#endif //DEFS_H
