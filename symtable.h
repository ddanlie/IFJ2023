
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "defs.h"


#define SYMTABLE_INIT_SIZE 10
#define HASHNUM 5381


typedef struct symtb_node_t
{
    symtb_token token;//we take token.id_name as a value when comparing
    bool deleted;
} symtb_node;

typedef struct symtable_t
{
    symtb_node* symtb_arr;
    int size;
    int capacity;
} symtable;

symtable symtb_init();

unsigned long hash(char *str);

void symtb_clear(symtable stb);//pass by value (there is nothing heavy in this structure)

//returs: -1 - error/no memory, 0 - inserted
ret_t symtb_insert(symtable *stb, char *key, symtb_node value);//pass a pointer because we need to change a size and capacity of a table

//returs: symtb_node. if nothing is found, deleted flag of symtb_node is true
symtb_node symtb_find(symtable stb, char *key);//pass by value (there is nothing heavy in this structure)

//returs: -1 - error/no element found, 0 - deleted
ret_t symtb_delete(symtable *stb, char *key);//pass a pointer because we need to change a size and capacity of a table

#endif //SYMTABLE_H
