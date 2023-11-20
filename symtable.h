
#ifndef SYMTABLE_H
#define SYMTABLE_H

#define FUNC_ARGS_INIT_CAPACITY 10
#define SYMTABLE_INIT_SIZE 10
#define HASHNUM 5381

#include <stdbool.h>
#include "lexical.h"

typedef enum id_type_t
{
    UNDEF_ID, VARIABLE, CONSTANT, FUNCTION
} id_type;

typedef enum literal_type_t
{
    UNDEF_TYPE, INT_TYPE, STRING_TYPE, DOUBLE_TYPE,
    NINT_TYPE, NSTRING_TYPE, NDOUBLE_TYPE, //nillable int/string/double
    NINT_NIL_TYPE, NSTRING_NIL_TYPE, NDOUBLE_NIL_TYPE,
    NIL_TYPE,
    VOID_TYPE,
    BOOL_TYPE
} literal_type;


//symbol table token to construct from lexeme tokens
typedef struct symtb_token_t
{
    char *id_name;//id is always a pointer to a heap (malloc), so it can be freed
    id_type type;
    literal_type lit_type;//for variable/constant INT/NINT,STRING/NSTRING and DOUBLE/NDOUBLE are legitimate //for function that means what it returns
    literal_type *funcArgTypes;//array order corresponds to funcArgnames
    char **funcArgnames;//array order corresponds to funcArgs //array and its elements are always pointers to a heap so they can be freed
    char **funcLocalArgnames;//array order corresponds to funcArgs //array and its elements are always pointers to a heap so they can be freed
    int funcArgsSize;
    int funcArgsCapacity;
    bool initialized;//VARIABLE: if variable was initialized with some value or not
    //FUNCTION: if function call was found this variable sets to false, otherwise if complete function definition was found - true
} symtb_token;


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
    int local_level;
} symtable;


symtable symtb_init(int init_size);

unsigned long hash(char *str, int capacity);

void symtb_clear(symtable stb);//pass by value (there is nothing heavy in this structure)

//returs: -1 - error/no memory, 0 - inserted
//value with the same key will be rewritten
ret_t symtb_insert(symtable *stb, char *key, symtb_token value);//pass a pointer because we need to change a size and capacity of a table

//returs: symtb_node. if nothing is found, deleted flag of symtb_node is true and token is undefined
//idnex_found parameter is optional and can be NULL. If nothing found this variable is undefined
//DO NOT free returned symtb_node
symtb_node* symtb_find(symtable stb, char *key, int *index_found);//pass by value (there is nothing heavy in this structure)

//returs: -1 - error/no element found, 0 - deleted
ret_t symtb_delete(symtable *stb, char *key);//pass a pointer because we need to change a size and capacity of a table

void symtb_print(symtable stb);




void initSymtbToken(symtb_token *token);
void clearSymtbToken(symtb_token *token);
void copySymtbToken(symtb_token *dst, symtb_token src);


//set type from keyword
void symtbTokenSetTypefromKw(symtb_token *dst, lex_token src);

void symtbTokenCopyName(symtb_token *dst, lex_token src);
void symtbTokenCopyName2(symtb_token *dst, const char *name);
void checkArgsSetSize(symtb_token *dst);
//next 3 functions watch each other. you cannot add next argument until all three of them are used. see 'checkArgsSetSize' function
void symtbTokenAddArgName(symtb_token *dst,  lex_token src);
void symtbTokenAddArgName2(symtb_token *dst,  char *argname);
void symtbTokenAddArgType2(symtb_token *dst, symtb_token src);
void symtbTokenAddArgType3(symtb_token *dst, literal_type t);
void symtbTokenAddArgType(symtb_token *dst, lex_token src);
void symtbTokenAddLocalArgName(symtb_token *dst, lex_token src);
void symtbTokenAddLocalArgName2(symtb_token *dst, char *argname);
#endif //SYMTABLE_H
