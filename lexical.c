#include "lexical.h"

int initLexToken(lex_token* token)
{
    token->lexeme_type = 0;
    token->double_value = 0.f;
    token->int_value = 0;
    
    token->str.value = malloc(sizeof(char) * INIT_STR_SIZE);
    if(token->str.value == NULL)
        return 1;
    token->str.capacity = INIT_STR_SIZE;
    token->str.len = 0;
    return 0;
}

int checkKeyword(lex_token token){
    //IF, WHILE, ELSE, FUNC, LET, RETURN, VAR, WITH
    if(!strncmp(token.str.value, "if", 2))
        return IF;
    else if(!strncmp(token.str.value, "while", 5))
        return WHILE;
    else if(!strncmp(token.str.value, "else", 4))
        return ELSE;
    else if(!strncmp(token.str.value, "func", 4))
        return FUNC;
    else if(!strncmp(token.str.value, "let", 3))
        return LET;
    else if(!strncmp(token.str.value, "return", 6))
        return RETURN;
    else if(!strncmp(token.str.value, "var", 3))
        return VAR;
    else if(!strncmp(token.str.value, "with", 4))
        return WITH;

    return 0;
}

int checkType(lex_token token){
    //INT, DOUBLE, STRING, NIL
    if(!strncmp(token.str.value, "Int", 3))
        return INT;
    else if(!strncmp(token.str.value, "Double", 6))
        return DOUBLE;
    else if(!strncmp(token.str.value, "String", 6))
        return STRING;
    else if(!strncmp(token.str.value, "nil", 3))
        return NIL;
    else if(!strncmp(token.str.value, "Int?", 4))
        return NILINT;
    else if(!strncmp(token.str.value, "Double?", 7))
        return NILDOUBLE;
    else if(!strncmp(token.str.value, "String?", 7))
        return NILSTRING;

    return 0;
}

int addToStr(lex_token* token, char c){
    if(token->str.len == token->str.capacity){
        token->str.capacity *= 2;
        token->str.value = (char *)realloc(token->str.value, token->str.capacity);
        if(token->str.value == NULL)
            return 1;
    }

    token->str.value[token->str.len++] = c;
    return 0;
}

void clearLexToken(lex_token *token)
{
    token->lexeme_type = 0;
    token->int_value = 0;
    token->double_value = 0;

    free(token->str.value);
    token->str.value = NULL;
    token->str.capacity = 0;
    token->str.len = 0;
}

ret_t getNextToken(lex_token* token)
{
    //states that lexical automata can take (see documentation)
    typedef enum q_enum
    {
        S,          // Starting state
        IDF,        // Identifier state
        ID1S,/* _ */
        KWRD,       // Keyword state
        TYPE,       // Type check state
        DEC1,       // Decimal state (probably for floats)
        DEC2,       // Another decimal state
        D2,         // State after first digit (could be used for multi-digit numbers)
        EXP,        // Exponent state for floating-point numbers
        INTS,        // Integer state
        SIGN,       // Sign state (+ or -)
        COM1,       // Comment start state (maybe for /* style comments)
        COM2,       // Comment middle state
        COM3,       // Comment end state
        LSTR,       // Large string state
        STR,/* " "*/
        FUNC,       // Function state (possibly for function or lambda constructs)
        FUNC2,      // Another function-related state
        LSTR2,      // Second literal string state
        BCOM2,      // Comment state (possibly for // style comments)
        BCOM3,      // Another comment state
        BCOM4,      // Yet another comment state
        BCOM5,      // Another comment state
        ESC1,       // Escape sequence state
        ESC2,       // Another escape sequence state
        ESC3,       // Yet another escape sequence state
        ESC4,       // Another escape sequence state
        ESC5,        // Another escape sequence state
        QS/* ? */,
        QQS/* ?? */,
        LES /* < */,
        GTS /* > */,
        EXCLAMS/* ! */, 
        ASS /* = */,
        MULS/* * */,
        PLUSS,
        MINUSS,
        CHECK,
    } State;

    if(initLexToken(token))
        return -1;  //vrací -1 když se nepodaří alokovat místo
    int end = 0;
    char c;
    int state = S;
    while(!end){
        c = getchar();

        switch (state)  //State switch
        {
        case S:    //Beginning S
            switch (c)
            {
            case ' ':
            case '\t':
            case '\n':
            case '\r':  //Staying in state S
                state = S;
                break;

            case ':':
                token->lexeme_type = COLON;
                end = 1;
                break;

            case ',':
                token->lexeme_type = COMMA;
                end = 1;
                break;

            case '(':
                token->lexeme_type = LBR1;
                end = 1;
                break;

            case ')':
                token->lexeme_type = RBR1;
                end = 1;
                break;

            case '{':
                token->lexeme_type = LBR2;
                end = 1;
                break;

            case '}':
                token->lexeme_type = RBR2;
                end = 1;
                break;
            
            case '?':   //Going to state Q
                state = QS;
                break;

            case '_':   //Going to state ID1S
                addToStr(token, c);
                state = ID1S;
                break;
            
            case '/':   //Going to state COM1
                state = COM1;
                break;

            case '"':   //Going to state STR
                state = STR;
                break;

            case '*':
                state = MULS;
                break;

            case '-':
                state = MINUSS;
                break;

            case '+':
                state = PLUSS;
                break;

            case '!':
                state = EXCLAMS;
                break;
            
            case '=':
                state = ASS;
                break;

            case '>':
                state = GTS;
                break;

            case '<':
                state = LES;
                break;
            
            default:    //Others
                if(isAlpha(c))
                    state = IDF;
                else if(isNumber(c))
                    state = INTS;
                else if(c == EOF){
                    ungetc(c, stdin);
                    return 0;
                }
                else{
                    token->lexeme_type = UNDEF;
                    return 1;
                }
                addToStr(token, c);
                break;
            }
            break;  //End S

        case MULS:
            token->lexeme_type = MUL;
            end = 1;
            break;

        case PLUSS:
            token->lexeme_type = PLUS;
            end = 1;
            break;

        case MINUSS:
            token->lexeme_type = MINUS;
            end = 1;
            break;

        case EXCLAMS:    //Beginning EXCLAMS '!'
        if(c == '='){
            token->lexeme_type = NEQ;
            end = 1;
        }
        else{
            token->lexeme_type = EXCLAM;
            ungetc(c, stdin);
            end = 1;
        }
        break; //End EXCLAMS '!'

        case ASS:    //Beginning AS '='
        if(c == '='){
            token->lexeme_type = EQ;
            end = 1;
        }
        else{
            token->lexeme_type = AS;
            ungetc(c, stdin);
            end = 1;
        }
        break; //End AS '='

        case GT:    //Beginning GT '>'
        if(c == '='){
            token->lexeme_type = GEQ;
            end = 1;
        }
        else{
            token->lexeme_type = GT;
            ungetc(c, stdin);
            end = 1;
        }
        break; //End GT '>'

        case LE:    //Beginning LE '<'
        if(c == '='){
            token->lexeme_type = LEQ;
            end = 1;
        }
        else{
            token->lexeme_type = LE;
            ungetc(c, stdin);
            end = 1;
        }
        break; //End LE '<'

        case QS: //Beginning Q
            if(c == '?'){
                token->lexeme_type = QQ;
                end = 1;
            }
            else{
                token->lexeme_type = Q;
                ungetc(c, stdin);
                end = 1;
            }
            break;  //End Q

        case ID1S:   //Beginning ID1S
            if(isAlpha(c) || isNumber(c)){
               addToStr(token, c);
               state = IDF;
            }
            else{
                token->lexeme_type = ID1;
                ungetc(c, stdin);
                end = 1;
            }
            break;  //End ID1S

        case IDF:   //Beginning IDF
            if(!(isAlpha(c) || isNumber(c) || c == '_')){
                state = CHECK;
                ungetc(c, stdin);
            }
            else
                addToStr(token, c);
            break;  //End IDF

        case CHECK:
            int res = checkKeyword(*token);
            if(res){
                ungetc(c, stdin);
                token->lexeme_type = res;
            }
            else{
                res = checkType(*token);
                if(res){
                    if(c == '?'){
                        addToStr(token, c);
                        res = checkType(*token);
                    }
                    else
                        ungetc(c, stdin);
                    token->lexeme_type = res;
                }
                else{
                    ungetc(c, stdin);
                    token->lexeme_type = ID;
                }
            }
            end = 1;
            break;

        case COM1:  //Beginning COM1
            switch (c)
            {
            case '/':
                state = COM2;
                break;
            
            case '*':
                state = BCOM2;
                break;
            
            default:
                token->lexeme_type = DIV;
                ungetc(c, stdin);
                end = 1;
                break;
            }
            break;  //End COM1

        case COM2:  //Beginning COM2
            if(c == '\n' || c == EOF){
                state = COM3;
            }
            break;  //End COM2

        case COM3:  //Beginning COM3
            state = S;
            break;  //End COM3

        case BCOM2: //Beginning BCOM2

            //---------------------- FUNC2 --------------------
            break;  //End BCOM2

        case INTS:   //Beginnig INTS
            if(isNumber(c))
                addToStr(token, c);
            else if(c == '.'){
                addToStr(token, c);
                state = D2;
            }
            else if(c == 'e' || c == 'E'){
                addToStr(token, c);
                state = EXP;
            }
            else{
                token->lexeme_type = INT_LIT;
                token->int_value = atoi(token->str.value);
                ungetc(c, stdin);
                end = 1;
            }
            break;  //End INTS

        case D2:    //Beginning D2
            if(isNumber(c)){
                addToStr(token, c);
                state = DEC1;
            }
            else{
                ungetc(c, stdin);
                token->lexeme_type = UNDEF;
                return 1;
            }
            break;  //End D2

        case DEC1:  //Beginning DEC1
            if(isNumber(c))
                addToStr(token, c);
            else if(c == 'e' || c == 'E'){
                addToStr(token, c);
                state = EXP;
            }
            else{
                token->lexeme_type = DOUBLE_LIT;
                token->double_value = atof(token->str.value);
                ungetc(c, stdin);
                end = 1;
            }
            break;  //End DEC1

        case EXP:   //Beginnig EXP
            if(isNumber(c)){
                addToStr(token, c);
                state = DEC2;
            }
            else if(c == '+' || c == '-'){
                addToStr(token, c);
                state = SIGN;
            }
            else{
                ungetc(c, stdin);
                token->lexeme_type = UNDEF;
                return 1;
            }
            break;  //End EXP

        case SIGN:
            if(isNumber(c)){
                addToStr(token, c);
                state = DEC2;
            }
            else{
                ungetc(c, stdin);
                token->lexeme_type = UNDEF;
                return 1;
            }
            break;

        case DEC2:  //Beginning DEC2
            if(isNumber(c))
                addToStr(token, c);
            else{
                token->lexeme_type = DOUBLE_LIT;
                token->double_value = atof(token->str.value);
                ungetc(c, stdin);
                end = 1;
            }
            break;  //End DEC2

        case STR:   //Beginning STR
            if(c == '\n' || c == EOF){
                token->lexeme_type = UNDEF;
                return 1;
            }
            if(c == '"' && token->str.value[token->str.len-3] != '\\'){
                token->lexeme_type = STRING_LIT;
                end = 1;
            }
            else
                addToStr(token, c);
            
            //přidat FUNC1
            
            break;  //End STR
        
        default:    
            //ERROR
            break;
        }

    }
    return 0;
}
