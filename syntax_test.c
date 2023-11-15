#include "syntax_test.h"



int main()
{
    char *testFile = "C:/Users/Daniel/Desktop/subjectsSources/IFJ/projopak/IFJ2023-master/test/syntaxtest.txt";
    FILE *tf = fopen(testFile, "r");
    if(tf == NULL)
        printf("error reading file\n");
    
    ret_t res = analyze_syntax(tf);
    printf("res = %d\n", res);
    fclose(tf);
    return 0;
}
