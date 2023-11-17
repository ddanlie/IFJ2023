#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>
#include "stack.h"
#include "symtable.h"
#include "semantic.h"

extern ret_t analysis_error;
extern lex_token current_lex_token;
extern lex_token previous_lex_token;
extern symtb_token current_symtb_token;
extern symtb_token current_called_function;
extern Stack *undefined_functions;
extern symtable global_table;
extern Stack *local_tables;
extern symtable current_local_table;
extern int current_local_level;
extern lexeme current_expr_lexeme;



void initPrecedenceTable();
void searchCalledFunction(lex_token function);
bool getFromGlobalTable(char *id, symtb_token *found);
bool getFromLocalTables(char *id, symtb_token *token_found, symtable **table_found);
bool currLexTokenIs(lexeme lexeme_value);
bool prevLexTokenIs(lexeme lexeme_value);
void read_move();
void err_exit(ret_t ret);

ret_t analyze_syntax(FILE *input_file);







#endif //SYNTAX_H
