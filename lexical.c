#include "lexical.h"

void strZerosFill(char *str, int start_index, int end_index)
{
    for(int i = start_index; i <= end_index; i++)
        str[i] = '\0';
}

int initLexToken(lex_token* token)
{
    clearLexToken(token);
    
    token->lexeme_type = 0;
    token->double_value = 0.f;
    token->int_value = 0;

    token->str.value = malloc(sizeof(char) * INIT_STR_SIZE);
    if(token->str.value == NULL)
        return 1;
    token->str.capacity = INIT_STR_SIZE;
    token->str.len = 0;
    strZerosFill(token->str.value, 0, token->str.capacity-1);
    return 0;
}

void clearLexToken(lex_token *token)
{
    token->lexeme_type = UNDEF;
    token->int_value = 0;
    token->double_value = 0;
    
    if(token->str.value != NULL)
    {
        free(token->str.value);
        token->str.value = NULL;
    }
    token->str.value = NULL;
    token->str.capacity = 0;
    token->str.len = 0;
}

void copyLexToken(lex_token src, lex_token *dst)
{
    clearLexToken(dst);
    *dst = src;
    dst->str.value = malloc(sizeof(char) * (src.str.len + 1));
    dst->str.capacity = src.str.len + 1;
    strcpy(dst->str.value, src.str.value);
}

int checkKeyword(lex_token token){
    //IF, WHILE, ELSE, FUNC, LET, RETURN, VAR
    if(!strcmp(token.str.value, "if"))
        return IF;
    else if(!strcmp(token.str.value, "while"))
        return WHILE;
    else if(!strcmp(token.str.value, "else"))
        return ELSE;
    else if(!strcmp(token.str.value, "func"))
        return FUNC;
    else if(!strcmp(token.str.value, "let"))
        return LET;
    else if(!strcmp(token.str.value, "return"))
        return RETURN;
    else if(!strcmp(token.str.value, "var"))
        return VAR;

    return -1;//-1 because 0 can be a 0th lexeme enum
}

int checkType(lex_token token){
    //INT, DOUBLE, STRING, NIL
    if(!strcmp(token.str.value, "Int"))
        return INT;
    else if(!strcmp(token.str.value, "Double"))
        return DOUBLE;
    else if(!strcmp(token.str.value, "String"))
        return STRING;
    else if(!strcmp(token.str.value, "nil"))
        return NIL;
    else if(!strcmp(token.str.value, "Int?"))
        return NILINT;
    else if(!strcmp(token.str.value, "Double?"))
        return NILDOUBLE;
    else if(!strcmp(token.str.value, "String?"))
        return NILSTRING;

    return -1;//-1 because 0 can be a 0th lexeme enum
}

int addToStr(lex_token* token, char c){
    if(token->str.len == token->str.capacity-1){ // don't forget about '\0'
        token->str.capacity *= 2;
        token->str.value = (char *)realloc(token->str.value, token->str.capacity);
        if(token->str.value == NULL)
            return 1;
        strZerosFill(token->str.value, token->str.len, token->str.capacity-1);
    }

    token->str.value[token->str.len++] = c;
    return 0;
}



ret_t getNextToken(lex_token* token, FILE *input_file)
{
    //states that lexical automata can take (see documentation)
    typedef enum q_enum
    {
        S,          // Starting state
        IDF,        // Identifier state
        ID1S,/* _ */
        KWRD,       // Keyword state
        TYPE,       // Type check state
        RARROWS,
        DECF2,       // Decimal state (probably for floats)
        DECF,       // Another decimal state
        D2,         // State after first digit (could be used for multi-digit numbers)
        EXP,        // Exponent state for floating-point numbers
        INTS,        // Integer state
        SIGN,       // Sign state (+ or -)
        COM1,       // Comment start state (maybe for /* style comments)
        COM2,       // Comment middle state
        COM3,       // Comment end state
        LSTR,       // Large string state
        STR,/* " "*/
        STR2,
        FUNCS,       // Function state (possibly for function or lambda constructs)
        LSTR2,      // Second literal string state
        BCOM2,      // Comment state (possibly for // style comments)
        BCOM3,      // Another comment state
        BCOM4,      // Yet another comment state
        BCOM5,      // Another comment state
        ASCII,
        ESC1,       // Escape sequence state
        ESC2,       // Another escape sequence state
        ESC3,       // Yet another escape sequence state
        ESC4,       // Another escape sequence state
        ESC5,        // Another escape sequence state
        QS/* ?? */,
        LES /* < */,
        GTS /* > */,
        EXCLAMS/* ! */, 
        ASS /* = */,
        CHECK,
    } State;

    if(initLexToken(token))
        return ANOTHER_ERROR;
    //automaton help variables
    int end = 0;
    int large_str_flag = 0;
    //automaton variables end
    int c;
    int state = S;
    //flags
    eoln_flag = false;
    end_of_file_flag = false;
    while(!end){
        c = fgetc(input_file);

        switch (state)  //State switch
        {
        case S:    //Beginning S
            switch (c)
            {

                
                case ' ':
                case '\t':
                case '\r':  //Staying in state S
                    state = S;
                    break;
    
                case '\n':
                    eoln_flag = true;
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
                
                case '?':
                    state = QS;
                    break;
    
                case '_':
                    addToStr(token, (char)c);
                    state = ID1S;
                    break;
                
                case '/':
                    state = COM1;
                    break;
    
                case '"':
                    state = STR;
                    break;
    
                case '*':
                    token->lexeme_type = MUL;
                    end = 1;
                    break;
    
                case '-':
                    state = RARROWS;
                    break;
    
                case '+':
                    token->lexeme_type = PLUS;
                    end = 1;
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
                    if(isAlpha((char)c))
                        state = IDF;
                    else if(isDigit((char)c))
                        state = INTS;
                    else if(c == EOF){
                        end_of_file_flag = true;
                        return -1;
                    }
                    else{
                        token->lexeme_type = UNDEF;
                        return LEXICAL_ERROR;
                    }
                    addToStr(token, (char)c);
                    break;
            }
            break;  //End S
            
            
        case RARROWS:
            if(c == '>')
            {
                token->lexeme_type = RARROW;
            }
            else
            {
                token->lexeme_type = MINUS;
                ungetc(c, input_file);
                end = 1;
            }
            break;
        
        case EXCLAMS:    //Beginning EXCLAMS '!'
            if(c == '='){
                token->lexeme_type = NEQ;
                end = 1;
            }
            else{
                token->lexeme_type = EXCLAM;
                ungetc(c, input_file);
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
                ungetc(c, input_file);
                end = 1;
            }
            break; //End AS '='

        case GTS:    //Beginning GTS '>'
            if(c == '='){
                token->lexeme_type = GEQ;
                end = 1;
            }
            else{
                token->lexeme_type = GT;
                ungetc(c, input_file);
                end = 1;
            }
            break; //End GTS '>'

        case LES:    //Beginning LES'<'
            if(c == '='){
                token->lexeme_type = LEQ;
                end = 1;
            }
            else{
                token->lexeme_type = LE;
                ungetc(c, input_file);
                end = 1;
            }
            break; //End LES '<'

        case QS:
            if(c == '?'){
                token->lexeme_type = QQ;
            }
            else{
                ungetc(c, input_file);
            }
            end = 1;
            break;

        case ID1S:
            if(isAlpha((char)c) || isDigit((char)c) || c == '_'){
               addToStr(token, (char)c);
               state = IDF;
            }
            else{
                token->lexeme_type = ID1;
                ungetc(c, input_file);
                end = 1;
            }
            break;

        case IDF:
            if(!(isAlpha((char)c) || isDigit((char)c) || c == '_')){
                state = CHECK;
                ungetc(c, input_file);
            }
            else
                addToStr(token, (char)c);
            break;

        case CHECK:
        {
            int res = checkKeyword(*token);
            if(res != -1){//if it is keyword
                ungetc(c, input_file);
                token->lexeme_type = res;
            }
            else{//if it is NOT a keyword
                res = checkType(*token);
                if(res != -1){//if it is a type
                    if(c == '?'){//then maybe it is a nillable type
                        addToStr(token, (char)c);
                        res = checkType(*token);
                    }
                    else//or not
                        ungetc(c, input_file);
                    
                    token->lexeme_type = res;
                }
                else{//if it is not a type
                    ungetc(c, input_file);
                    token->lexeme_type = ID;
                }
            }
            end = 1;
            break;
        }
        case COM1:
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
                ungetc(c, input_file);
                end = 1;
                break;
            }
            break;

        case COM2:
            if(c == '\n' || c == EOF){
                state = S;//COM3
            }
            break;

        case BCOM2: //Beginning BCOM2
        {
            static int opened = 1;
            if(c == '/')
            {
                c = fgetc(input_file);
                if(c == '*')
                    opened++;
                else
                    ungetc(c, input_file);
            }
            else if(c == '*')
            {
                c = fgetc(input_file);
                if(c == '/')
                {
                    opened--;
                    if(opened == 0)
                    {
                        opened = 1;
                        state = S;
                    }
                }
                else
                    ungetc(c, input_file);
            }
            else if(c == EOF)
            {
                opened = 1;
                return LEXICAL_ERROR;
            }
            break;  //End BCOM2
        }
        case INTS:   //Beginnig INTS
            if(isDigit((char)c))
                addToStr(token, (char)c);
            else if(c == '.'){
                addToStr(token, (char)c);
                state = D2;
            }
            else if(c == 'e' || c == 'E'){
                addToStr(token, (char)c);
                state = EXP;
            }
            else{
                token->lexeme_type = INT_LIT;
                token->int_value = atoi(token->str.value);
                ungetc(c, input_file);
                end = 1;
            }
            break;  //End INTS

        case D2:    //Beginning D2
            if(isDigit((char)c)){
                addToStr(token, (char)c);
                state = DECF;
            }
            else{
                ungetc(c, input_file);
                token->lexeme_type = UNDEF;
                return LEXICAL_ERROR;
            }
            break;  //End D2

        case DECF:  //Beginning DECF
            if(isDigit((char)c))
                addToStr(token, (char)c);
            else if(c == 'e' || c == 'E'){
                addToStr(token, (char)c);
                state = EXP;
            }
            else{
                token->lexeme_type = DOUBLE_LIT;
                token->double_value = atof(token->str.value);
                ungetc(c, input_file);
                end = 1;
            }
            break;  //End DECF

        case EXP:   //Beginnig EXP
            if(isDigit((char)c)){
                addToStr(token, (char)c);
                state = DECF2;
            }
            else if(c == '+' || c == '-'){
                addToStr(token, (char)c);
                state = SIGN;
            }
            else{
                ungetc(c, input_file);
                token->lexeme_type = UNDEF;
                return LEXICAL_ERROR;
            }
            break;  //End EXP

        case SIGN:
            if(isDigit((char)c)){
                addToStr(token, (char)c);
                state = DECF2;
            }
            else{
                ungetc(c, input_file);
                token->lexeme_type = UNDEF;
                return LEXICAL_ERROR;
            }
            break;

        case DECF2:  //Beginning DECF2
            if(isDigit((char)c))
                addToStr(token, (char)c);
            else{
                token->lexeme_type = DOUBLE_LIT;
                token->double_value = atof(token->str.value);
                ungetc(c, input_file);
                end = 1;
            }
            break;  //End DECF2

        case STR:   //Beginning STR
            if(c == '"')
                state = LSTR;
            else
            if(c == '\\')
            {
                state = ESC1;//we add '\' later, see ESC1
            }
            else
            if(c > ASCII_REPR_START)
            {
                addToStr(token, (char)c);
                state = ASCII;
            }
            else
            {
                token->lexeme_type = UNDEF;
                return LEXICAL_ERROR;
            }
            
            break;  //End STR
            
        case ASCII:
        {
            if(c == '"')
                state = STR2;
            else if(c == '\\')//must be before if(c > ASCII_REPR_START) condition
                state = ESC1;//we add '\' later, see ESC1
            else if(c > ASCII_REPR_START)
                addToStr(token, (char)c);
            else
            {
                if(large_str_flag)
                {
                    ungetc(c, input_file);
                    state = FUNCS;
                } else
                {
                    token->lexeme_type = UNDEF;
                    return LEXICAL_ERROR;
                }
            }
    
            break;
        }
            
        case ESC1:
            switch(c)
            {
                //added \r \n ... see STR/ASCII state
                case 'n':
                    addToStr(token, '\n');
                    state = ESC2;
                    break;
                case 'r':
                    addToStr(token, '\r');
                    state = ESC2;
                    break;
                case 't':
                    addToStr(token, '\t');
                    state = ESC2;
                    break;
                case '"':
                    addToStr(token, '\"');
                    state = ESC2;
                    break;
                case '\\':
                    addToStr(token, '\\');
                    state = ESC2;
                    break;
                case 'u':
                    state = ESC3;
                    break;
                default:
                    token->lexeme_type = UNDEF;
                    return LEXICAL_ERROR;
            }
            break;
    
        case ESC2:
            if(c == '"')
                state = STR2;
            else
            if(c == '\\')//must be before if(c > ASCII_REPR_START) condition
                state = ESC1;//we add '\' later, see ESC1
            else
            if(c > ASCII_REPR_START)
            {
                addToStr(token, (char)c);
                state = ASCII;
            }
            else
            {
                if(large_str_flag)
                {
                    ungetc(c, input_file);
                    state = FUNCS;
                }
                else
                {
                    token->lexeme_type = UNDEF;
                    return LEXICAL_ERROR;
                }
            }

            break;

        case ESC3:
            if(c == '{')
                state = ESC4;
            else
            {
                token->lexeme_type = UNDEF;
                return LEXICAL_ERROR;
            }
            break;

        case ESC4:
        {
            static char value[MAX_ESCAPE_HEX_LEN + 1];
            static int valueIndex = 0;//attention. correctly set this variable before going to next state or returning from function
            if(valueIndex == 0)
                strZerosFill(value, 0, MAX_ESCAPE_HEX_LEN);
    
            if(c == '}' && (valueIndex > 0))
            {
                int converted = strtol(value, NULL, 16);
                addToStr(token, (char) converted);
                valueIndex = 0;
                state = ESC5;
            } else if(isHex((char)c) && (valueIndex < MAX_ESCAPE_HEX_LEN))
            {
                value[valueIndex++] = (char)c;
            } else
            {
                token->lexeme_type = UNDEF;
                valueIndex = 0;
                return LEXICAL_ERROR;
            }
            break;
        }

        case ESC5:
            if(c == '"')
                state = STR2;
            else
            if(c == '\\')
            {
                state = ESC1;//we add '\' later, see ESC1
            }
            else
            if(c > ASCII_REPR_START)
            {
                addToStr(token, (char)c);
                state = ASCII;
            }
            else
            {
                if(large_str_flag)
                {
                    ungetc(c, input_file);
                    state = FUNCS;
                }
                else
                {
                    token->lexeme_type = UNDEF;
                    return LEXICAL_ERROR;
                }
            }
            break;
    
    
        case STR2:
            if(large_str_flag)
            {
                ungetc('"', input_file);//return quote previous state have read
                ungetc(c, input_file);
                state = FUNCS;
            }
            else
            {
                token->lexeme_type = STRING_LIT;
                ungetc(c, input_file);
                end = 1;
            }

            break;
            
        case LSTR:
            if(c == '"')
            {
                large_str_flag = 1;
                c = fgetc(input_file);
                if(c != '\n')//skip first Large String EOLN
                    ungetc(c, input_file);
                state = FUNCS;
            }
            else
            {
                token->lexeme_type = STRING_LIT;
                ungetc(c, input_file);
                end = 1;
            }
            break;
        
        case FUNCS:
        {
            static int quotesTogetherCount = 0;//e.g. "" - 2 quotes in a row
            if(c == '"')
            {
                addToStr(token, (char) c);
                quotesTogetherCount++;
                if(quotesTogetherCount == 3)
                {
                    quotesTogetherCount = 0;
                    strZerosFill(token->str.value, token->str.len - 3,
                                 token->str.len - 1);//clean previous quotes as they are not part of a large string
                    token->str.len -= 3;
                    if(token->str.len > 0)
                        if(token->str.value[token->str.len - 1] == '\n')//EOLN before ending quotes does not count
                            token->str.value[--(token->str.len)] = '\0';
            
                    token->lexeme_type = STRING_LIT;
                    large_str_flag = 0;
                    end = 1;
                }
            } else if(c == '\\')
            {
                quotesTogetherCount = 0;
                state = ESC1;
            } else if(c > ASCII_REPR_START)
            {
                addToStr(token, (char) c);
                quotesTogetherCount = 0;
                state = ASCII;
            } else //c <= ASCII_REPR_START
            {
                addToStr(token, (char) c);
                quotesTogetherCount = 0;
            }
            break;
        }
        default:
            //ERROR
            fprintf(stderr, "Unknown lexical automaton state");
            break;
        }
    }
    return 0;
}


void printLexeme(lexeme l)
{
    switch (l) {
        case UNDEF: printf("UNDEF\n"); break;
        case INT: printf("INT\n"); break;
        case DOUBLE: printf("DOUBLE\n"); break;
        case STRING: printf("STRING\n"); break;
        case NIL: printf("NIL\n"); break;
        case NILINT: printf("INT?\n"); break;
        case NILDOUBLE: printf("DOUBLE?\n"); break;
        case NILSTRING: printf("STRING?\n"); break;
        case IF: printf("IF\n"); break;
        case WHILE: printf("WHILE\n"); break;
        case ELSE: printf("ELSE\n"); break;
        case FUNC: printf("FUNC\n"); break;
        case LET: printf("LET\n"); break;
        case RETURN: printf("RETURN\n"); break;
        case VAR: printf("VAR\n"); break;
        case QQ: printf("??\n"); break;
        case EXCLAM: printf("!\n"); break;
        case EQ: printf("==\n"); break;
        case LEQ: printf("<=\n"); break;
        case GEQ: printf(">=\n"); break;
        case NEQ: printf("!=\n"); break;
        case MINUS: printf("-\n"); break;
        case PLUS: printf("+\n"); break;
        case LE: printf("<\n"); break;
        case GT: printf(">\n"); break;
        case AS: printf("=\n"); break;
        case MUL: printf("*\n"); break;
        case DIV: printf("/\n"); break;
        case ID1: printf("_\n"); break;
        case ID: printf("ID\n"); break;
        case LBR1: printf("(\n"); break;
        case RBR1: printf(")\n"); break;
        case LBR2: printf("{\n"); break;
        case RBR2: printf("}\n"); break;
        case COLON: printf(":\n"); break;
        case COMMA: printf(",\n"); break;
        case RARROW: printf("->\n"); break;
        case INT_LIT: printf("INT_LIT\n"); break;
        case DOUBLE_LIT: printf("DOUBLE_LIT\n"); break;
        case STRING_LIT: printf("STRING_LIT\n"); break;
        default: printf("Unknown lexeme\n"); break;
    }
}


void printLexToken(lex_token token){
    printf("_________TOKEN_________\n");
    printf("Lexeme type: ");
    printLexeme(token.lexeme_type);
    
    printf("string value: %s\n", token.str.value);
    
    printf("int value: %d\n", token.int_value);
    
    printf("double value: %f\n", token.double_value);
    printf("_______________________\n");
}