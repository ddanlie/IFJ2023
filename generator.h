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
//returned pointer has to be freed
char* generate_expr_var_name();
char* generate_label();
void pass_vars_to_global();
void jump(char *lbl);
void funcdef_define_temp_params(symtb_token func);
void func_call_put_param(lex_token param, int param_number);

#endif GENERATOR_H
