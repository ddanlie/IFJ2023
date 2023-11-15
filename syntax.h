#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>
#include "defs.h"
#include "stack.h"
#include "symtable.h"


void searchCalledFunction(lex_token function);
bool getFromGlobalTable(char *id, symtb_token *found);
bool currLexTokenIs(lexeme lexeme_value);
bool prevLexTokenIs(lexeme lexeme_value);
void read_move();
void err_exit(ret_t ret);

ret_t analyze_syntax(FILE *input_file);







#endif //SYNTAX_H
