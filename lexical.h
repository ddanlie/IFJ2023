//
// Created by Daniel on 21.10.2023.
//

#ifndef LEXICAL_H
#define LEXICAL_H

#define INIT_STR_SIZE 10

#include "defs.h"

//lexemes to read
typedef enum lexeme_enum
{
    UNDEF, INT, DOUBLE, STRING, NIL, NILINT, NILDOUBLE, NILSTRING,
    IF, WHILE, ELSE, FUNC, LET, RETURN, VAR, WITH,
    Q/* ? */, QQ/* ?? */, EXCLAM/* ! */, EQ/* == */, LEQ/* <= */, GEQ/* >= */, NEQ/* != */,
    MINUS, PLUS, LE/* < */, GT/* > */, AS/* = */,
    MUL, DIV, ID1/* _ */, ID,
    LBR1, RBR1, LBR2, RBR2, COLON, COMMA,
    
    INT_LIT, DOUBLE_LIT, STRING_LIT, LRGSTRING_LIT/* large string literal */,
} lexeme;

typedef struct str_value_t{
    char* value;
    int capacity;
    int len;
} str_value;

//token for lexeme type and its value
typedef struct lex_token_t
{
    lexeme lexeme_type;
    int int_value;
    double double_value;
    str_value str;
} lex_token;

//init all pointers of the token as null.
//call this function before using token
int initLexToken(lex_token* token);
int addToStr(lex_token* token, char c);
void clearLexToken(lex_token *token);

ret_t getNextToken(lex_token* token);

int checkKeyword(lex_token token);
int checkType(lex_token token);

#endif //LEXICAL_H
