#include "list_test.h"

int list_add_test()
{
    List *lst = listInit(sizeof(int));
    int arg = 11;
    listAdd(lst, (void*)&arg);
    arg = 12;
    listAdd(lst, (void*)&arg);
    arg = 13;
    listAdd(lst, (void*)&arg);
    arg = 14;
    listAdd(lst, (void*)&arg);
    //printList(lst, intToString);
    return 0;
}

int list_get_test()
{
    List *lst = listInit(sizeof(int));
    int arg = 11;
    listAdd(lst, (void*)&arg);
    arg = 12;
    listAdd(lst, (void*)&arg);
    arg = 13;
    listAdd(lst, (void*)&arg);
    arg = 14;
    listAdd(lst, (void*)&arg);
    
    int elem0 = *(int*)listGet(lst, 0);
    bool res = (elem0 == 11);
    int elemRand = *(int*)listGet(lst, 2);
    res &= (elemRand == 13);
    int elemLast = *(int*)listGet(lst, 3);
    res &= (elemLast == 14);
    printf("Elem 0: %d\nLast elem: %d\nRandom elem: %d\n", elem0, elemLast, elemRand);
    if(res)
        return 1;
    else
        return -1;
}

//supposes that list_get_test is passed
int list_insert_test()
{
    List *lst = listInit(sizeof(int));
    int arg = 11;
    listAdd(lst, (void*)&arg);
    arg = 12;
    listAdd(lst, (void*)&arg);
    arg = 13;
    listAdd(lst, (void*)&arg);
    arg = 14;
    listAdd(lst, (void*)&arg);
    
    arg = 999;
    listInsert(lst, (void*)&arg, 0);//11,12,13,14 --> 999,11,12,13,14
    arg = 1010;
    listInsert(lst, (void*)&arg, lst->size-1);//999,11,12,13,14 --> 999,11,12,13,1010,14
    arg = 2002;
    listInsert(lst, (void*)&arg, lst->size);//999,11,12,13,1010,14 --> 999,11,12,13,1010,14,2002
    arg = 8800;
    listInsert(lst, (void*)&arg, 5);//999,11,12,13,1010,14,2002 --> //999,11,12,13,1010,8800,14,2002
    
    //result - 0: 999 4: 1010 5: 8800 7:2002
    int tmp = *(int*)listGet(lst, 0);
    bool result = (tmp == 999);
    tmp = *(int*)listGet(lst, 4);
    result &= (tmp == 1010);
    tmp = *(int*)listGet(lst, 5);
    result &= (tmp == 8800);
    tmp = *(int*)listGet(lst, 7);
    result &= (tmp == 2002);
    if(result)
        return 1;
    else
        return -1;
}

//supposes that list_get_test is passed
int list_delete_test()
{
    List *lst = listInit(sizeof(int));
    int arg = 11;
    listAdd(lst, (void*)&arg);
    arg = 12;
    listAdd(lst, (void*)&arg);
    arg = 13;
    listAdd(lst, (void*)&arg);
    arg = 14;
    listAdd(lst, (void*)&arg);
    arg = 15;
    listAdd(lst, (void*)&arg);
    
    listDelete(lst, 0);//11,12,13,14,15 --> 12,13,14,15
    int tmp = *(int*)listGet(lst, 0);
    bool result = (tmp == 12);
    listDelete(lst, lst->size-1);//12,13,14,15 --> 12,13,14
    tmp = *(int*) listGet(lst, lst->size-1);
    result &= (tmp == 14);
    listDelete(lst, 1);//12,13,14 --> 12,14
    tmp = *(int*)listGet(lst, 1);
    result &= (tmp == 14);
    if(result)
        return 1;
    else
        return -1;
}