//Implementace překladače imperativního jazyka IFJ23
//Danil Domrachev (xdomra00)


#ifndef STACK_H
#define STACK_H

#include "list.h"

typedef struct
{
    List *lst;
    unsigned semiPopCounter;
} Stack;

/*
description
    initializing stack data structure
parameters
    elSize - element size (in bytes)
return value
    Stack pointer that has to be destroyed later
    NULL if memory allocation went wrong
*/
Stack* stackInit(unsigned elSize);

/*
description
    destroying stack data structure
    if you use structures with pointers inside, take all elements with 'stackSemiPop' and free them first, then use this function
parameters
    st - stack pointer to destroy
return value
    -
*/
void stackDestroy(Stack *st);

/*
description
    push element to the stack
parameters
    st - stack where push to
    elem - void pointer to element push to
return value
    0 if succeeded
    1 if there is error
*/
ret_t stackPush(Stack *st, void *elem);

/*
description
    get and delete element from the stack
parameters
    st - stack pointer where pop from
return value
    void pointer to the first element (needs to be FREED later)
    NULL if stack is empty
*/
void* stackPop(Stack *st);

/*
description
    get but do not delete element from the stack
    after last semi popped element semi pop function begins
    with the first stack element again
parameters
    st - stack pointer where "semi-pop" from
return value
    void pointer to the first element (doesnt need to be freed)
    NULL if stack is empty
*/
void* stackSemiPop(Stack *st);


/*
description
    begin pops from 0th index
parameters
    st - stack pointer
return value
    -
*/
void stackResetSemiPop(Stack *st);




#endif
