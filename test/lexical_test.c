#include "lexical_test.h"


int main(){
    char *testFile = "C:/Users/Daniel/Desktop/subjectsSources/IFJ/projopak/lexicalBranch/IFJ2023-lexical/test/lextest.txt";
    FILE *tf = fopen(testFile, "r");
    if(tf == NULL)
        printf("error reading file\n");
    int i = 0;
    int counter = 1;
    while(i != -1){
        lex_token token;
        printf("#%d\n", counter++);
        i = getNextToken(&token, tf);
        printf("Return value: %d\n", i);
        printLexToken(token);
    }
    fclose(tf);
    return 0;
}