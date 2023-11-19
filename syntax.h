#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>
#include "stack.h"
#include "symtable.h"
#include "semantic.h"
#include "generator.h"

extern ret_t analysis_error;
extern lex_token current_lex_token;
extern lex_token previous_lex_token;
extern symtb_token current_symtb_token;
extern symtb_token current_called_function;
extern symtb_token current_defined_function;
extern Stack *undefined_functions;
extern symtable global_table;
extern Stack *local_tables;
extern symtable temporary_table;
extern int current_local_level;
extern lexeme current_expr_lexeme;
extern Stack *add_later_stack;
extern char *generator_temp_res_name;

extern literal_type current_expr_type;

#define gr '>'
#define le '<'
#define eq '='
extern char precedence_table[LEXEMES_COUNT][LEXEMES_COUNT];


//'expr_lex' is modified 'lex_token.lexeme_type' so it can be recognised by precedence analyser
typedef struct expr_lexeme_t
{
    lex_token lxtoken;//is used during rule transformation ID1 -> ID
    lexeme exp_lex;
    char specChar;//can acquire '<' '>' or '=' chars
    literal_type type;//is used during operator rule transformation
    char *generator_tmp_name;//pointer has to be freed
} expr_lexeme;


void initPrecedenceTable();
void searchCalledFunction(lex_token function);
bool getFromGlobalTable(char *id, symtb_token **found);
bool getFromLocalTables(char *id, symtb_token **token_found, symtable **table_found);
bool getFromEverywhere(char *id, symtb_token **found, symtable **table_found);
bool currLexTokenIs(lexeme lexeme_value);
bool prevLexTokenIs(lexeme lexeme_value);
void read_move();
void err_exit(ret_t ret);

ret_t analyze_syntax(FILE *input_file);







#endif //SYNTAX_H
