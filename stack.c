//Implementace překladače imperativního jazyka IFJ22
//Danil Domrachev (xdomra00)

#include "stack.h"

Stack* stackInit(unsigned elSize)
{
    Stack *newStack = malloc(sizeof(Stack));
    newStack->lst = listInit(elSize);
    newStack->semiPopCounter = 0;
    return newStack;
}

void stackDestroy(Stack *st)
{
    listDestroy(st->lst);
    free(st);
}

ret_t stackPush(Stack *st, void *elem)
{
    if(st->lst->size == 0)
        return listAdd(st->lst, elem);
    if(st->semiPopCounter != 0)
        st->semiPopCounter++; 
    return listInsert(st->lst, elem, 0);
}

void* stackPop(Stack *st)
{
    if(st->lst->size == 0)
        return NULL;
    void* result = malloc(st->lst->elSize);
    memcpy(result, listGet(st->lst, 0), st->lst->elSize);
    listDelete(st->lst, 0);
    if(st->semiPopCounter != 0)
        st->semiPopCounter--;
    return result;
}

void* stackSemiPop(Stack *st)
{
    if(st->semiPopCounter >= st->lst->size)
        st->semiPopCounter = 0;
    void *result = listGet(st->lst, st->semiPopCounter);
    st->semiPopCounter++;
    return result;
}

void stackResetSemiPop(Stack *st)
{
    st->semiPopCounter = 0;
}

