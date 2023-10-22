//
// Created by Daniel on 21.10.2023.
//

#ifndef LEXICAL_H
#define LEXICAL_H

//lexemes to read
typedef enum lexeme_enum
{
    INT, DOUBLE, STRING, NIL,
    IF, WHILE, ELSE, FUNC, LET, RETURN, VAR,
    Q/* ? */, QQ/* ?? */, EXCLAM/* ! */, EQ/* = */, LEQ/* <= */, GEQ/* >= */, NEQ/* != */,
    MINUS, PLUS, LE/* < */, GT/* > */,
    
    INT_LIT, DOUBLE_LIT, STRING_LIT, LRGSTRING_LIT/* large string literal */,
} lexeme;

//token for lexeme type and its value
typedef struct lex_token_t
{
    lexeme lexeme_type;
    int int_value;
    double double_value;
    char* str_value;
} lex_token;

//init all pointers of the token as null.
//call this function before using token
void initLexToken(lex_token* token);

ret_t getNextToken(lex_token* token);


#endif //LEXICAL_H
