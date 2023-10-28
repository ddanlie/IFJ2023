#include "defs.h"
#include "lexical.h"
#include "defs.h"

void initLexToken(lex_token* token)
{
    token->str_value = NULL;
}

void clearLexToken(lex_token *token)
{
    free(token->str_value);
    token->str_value = NULL;
}

ret_t getNextToken(lex_token* token)
{
    //states that lexical automata can take (see documentation)
    typedef enum q_enum
    {
        S,          // Starting state
        IDF,        // Identifier state
        KWRD,       // Keyword state
        TYPE,       // Type check state
        DEC1,       // Decimal state (probably for floats)
        DEC2,       // Another decimal state
        D2,         // State after first digit (could be used for multi-digit numbers)
        EXP,        // Exponent state for floating-point numbers
        INT,        // Integer state
        SIGN,       // Sign state (+ or -)
        COM1,       // Comment start state (maybe for /* style comments)
        COM2,       // Comment middle state
        COM3,       // Comment end state
        LSTR,       // Literal string state
        FUNC,       // Function state (possibly for function or lambda constructs)
        FUNC2,      // Another function-related state
        LSTR2,      // Second literal string state
        BCOM1,      // Comment state (possibly for // style comments)
        BCOM2,      // Another comment state
        BCOM3,      // Yet another comment state
        BCOM4,      // Another comment state
        BCOM5,      // Another comment state
        ESC1,       // Escape sequence state
        ESC2,       // Another escape sequence state
        ESC3,       // Yet another escape sequence state
        ESC4,       // Another escape sequence state
        ESC5,        // Another escape sequence state
        Q,/* ? */
        QQ,/* ?? */
        LEQ,/* <= */
        GEQ,/* >= */
        NEQ/* != */
        //EXCLAM/* ! */, theese two don't need extra state as they consist of 1 character, so
        //EQ/* = */,     they can be dealt in one raw of switch-case construction
    } States;
    
    
    //clear token memory
    if(token->str_value != NULL)
        free(token->str_value);
    //
}


