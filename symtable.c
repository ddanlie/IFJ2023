#include "symtable.h"

unsigned long hash(char *str, int capacity)
{
    unsigned long hash = HASHNUM;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    
    return hash % capacity;
}

symtable symtb_init(int init_size)
{
    symtable newTable;
    newTable.symtb_arr = malloc(sizeof(symtb_node)*init_size);
    if(newTable.symtb_arr == NULL)
    {
        newTable.symtb_arr = NULL;
        newTable.capacity = -1;
        newTable.size = -1;
    }
    newTable.capacity = init_size;
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

ret_t symtb_insert(symtable *stb, char *key, symtb_token value)
{
    if(stb->size+1 > stb->capacity)
    {
        //realloc
        symtable old_table = *stb;
        int new_capacity = old_table.capacity * 2;
        
        *stb = symtb_init(new_capacity);
        if(stb->symtb_arr == NULL)
        {
            *stb = old_table;
            return -1;
        }
        for(int i = 0; i < old_table.capacity; i++)
        {
            symtb_insert(stb, old_table.symtb_arr[i].token.id_name, old_table.symtb_arr[i].token);
        }
        free(old_table.symtb_arr);
    }
    int index = hash(key, stb->capacity);
    while((! stb->symtb_arr[index].deleted) && (0 != strcmp(stb->symtb_arr[index].token.id_name, key)))//while deleted item or item with the same key not found
        index = (index + 1) % stb->capacity;
    
    if(stb->symtb_arr[index].deleted)
        stb->size++;
    stb->symtb_arr[index].deleted = false;
    stb->symtb_arr[index].token = value;
}

symtb_node symtb_find(symtable stb, char *key, int *index_found)
{
    int index = hash(key, stb.capacity);
    int counter = 0;
    while((counter < stb.capacity) &&  //keed doing while not all elements viewd OR not deleted element which responds to the key is not found 
            (0 != strcmp(stb.symtb_arr[index].token.id_name, key) || stb.symtb_arr[index].deleted == true))
    {
        index = (index+1) % stb.capacity;
        counter++;
    }
    symtb_node result;
    result.deleted = true;
    if(counter == stb.capacity)
        return result;//nothing found, deleted = true, token is undefined
    result.token = stb.symtb_arr[index].token;
    result.deleted = false;
    if(index_found != NULL)
        *index_found = index; 
    return result;
}

ret_t symtb_delete(symtable *stb, char *key)
{
    int index;
    symtb_node elem = symtb_find(*stb, key, &index);
    if(elem.deleted)
        return -1;
    stb->symtb_arr[index].deleted = true;
    stb->size--;
    return 0;
}


void symtb_print(symtable stb)
{
    printf("------------------------------");
    printf("Size: %d\nCapacity: %d\n", stb.size, stb.capacity);
    for(int i = 0; i < stb.capacity; i++)
    {
        printf("Element #%d\n:", i);
        if(stb.symtb_arr[i].deleted)
        {
            printf("deleted");
            continue;
        }
        printf("ID NAME: %s\n", stb.symtb_arr[i].token.id_name);
        printf("ID TYPE: %d\n", stb.symtb_arr[i].token.type);
    }
}