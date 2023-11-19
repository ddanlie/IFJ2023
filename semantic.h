#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "syntax.h"

bool varRedefinition(char *var);
bool funcRedefinition(char *func);
bool isVarDefined(char *var);
bool isFuncDefined(char *func);
bool compareFunctionsSignature(symtb_token f1, symtb_token f2);
bool compareUndefinedFunction(symtb_token f1, symtb_token f2);
//returned false -> found undefined function and signature is not compatible
bool resolveUndefinedFunctions(Stack *undefs, symtb_token defined);
bool compareIDtoFuncReturn(symtb_token var, symtb_token f);
bool setLiteralType(literal_type *type, lex_token lxtoken, bool checkInit);
//this function crucially depends on the `expr_rule_table` and rule order in it
bool ruleTypeCheck(int rule_index, Stack *expr_stack);
//compares type and expression. if there is no type - tries to derive the type and assigns it to var
bool vardefCompareTypeExpr(symtb_token *var, literal_type expr_type);
void addFuncVarsToTable(symtb_token func, symtable *tb);
//add to global or temp frame
void addVarToFrame(symtb_token current_symtb_token);


#endif //SEMANTIC_H
