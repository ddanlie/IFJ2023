#ifndef GENERATOR_H
#define GENERATOR_H

#include "defs.h"
#include "syntax.h"

#define MAX_VAR_NAME_LENGTH 100
#define MAX_LITERAL_LENGTH 10000

void prepare();
void defvar(char *name);
void addr3op(char *op, char *res, char *a1, char *a2);
void move(char *var, char *symb);
void concat(char *res, char *s1, char *s2);
char *get_var_name(char *var);
char *get_literal_name(lex_token lt);
char *get_int_literal_name(int x);
//returned pointer has to be freed
char* generate_expr_var_name();
char* generate_label();
void pass_vars_to_global();
void jump(char *lbl);
void funcdef_define_temp_params(symtb_token func);
void func_call_put_param(lex_token param,  symtb_token called_fun);

void pass_vars_to_global();
void return_passed_vars();
void generateInt2Double();
void generateDouble2Int();
void generatereadString();
void generatereadInt();
void generatereadDouble();
void generateLength();
void generateSubstring();
void generateord();
void generatechr();
void generateWrite();
void generateBuiltinFuncs();

#endif GENERATOR_H
