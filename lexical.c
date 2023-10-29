#include "lexical.h"

void initLexToken(lex_token* token)
{
    token->lexeme_type = 0;
    token->double_value = 0.f;
    token->int_value = 0;
    
    token->str.value = malloc(sizeof(char) * INIT_STR_SIZE);
    token->str.capacity = INIT_STR_SIZE;
    token->str.len = 0;
}

int checkKeyword(lex_token token){
    //IF, WHILE, ELSE, FUNC, LET, RETURN, VAR
    if(!strncmp(token.str.value, "if", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "while", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "else", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "func", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "let", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "return", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "var", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "with", token.str.len))
        return 1;

    return 0;
}

int checkType(lex_token token){
    //INT, DOUBLE, STRING, NIL,
    if(!strncmp(token.str.value, "Int", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "Double", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "String", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "nil", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "Int?", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "Double?", token.str.len))
        return 1;
    else if(!strncmp(token.str.value, "String?", token.str.len))
        return 1;

    return 0;
}

void addToStr(lex_token* token, char c){
    if(token->str.capacity == token->str.len){
        token->str.capacity *= 2;
        token->str.value = (char *)realloc(token->str.value, token->str.capacity);
    }

    token->str.value[token->str.len++] = c;
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

int isNumber(char c){
    if(c >= '0' && c <= '9')
        return 1;
    return 0;
}

int isAlpha(char c){
    if(c >= 'a' && c <= 'z')
        return 1;
    else if(c >= 'A' && c <= 'Z')
        return 1;
    return 0;
}

ret_t getNextToken(lex_token* token)
{
    //states that lexical automata can take (see documentation)
    typedef enum q_enum
    {
        S,          // Starting state
        IDF,        // Identifier state
        ID1,/* _ */
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
        Q/* ? */,
        QQ/* ?? */,
        LE /* < */,
        LEQ/* <= */,
        GT /* > */,
        GEQ/* >= */,
        NEQ/* != */,
        EXCLAM/* ! */, 
        AS /* = */,
        EQ/* == */,
        MUL/* * */,
        DIV/* / */,
        UNDEF,
    } State;

    initLexToken(token);
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
            case ':':
            case ',':
            case '(':
            case ')':
            case ' ':
            case '\t':
            case '\n':
            case '\r':  //Staying in state S
                state = S;
                break;
            
            case '?':   //Going to state Q
                addToStr(token, c);
                state = Q;
                break;

            case '_':   //Going to state ID1
                addToStr(token, c);
                state = ID1;
                break;
            
            case '/':   //Going to state COM1
                addToStr(token, c);
                state = COM1;
                break;

            case '"':   //Going to state STR
                addToStr(token, c);
                state = STR;
                break;

            case '*':
                addToStr(token, c);
                state = MUL;
                end = 1;
                break;

            case '-':
            case '+':
                addToStr(token, c);
                state = SIGN;
                end = 1;
                break;

            case '!':
                addToStr(token, c);
                state = EXCLAM;
                break;
            
            case '=':
                addToStr(token, c);
                state = AS;
                break;

            case '>':
                addToStr(token, c);
                state = GT;
                break;

            case '<':
                addToStr(token, c);
                state = LE;
                break;
            
            default:    //Others
                if(isAlpha(c))    //Alpha
                    state = IDF;
                else if(isNumber(c))  //Digit
                    state = INT;
                else if(c == EOF){
                    ungetc(c, stdin);
                    return 1;
                }
                else{
                    token->lexeme_type = UNDEF;
                    return 10;
                }
                addToStr(token, c);
                break;
            }
            break;  //End S

        case EXCLAM:    //Beginning EXCLAM '!'
        if(c == '='){
            addToStr(token, c);
            state = NEQ;
            end = 1;
        }
        else{
            ungetc(c, stdin);
            end = 1;
        }
        break; //End EXCLAM '!'

        case AS:    //Beginning AS '='
        if(c == '='){
            addToStr(token, c);
            state = EQ;
            end = 1;
        }
        else{
            ungetc(c, stdin);
            end = 1;
        }
        break; //End AS '='

        case GT:    //Beginning GT '>'
        if(c == '='){
            addToStr(token, c);
            state = GEQ;
            end = 1;
        }
        else{
            ungetc(c, stdin);
            end = 1;
        }
        break; //End GT '>'

        case LE:    //Beginning LE '<'
        if(c == '='){
            addToStr(token, c);
            state = LEQ;
            end = 1;
        }
        else{
            ungetc(c, stdin);
            end = 1;
        }
        break; //End LE '<'

        case Q: //Beginning Q
            if(c == '?'){
                addToStr(token, c);
                state = QQ;
                end = 1;
            }
            else{
                ungetc(c, stdin);
                end = 1;
            }
            break;  //End Q

        case ID1:   //Beginning ID1
            if(isAlpha(c) || isNumber(c)){
               addToStr(token, c);
               state = IDF;
            }
            else{
                ungetc(c, stdin);
                token->lexeme_type = UNDEF;
                return 10;
            }
            break;  //End ID1

        case KWRD:
        case TYPE:      //Beginning IDF
        case IDF:
            if(checkKeyword(*token))
                state = KWRD;
            else if (checkType(*token))
                state = TYPE;
            else
                state = IDF;

            if((state == KWRD || state == TYPE) && c == '?'){
                addToStr(token, c);
                end = 1;
                break;
            }
            
            if(!(isAlpha(c) || isNumber(c) || c == '_')){
                ungetc(c, stdin);
                end = 1;
               }
            else
                addToStr(token, c);
            break;  //End IDF

        case COM1:  //Beginning COM1
            switch (c)
            {
            case '/':
                addToStr(token, c);
                state = COM2;
                break;
            
            case '*':
                addToStr(token, c);
                state = BCOM2;
                break;
            
            default:
                if(isNumber(c) || isAlpha(c) || c == ' ' || c == '_'){
                    ungetc(c, stdin);
                    state = DIV;
                    end = 1;
                }
                else{
                    ungetc(c, stdin);
                    token->lexeme_type = UNDEF;
                    return 10;
                }
                break;
            }
            break;  //End COM1

        case COM2:  //Beginning COM2
            if(c == '\n' || c == EOF){
                state = COM3;
                end = 1;
            }
            else
                addToStr(token, c);
            break;  //End COM2

        case BCOM2: //Beginning BCOM2
            //---------------------- FUNC2 --------------------
            end = 1; //Prozatím
            break;  //End BCOM2w

        case INT:   //Beginnig INT
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
                ungetc(c, stdin);
                token->int_value = atoi(token->str.value);
                end = 1;
            }
            break;  //End INT

        case D2:    //Beginning D2
            if(isNumber(c)){
                addToStr(token, c);
                state = DEC1;
            }
            else{
                ungetc(c, stdin);
                token->lexeme_type = UNDEF;
                return 10;
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
                ungetc(c, stdin);
                token->double_value = atof(token->str.value);
                end = 1;
            }
            break;  //End DEC1

        case EXP:   //Beginnig EXP
            if(isNumber(c) || c == '+' || c == '-'){
                addToStr(token, c);
                state = DEC2;
            }
            else{
                ungetc(c, stdin);
                token->lexeme_type = UNDEF;
                return 10;
            }
            break;  //End EXP

        case DEC2:  //Beginning DEC2
            if(isNumber(c))
                addToStr(token, c);
            else{
                ungetc(c, stdin);
                end = 1;
            }
            break;  //End DEC2

        case STR:   //Beginning STR
            if(c == '\n' || c == EOF){
                token->lexeme_type = UNDEF;
                return 10;
            }
            addToStr(token, c);
            if(c == '"' && token->str.value[token->str.len-2] != '\\')
                end = 1;
            
            break;  //End STR
        
        default:    
            //ERROR
            break;
        }

    }

    token->lexeme_type = state;
    return 0;
}
