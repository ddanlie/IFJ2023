#include "symtable.h"

unsigned long hash(char *str)
{
    unsigned long hash = HASHNUM;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    
    return hash;
}

symtable symtb_init()
{
    symtable newTable;
    newTable.symtb_arr = malloc(sizeof(symtb_node)*SYMTABLE_INIT_SIZE);
    newTable.capacity = SYMTABLE_INIT_SIZE;
    newTable.size = 0;
    for(int i = 0; i < newTable.capacity; i++)
    {
        newTable.symtb_arr[i].deleted = true;
    }
    return newTable;
}

void symtb_clear(symtable stb)
{
    for(int i = 0; i < stb.size; i++)
    {
        if(! stb.symtb_arr[i].deleted)
            clearSymtbToken(stb.symtb_arr[i].token);
    }
    free(stb.symtb_arr);
}

ret_t symtb_insert(symtable *stb, char *key, symtb_node value)
{
    unsigned long index = hash(key) % stb->size;
    if(stb->size+1 >= stb->capacity)
    {
        //realloc
    }
    int equal_keys;
    while((! stb->symtb_arr[index].deleted) && (0 != strcmp(stb->symtb_arr[index].token.id_name, key)))//while deleted item or item with the same key not found
        index = (index + 1) % stb->capacity;
    
    if(stb->symtb_arr[index].deleted)
        stb->size++;
    stb->symtb_arr[index].deleted = false;
    stb->symtb_arr[index] = value;
}

symtb_node symtb_find(symtable stb, char *key)
{

}

ret_t symtb_delete(symtable *stb, char *key)
{

}