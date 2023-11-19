#include "generator.h"

char *lowlevel_var_format = "%s@&_%s$%d_&";
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

void jump(char *lbl)
{
    printf("JUMP %s\n", lbl);
}


void addr3op(char *op, char *res, char *a1, char *a2)
{
    if(0 == strcmp(op, "CONCAT"))
    {
        printf("CONCAT %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "AND"))
    {
        printf("AND %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "OR"))
    {
        printf("OR %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "NOT"))
    {
        printf("NOT %s %s\n", res, a1);
    }
    else if(0 == strcmp(op, "PLUS"))
    {
        printf("ADD %s %s %s\n", res, a1, a2);
    }
    else if(0 == strcmp(op, "MINUS"))
    {
        printf("SUB %s %s %s\n", res, a1, a2);
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
        printf("LT %s %s %s\n", helpvar1, a1, a2);
        printf("EQ %s %s %s\n", helpvar2, a1, a2);
        printf("OR %s %s %s\n", res, helpvar1, helpvar2);
    }
    else if(0 == strcmp(op, "GTE"))
    {
        printf("GT %s %s %s\n", helpvar1, a1, a2);
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
        printf("LABEL %s\n", lbl3);
        
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
    
    if(level_name_addition == -1)
    {
        sprintf(buf, "%s@%s", frame, var);
    }
    else//variable from lower levels
    {
        sprintf(buf, lowlevel_var_format, frame, var, level_name_addition);
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

void convert_string(const char *in_str, char *out_str) {
    int i, j = 0;
    for (i = 0; in_str[i] != '\0'; i++)
    {
        if (in_str[i] == ' ')
        {
            sprintf(&out_str[j], "\032");
            j += 4;
        }
        else if (in_str[i] == '\\')
        {
            sprintf(&out_str[j], "\\092");
            j += 4;
        }
        else if (in_str[i] == '\n')
        {
            sprintf(&out_str[j], "\\010");
            j += 4;
        }
        else if (in_str[i] == '#')
        {
            sprintf(&out_str[j], "\035");
            j += 4;
        }
        else {
            out_str[j] = in_str[i];
            j++;
    }
}
out_str[j] = '\0';
}

//see get_var_name()
void pass_vars_to_global()
{
    int local_lvl = temporary_table.local_level;
    for(int i = 0; i < temporary_table.capacity; i++)
    {
        if(temporary_table.symtb_arr[i].deleted)
            continue;
        
        char codename[MAX_VAR_NAME_LENGTH];
        sprintf(codename, lowlevel_var_format, "GF", temporary_table.symtb_arr[i].token.id_name, local_lvl);
        //printf("-----------codename = %s---------\n", codename);
        char *tfname = get_var_name(temporary_table.symtb_arr[i].token.id_name);
        //printf("-----------tfname = %s---------\n", tfname);
        symtb_token *found;
        if(getFromGlobalTable(codename, &found))
        {
            move(codename, tfname);
        }
        else
        {
            defvar(codename);
            move(codename, tfname);
            
            symtb_token defined;
            initSymtbToken(&defined);
            copySymtbToken(&defined, temporary_table.symtb_arr[i].token);
            symtbTokenCopyName2(&defined, codename);
            symtb_insert(&global_table, codename, defined);
            clearSymtbToken(&defined);
        }
        free(tfname);

    }
}