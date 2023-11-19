#include "generator.h"


char *helpvar1 = "GF@$%optempvar%$";
char *helpvar2 = "GF@$%optempvar2%$";

void prepare()
{
    printf(".ifjcode23\n");
    printf("CREATEFRAME\n");
    printf("DEFVAR %s\n", helpvar1);
    printf("DEFVAR %s\n", helpvar2);
}

void defvar(char *name)
{
    printf("DEFVAR %s\n", name);
}


void move(char *var, char *symb)
{
    printf("MOVE %s %s\n", var, symb);
}


void addr3op(char *op, char *res, char *a1, char *a2)
{
    if(0 == strcmp(op, "CONCAT"))
    {
        printf("CONCAT %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "PLUS"))
    {
        printf("PLUS %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "MINUS"))
    {
        printf("MINUS %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "MUL"))
    {
        printf("MUL %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "DIV"))
    {
        printf("DIV %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "EQ"))
    {
        printf("EQ %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "NEQ"))
    {
        printf("EQ %s %s %s\n", a1, a1, a2);
        printf("NOT %s %s\n", res, a1);
    }
    else if(0 == strcmp(op, "LT"))
    {
        printf("LT %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "GT"))
    {
        printf("GT %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "LTE"))
    {
        printf("LE %s %s %s\n", helpvar1, a1, a2);
        printf("EQ %s %s %s\n", helpvar2, a1, a2);
        printf("OR %s %s %s\n", res, helpvar1, helpvar2);
    }
    else if(0 == strcmp(op, "GTE"))
    {
        printf("GE %s %s %s\n", helpvar1, a1, a2);
        printf("EQ %s %s %s\n", helpvar2, a1, a2);
        printf("OR %s %s %s\n", res, helpvar1, helpvar2);
    }
    else if(0 == strcmp(op, "QQ"))
    {
        char *lbl1 = generate_label();
        char *lbl2 = generate_label();
        char *lbl3 = generate_label();
        
        printf("JUMPIFEQ %s %s %s\n", lbl1, a1, "nil@nil");
        printf("JUMP %s\n", lbl2);
        printf("LABEL %s\n", lbl1);
        printf("MOVE %s %s\n", res, a2);
        printf("JUMP %s\n", lbl3);
        printf("LABEL %s\n", lbl2);
        printf("MOVE %s %s\n", res, a1);
        printf("LABEL %s", lbl3);
        
        free(lbl1);
        free(lbl2);
        free(lbl3);
    }
    
}

char* generate_expr_var_name()
{
    static int counter = 0;
    char buf[MAX_VAR_NAME_LENGTH];
    sprintf(buf, "TF@$generated_on_stack_%d$", counter);
    printf("DEFVAR %s\n", buf);
    char *res = malloc(sizeof(char)* (strlen(buf)+1));
    strcpy(res, buf);
    counter++;
    return res;
}

char *get_var_name(char *var)
{
    symtb_token *found;
    symtable *table;
    getFromEverywhere(var, &found, &table);
    
    char buf[MAX_VAR_NAME_LENGTH];
    char *frame;
    int level_name_addition = -1;
    if(table->local_level == 0)
    {
        frame = "GF";
    }
    else if(table->local_level == current_local_level)
    {
        frame = "TF";
    }
    else
    {
        frame = "GF";
        level_name_addition = table->local_level;
    }
    
    if(level_name_addition != -1)
    {
        sprintf(buf, "%s@%s$%d", frame, var, level_name_addition);
    }
    else
    {
        sprintf(buf, "%s@%s", frame, var);
    }
    
    char *res = malloc(sizeof(char)* (strlen(buf)+1));
    strcpy(res, buf);
    
    return res;
}


char *get_literal_name(lex_token lt)
{
    char buf[MAX_LITERAL_LENGTH];
    switch(lt.lexeme_type)
    {
        case NIL:
        {
            sprintf(buf, "nil@nil");
            break;
        }
        case STRING_LIT:
        {
            sprintf(buf, "string@%a", lt.str.value);
            break;
        }
        case INT_LIT:
        {
            sprintf(buf, "int@%d", lt.int_value);
            break;
        }
        case DOUBLE_LIT:
        {
            sprintf(buf, "float@%a", lt.double_value);
            break;
        }
        default:
            break;
    }
    char *res = malloc(sizeof(char)* (strlen(buf)+1));
    strcpy(res, buf);
    return res;
}

char* generate_label()
{
    static int counter = 0;
    char buf[MAX_VAR_NAME_LENGTH];
    sprintf(buf, "$help_label_%d$", counter);
    char *res = malloc(sizeof(char)* (strlen(buf)+1));
    strcpy(res, buf);
    counter++;
    return res;
}