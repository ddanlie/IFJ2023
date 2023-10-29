#include"lexical_test.h"

void vypisTokenu(int i, lex_token token){
    printf("Navratova hodnota: %d\n", i);

    printf("lexeme_type: %d\n", token.lexeme_type);
    printf("str_value: ");
    for(int i = 0; i < token.str.len; i++)
        printf("%c", token.str.value[i]);
    printf("\n");
    printf("int_value: %d\n", token.int_value);
    printf("double_value: %f\n\n", token.double_value);
}

int main(){
    char c = 0;
    while(1){
        c = getchar();
        if(c == EOF)
            break;
        ungetc(c, stdin);

        lex_token token;
        int i = getNextToken(&token);
        vypisTokenu(i, token); 
        clearLexToken(&token);
    }
    return 0;
}