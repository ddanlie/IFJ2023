//Implementace překladače imperativního jazyka IFJ22
//Danil Domrachev (xdomra00)


#include "list.h"

List* listInit(unsigned elSize)
{
    List *newList = malloc((size_t)sizeof(List));
    if(newList == NULL)
        return NULL;
    newList->head = NULL;
    newList->elSize = elSize;
    newList->size = 0;
    return newList;
}

void listDestroy(List *lst)
{
    listElem *tmp;
    while(lst->head != NULL)
    {
        tmp = lst->head->nextElem;
        free(lst->head->elem);
        free(lst->head);
        lst->head = tmp;
    }
    free(lst);
}

ret_t listAdd(List *lst, void *elem)
{
    listInsert(lst, elem, lst->size);
//    if(lst == NULL)
//        return 1;
//
//    if(lst->head == NULL)
//    {
//        lst->head = malloc(sizeof(listElem));
//        if(lst->head == NULL)
//            return 1;
//        lst->head->elem = malloc(lst->elSize);
//        memcpy(lst->head->elem, elem, lst->elSize);
//        lst->head->nextElem = NULL;
//    }
//    else
//    {
//        listElem *tmp = lst->head;
//        while(tmp->nextElem != NULL)
//            tmp = tmp->nextElem;
//        tmp->nextElem = malloc(sizeof(listElem));
//        if(tmp->nextElem == NULL)
//            return 1;
//        tmp->nextElem->nextElem = NULL;
//        tmp->nextElem->elem = malloc(lst->elSize);
//        memcpy(tmp->nextElem->elem, elem, lst->elSize);
//    }
//    lst->size++;
    return 0;
}

ret_t listDelete(List *lst, unsigned i)
{
    if(lst->size <= i)
        return 1;
    listElem *tmpElem;
    if(i == 0)
    {
        tmpElem = lst->head->nextElem;
        free(lst->head->elem);
        free(lst->head);
        lst->head = tmpElem;
    }
    else
    {
        unsigned tmpIdx = 0;
        tmpElem = lst->head;
        while(tmpIdx+1 != i)
        {
            tmpElem = tmpElem->nextElem;
            tmpIdx++;
        }
        listElem *toDelete = tmpElem->nextElem;
        tmpElem->nextElem = toDelete->nextElem;
        free(toDelete->elem);
        free(toDelete);
    }
    lst->size--;
    return 0;
}

void *listGet(List *lst, unsigned i)
{
    if(lst->size <= i)
        return NULL;
    listElem *tmpElem = lst->head;
    unsigned tmpIndex = 0;
    while(tmpIndex != i)
    {
        tmpElem = tmpElem->nextElem;
        tmpIndex++;
    }
    return tmpElem->elem;
}

ret_t listInsert(List *lst, void* elem, unsigned i)
{
    if(lst->size < i)
        return 1;
    listElem *newElem = malloc(sizeof(listElem));
    newElem->elem = malloc(lst->elSize);
    memcpy(newElem->elem, elem, lst->elSize);
    if(i == 0)
    {
        newElem->nextElem = lst->head;
        lst->head = newElem;
    }
    else
    {
        unsigned tmpIdx = 0;
        listElem *tmpElem = lst->head;
        while(tmpIdx+1 != i)//+1 because we can add to the end with to not existing index
        {
            tmpElem = tmpElem->nextElem;
            tmpIdx++;
        }
        newElem->nextElem = tmpElem->nextElem;
        tmpElem->nextElem = newElem;
    }
    lst->size++;
    return 0;
}

void printList(List *lst, const char* (*toString)(void*))
{
    listElem *listEl = lst->head;
    unsigned counter = 0;
    printf("List size = %u\n", lst->size);
    while(listEl != NULL)
    {
        printf("List element #%d:\n%s\n", counter++, toString(listEl->elem));
        listEl = listEl->nextElem;
    }
}








