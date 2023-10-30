#ifndef LEXICAL_H
#define LEXICAL_H

#define INIT_STR_SIZE 10
#define MAX_ESCAPE_HEX_LEN 8
#define ASCII_REPR_START 31

#include "defs.h"

//lexemes to read
typedef enum lexeme_enum
{
    UNDEF, INT, DOUBLE, STRING, NIL, NILINT, NILDOUBLE, NILSTRING,
    IF, WHILE, ELSE, FUNC, LET, RETURN, VAR, WITH,
    QQ/* ?? */, EXCLAM/* ! */, EQ/* == */, LEQ/* <= */, GEQ/* >= */, NEQ/* != */,
    MINUS, PLUS, LE/* < */, GT/* > */, AS/* = */,
    MUL, DIV, ID1/* _ */, ID,
    LBR1 /* left ( bracket*/, RBR1/* right ) bracket*/, LBR2/* left { bracket*/, RBR2/* right } bracket*/,
    COLON, COMMA,
    
    INT_LIT, DOUBLE_LIT, STRING_LIT/* large string literal */,
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
void printLexeme(lexeme l);
int checkKeyword(lex_token token);
int checkType(lex_token token);
void printLexToken(lex_token token);

ret_t getNextToken(lex_token* token, FILE *input_file);


#endif //LEXICAL_H
