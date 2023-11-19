
#ifndef DEFS_H
#define DEFS_H

#define SYNTAX_DBG
#define SEM_DBG

#define LEXICAL_ERROR 1       // Lexical analysis error (faulty structure of current lexeme)
#define SYNTAX_ERROR 2        // Syntax analysis error
#define UNDEF_ERROR 3         // Undefined function or variable redefinition
#define FUNC_PARAM_ERROR 4    // Incorrect function parameters or return type
#define VAR_INIT_ERROR 5      // Use of undefined or uninitialized variable
#define RET_VAL_ERROR 6       // Missing/extra expression in return statement
#define TYPE_COMPAT_ERROR 7   // Type compatibility error in arithmetic, string, or relational expressions
#define TYPE_DERIV_ERROR 8    // Type of variable or parameter cannot be inferred from the expression
#define SEMANTIC_OTHER_ERROR 9 // Other semantic errors
#define COMPILER_INTERNAL_ERROR 99 // Internal compiler error, e.g., memory allocation error
#define ANOTHER_ERROR 100
#define NO_ERROR 0

typedef short int ret_t;//error return type

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct str_value_t{
    char* value;
    int capacity;
    int len;
} str_value;

int isDigit(char c);
int isAlpha(char c);
int isHex(char c);


void strZerosFill(char *str, int start_index, int end_index);




#endif //DEFS_H
