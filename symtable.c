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
    else
    {
        newTable.capacity = init_size;
        newTable.size = 0;
        newTable.local_level = -1;
        for(int i = 0; i < newTable.capacity; i++)
        {
            newTable.symtb_arr[i].deleted = true;
        }
    }
    return newTable;
}

void symtb_clear(symtable stb)
{
    for(int i = 0; i < stb.size; i++)
    {
        if(! stb.symtb_arr[i].deleted)
            clearSymtbToken(&stb.symtb_arr[i].token);
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


void initSymtbToken(symtb_token *token)
{
    token->id_name = NULL;
    token->type = UNDEF_ID;
    token->lit_type = UNDEF_TYPE;
    token->funcArgTypes = NULL;
    token->funcArgnames = NULL;
    token->funcLocalArgnames = NULL;
    token->funcArgsSize = 0;
    token->initialized = false;
}

void clearSymtbToken(symtb_token *token)
{
    if(token->id_name != NULL)
    {
        free(token->id_name);
        token->id_name = NULL;
    }
    
    if(token->funcArgTypes != NULL)
    {
        free(token->funcArgTypes);
        token->funcArgTypes = NULL;
    }
    
    for(int i = 0; i < token->funcArgsSize; i++)
    {
        free(token->funcArgnames[i]);
        free(token->funcLocalArgnames[i]);
    }
    if(token->funcArgnames != NULL)
    {
        free(token->funcArgnames);
        token->funcArgnames = NULL;
    }
    if(token->funcLocalArgnames != NULL)
    {
        free(token->funcLocalArgnames);
        token->funcLocalArgnames = NULL;
    }
    
}



void checkArgsSetSize(symtb_token *dst)
{
    if(dst->funcLocalArgnames[dst->funcArgsSize] != NULL
       && dst->funcArgnames[dst->funcArgsSize] != NULL
       && dst->funcArgTypes[dst->funcArgsSize] != UNDEF_TYPE)
        dst->funcArgsSize++;
}

void symtbTokenCopyName(symtb_token *dst, lex_token src)
{
    if(dst->id_name == NULL)
        dst->id_name = malloc(sizeof(char)*(src.str.len+1));
    if(strlen(dst->id_name) < src.str.len)
        dst->id_name = realloc(dst->id_name, sizeof(char)*(src.str.len+1));
    strcpy(dst->id_name, src.str.value);
}

void symtbTokenAddArgName(symtb_token *dst,  lex_token src)
{
    static int args_capacity;
    if(dst->funcArgnames == NULL)
    {
        args_capacity = 10;
        dst->funcArgnames = malloc(sizeof(char *) * args_capacity);
        for(int i = 0; i < args_capacity; i++)
            dst->funcArgnames[i] = NULL;
    }
    
    dst->funcArgnames[dst->funcArgsSize] = malloc(sizeof(char) * (src.str.len + 1));
    strcpy(dst->funcArgnames[dst->funcArgsSize], src.str.value);
    
    checkArgsSetSize(dst);
    
    if(dst->funcArgsSize == args_capacity)
    {
        args_capacity *= 2;
        dst->funcArgnames = realloc(dst->funcArgnames, sizeof(char*)*args_capacity);
        for(int i = dst->funcArgsSize; i < args_capacity; i++)
            dst->funcArgnames[i] = NULL;
    }
}

void symtbTokenAddArgType(symtb_token *dst, lex_token src)
{
    static int args_capacity;
    if(dst->funcArgTypes == NULL)
    {
        args_capacity = 10;
        dst->funcArgTypes = malloc(sizeof(literal_type)*args_capacity);
        for(int i = 0; i < args_capacity; i++)
            dst->funcArgTypes[i] = UNDEF_TYPE;
    }
    
    //2 cases: 1. when calling function, 2. when defining function
    switch(src.lexeme_type)
    {
        case INT:
        case INT_LIT:
            dst->funcArgTypes[dst->funcArgsSize] = INT_TYPE;
            break;
        case DOUBLE:
        case DOUBLE_LIT:
            dst->funcArgTypes[dst->funcArgsSize] = DOUBLE_TYPE;
            break;
        case STRING:
        case STRING_LIT:
            dst->funcArgTypes[dst->funcArgsSize] = STRING_TYPE;
            break;
        case NILSTRING:
            dst->funcArgTypes[dst->funcArgsSize] = NSTRING_TYPE;
            break;
        case NILDOUBLE:
            dst->funcArgTypes[dst->funcArgsSize] = NDOUBLE_TYPE;
            break;
        case NILINT:
            dst->funcArgTypes[dst->funcArgsSize] = NINT_TYPE;
            break;
        case NIL:
            dst->funcArgTypes[dst->funcArgsSize] = UNDEF_TYPE;
            break;
        default:
            return;
    }
    checkArgsSetSize(dst);
    
    if(dst->funcArgsSize == args_capacity)
    {
        args_capacity *= 2;
        dst->funcArgTypes = realloc(dst->funcArgTypes, sizeof(literal_type) * args_capacity);
    }
}

void symtbTokenAddLocalArgName(symtb_token *dst,  lex_token src)
{
    static int args_capacity;
    if(dst->funcLocalArgnames == NULL)
    {
        args_capacity = 10;
        dst->funcLocalArgnames = malloc(sizeof(char *) * args_capacity);
        for(int i = 0; i < args_capacity; i++)
            dst->funcLocalArgnames[i] = NULL;
    }
    
    dst->funcLocalArgnames[dst->funcArgsSize] = malloc(sizeof(char) * (src.str.len + 1));
    strcpy(dst->funcLocalArgnames[dst->funcArgsSize], src.str.value);
    
    checkArgsSetSize(dst);
    
    if(dst->funcArgsSize == args_capacity)
    {
        args_capacity *= 2;
        dst->funcLocalArgnames = realloc(dst->funcLocalArgnames, sizeof(char*)*args_capacity);
        for(int i = dst->funcArgsSize; i < args_capacity; i++)
            dst->funcLocalArgnames[i] = NULL;
    }
}

