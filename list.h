//Implementace překladače imperativního jazyka IFJ22
//Danil Domrachev (xdomra00)


#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

typedef struct list_elem
{
    void *elem;
    struct list_elem *nextElem;
} listElem;

typedef struct
{
    listElem *head;
    unsigned elSize;
    unsigned size;//elements counter
} List;

/*
description
    initializing list data structure
parameters
    elSize - element size (in bytes)
return value
    List pointer that has to be destroyed later
    NULL if memory allocation went wrong
*/
List* listInit(unsigned elSize);

/*
description
    free memory from List data structure
parameters
    lst - List pointer
return value
    -
*/
void listDestroy(List *lst);

/*
description
    add an element to the end
parameters
    lst - list where to add
    elem - void pointer to element to add
return value
    0 if succeeded
    1 if not enough memory
*/
ret_t listAdd(List *lst, void *elem);

/*
description
    deletes an i`s element of list
parameters
    lst - list where to delete from
    i - element index beginning with 0
return value
    0 if succeeded
    1 if there is no such index
*/
ret_t listDelete(List *lst, unsigned i);

/*
description
    gives an element with corresponding index
parameters
    lst - list where to get from
    i - element index beginning with 0
return value
    void pointer to element if found
    NULL if no element found
*/
void *listGet(List *lst, unsigned i);

/*
description
    inserts an element before the element with corresponding index
parameters
    lst - list where to insert
    elem - pointer to element to add
    i - element index
return value
    0 if succeeded
    1 if there is error
*/
ret_t listInsert(List *lst,  void* elem, unsigned i);


/*
description
    prints list elements if they are printable
parameters
    toString - function that can cast your element to string (char*)
return value
    -
*/
void printList(List *lst, const char* (*toString)(void*));

#endif
