#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "syntax.h"

//checks if variable ID is redefined (including function ID)
bool varRedefinition(char *var);
bool isVarDefined(char *var);
bool isFuncDefined(char *func);
bool setLiteralType(literal_type *type, lex_token lxtoken, bool checkInit);
//this function crucially depends on the `expr_rule_table` and rule order in it
bool ruleTypeCheck(int rule_index, Stack *expr_stack);
//compares type and expression. if there is no type - tries to derive the type and assigns it to var
bool vardefCompareTypeExpr(symtb_token *var, literal_type expr_type);
//add to global or temp frame
void addVarToFrame(symtb_token current_symtb_token);


#endif //SEMANTIC_H
