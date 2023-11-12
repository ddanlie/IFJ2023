//
// Created by Daniel on 21.10.2023.
//

#ifndef SYNTAX_H
#define SYNTAX_H

#include "defs.h"
#include "lexical.h"
#include "symtable.h"
#include "stack.h"

FILE *readfile;

lex_token current_lex_token;
lex_token previous_lex_token;
symtb_token current_symtb_token;

symtable global_table;
Stack *local_tables;

ret_t analysis_error;

bool getFromGlobalTable(char *id, symtb_token *found);
bool currLexTokenIs(lexeme lexeme_value);
bool prevLexTokenIs(lexeme lexeme_value);
ret_t read_move();
void err_exit(ret_t ret);

ret_t analyze_syntax(FILE *input_file);







#endif //SYNTAX_H
