#include <stdio.h>

#include "list_test.h"

#define CHECK_TEST(func) checkTest(#func, func)//take function name as a string

#define LIST_TEST

void checkTest(char *testName, int (*test)());


int main(int argc, char *argv[])
{
#ifdef LIST_TEST
    CHECK_TEST(list_add_test);
    CHECK_TEST(list_get_test);
    CHECK_TEST(list_insert_test);
    CHECK_TEST(list_delete_test);
#endif
    return 0;
}

void checkTest(char *testName, int (*test)())
{
    printf("--- Test %s ---\n", testName);
    printf("Output:\n");
    int result = test();
    printf("Result - ");
    if(result > 0)
        printf("PASSED");
    else if(result < 0)
        printf("FAILED");
    else
        printf("UNDEFINED");
    putchar('\n');
}

