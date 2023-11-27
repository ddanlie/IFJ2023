//Implementace překladače imperativního jazyka IFJ23
//Danil Domrachev (xdomra00)
//Martin Rybnikář (xrybni10)

#ifndef LEXICAL_H
#define LEXICAL_H

#include "defs.h"

#define INIT_STR_SIZE 10
#define MAX_ESCAPE_HEX_LEN 8
#define ASCII_REPR_START 31
#define LEXEMES_COUNT 40


extern bool eoln_flag; // means that there was an end of line during reading next token, becomes resets to false for every new reading
extern bool end_of_file_flag;

//lexemes to read
typedef enum lexeme_enum
{
    UNDEF, INT, DOUBLE, STRING, NIL, NILINT, NILDOUBLE, NILSTRING,
    IF, WHILE, ELSE, FUNC, LET, RETURN, VAR,
    QQ/* ?? */, EXCLAM/* ! */, EQ/* == */, LEQ/* <= */, GEQ/* >= */, NEQ/* != */,
    MINUS, PLUS, LE/* < */, GT/* > */, AS/* = */,
    MUL, DIV, ID1/* _ */, ID,
    LBR1 /* left ( bracket*/, RBR1/* right ) bracket*/, LBR2/* left { bracket*/, RBR2/* right } bracket*/,
    COLON, COMMA, RARROW /* -> */,

    INT_LIT, DOUBLE_LIT, STRING_LIT/* including large string literal */,
} lexeme;

//token for lexeme type and its value
typedef struct lex_token_t
{
    lexeme lexeme_type;
    int int_value;
    double double_value;
    str_value str;
} lex_token;

//init all pointers of the token as null.
//call this function before using token and then initLexToken
//example:
// 1st use: clearLexToken(...); initLexToken(...).
// 2nd and more: initLexToken(...).
// Last use: freeLexToken(...)
void clearLexToken(lex_token *token);
int initLexToken(lex_token* token);
void freeLexToken(lex_token *token);
void copyLexToken(lex_token src, lex_token *dst);
int addToStr(lex_token* token, char c);
void printLexeme(lexeme l);
int checkKeyword(lex_token token);
int checkType(lex_token token);
void printLexToken(lex_token token);


ret_t getNextToken(lex_token* token, FILE *input_file);




#endif //LEXICAL_H
