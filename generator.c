//Implementace překladače imperativního jazyka IFJ23
//Danil Domrachev (xdomra00)
//Elena Ivanova (xivano08)

#include "generator.h"

char *lowlevel_var_format = "%s@&_%s$%d_&";
char *helpvar1 = "GF@$%optempvar%$";
char *helpvar2 = "GF@$%optempvar2%$";

void prepare()
{
    printf(".ifjcode23\n");
    printf("CREATEFRAME\n");
    defvar("GF@___$expr_res$___");
    defvar("GF@___$recursion$___");
    move("GF@___$recursion$___", "bool@false");
    defvar(helpvar1);
    defvar(helpvar2);
    defvar("GF@%retval");
    printf("PUSHS int@0\n");//stack param counter initialization
    generateInt2Double();
    generateDouble2Int();
    generatereadString();
    generatereadInt();
    generatereadDouble();
    generateLength();
    generateSubstring();
    generateord();
    generatechr();
    generateWrite();
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


char* convert_string(const char* inputStr) {
    
    size_t input_length = strlen(inputStr);
    char* resultStr = (char *)malloc(sizeof(char)*input_length * 5 );
    size_t i = 0, j = 0;
    for (; i < input_length; i++)
    {
        char current = inputStr[i];
        
        if (((int)current >= 0 && (int)current <= 32) || (int)current == 35 || (int)current == 92)
        {
            sprintf(resultStr + j, "\\%03d", (int)current);
            j+=4;
        }
        else
        {
            resultStr[j++] = current;
        }
    }
    resultStr[j] = '\0';
    return resultStr;
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
            char *tmp = convert_string( lt.str.value);
            sprintf(buf, "string@%s", tmp);
            free(tmp);
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


char *get_int_literal_name(int x)
{
    char buf[MAX_LITERAL_LENGTH];
    sprintf(buf, "int@%d", x);
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
            char *defskip_lbl = generate_label();
            printf("JUMPIFEQ %s GF@___$recursion$___ bool@true\n", defskip_lbl);
            defvar(codename);
            printf("LABEL %s\n", defskip_lbl);
            free(defskip_lbl);
            
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

void return_passed_vars()
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
            move(tfname, codename);
        }
        
    }
}

void funcdef_define_temp_params(symtb_token func)
{
    for(int i = 0; i < func.funcArgsSize; i++)
    {
        if(0 == strcmp(func.funcLocalArgnames[i], "_"))
            continue;
    
        char *vname = get_var_name(func.funcLocalArgnames[i]);
        defvar(vname);
        printf("POPS %s\n", vname);
        free(vname);
    }
}

void write_call_put_param(lex_token param, symtb_token called_fun)
{
    printf("POPS %s\n", helpvar1);
    printf("ADD %s %s int@1\n", helpvar1, helpvar1);
    if(param.lexeme_type == ID)
    {
        char *vname = get_var_name(param.str.value);
        printf("PUSHS %s\n", vname);
        free(vname);
    }
    else
    {
        char *litname = get_literal_name(param);
        printf("PUSHS %s\n", litname);
        free(litname);
    }
    printf("PUSHS %s\n", helpvar1);
}


void func_call_put_param(lex_token param,  symtb_token called_fun)
{
    if(0 == strcmp(called_fun.id_name, "write"))
    {
        write_call_put_param(param, called_fun);
        return;
    }
    
    
    if(param.lexeme_type == ID)
    {
        char *vname = get_var_name(param.str.value);
        printf("PUSHS %s\n", vname);
        free(vname);
    }
    else
    {
        char *litname = get_literal_name(param);
        printf("PUSHS %s\n", litname);
        free(litname);
    }
}


void generateInt2Double()
{
    printf("JUMP $Int2DoubleEND\n"
           "LABEL Int2Double\n"
           "PUSHFRAME\n"
           "CREATEFRAME\n"
           "DEFVAR TF@%%1\n"
           "POPS TF@%%1\n"
           "INT2FLOAT GF@%%retval TF@%%1\n"
           "POPFRAME\n"
           "RETURN\n"
           "LABEL $Int2DoubleEND\n");
}

void generateDouble2Int()
{
    printf("JUMP $Double2IntEND\n"
           "LABEL Double2Int\n"
           "PUSHFRAME\n"
           "CREATEFRAME\n"
           "DEFVAR TF@%%1\n"
           "POPS TF@%%1\n"
           "FLOAT2INT GF@%%retval TF@%%1\n"
           "POPFRAME\n"
           "RETURN\n"
           "LABEL $Double2IntEND\n");
}

void generatereadString()
{
    printf("JUMP $readStringEND\n"
           "LABEL readString\n"
           "PUSHFRAME\n"
           "CREATEFRAME\n"
           "READ GF@%%retval string\n"
           "POPFRAME\n"
           "RETURN\n"
           "LABEL $readStringEND\n");
}

void generatereadInt()
{
    printf("JUMP $readIntEND\n"
           "LABEL readInt\n"
           "PUSHFRAME\n"
           "CREATEFRAME\n"
           "READ GF@%%retval int\n"
           "DEFVAR TF@readtype\n"
           "TYPE TF@readtype GF@%%retval\n"
           "JUMPIFNEQ $%%notinttype TF@readtype string@int\n"
           "JUMP $%%readIntRet\n"
           "LABEL $%%notinttype\n"
           "MOVE GF@%%retval nil@nil\n"
           "LABEL $%%readIntRet\n"
           "POPFRAME\n"
           "RETURN\n"
           "LABEL $readIntEND\n");
}

void generatereadDouble()
{
    printf("JUMP $readDoubleEND\n"
           "LABEL readDouble\n"
           "PUSHFRAME\n"
           "CREATEFRAME\n"
           "READ GF@%%retval float\n"
           "DEFVAR TF@readtype\n"
           "TYPE TF@readtype GF@%%retval\n"
           "JUMPIFNEQ $%%notDtype TF@readtype string@float\n"
           "JUMP $%%readDRet\n"
           "LABEL $%%notDtype\n"
           "MOVE GF@%%retval nil@nil\n"
           "LABEL $%%readDRet\n"
           "POPFRAME\n"
           "RETURN\n"
           "LABEL $readDoubleEND\n");
}

void generateLength()
{
    printf("JUMP $lengthEND\n"
           "LABEL length\n"
           "PUSHFRAME\n"
           "CREATEFRAME\n"
           "DEFVAR TF@%%1\n"
           "POPS TF@%%1\n"
           "STRLEN GF@%%retval TF@%%1\n"
           "POPFRAME\n"
           "RETURN\n"
           "LABEL $lengthEND\n");
}

void generateSubstring()
{
    printf("JUMP $substringEND\n"
           "LABEL substring\n"
           "PUSHFRAME\n"
           "CREATEFRAME\n"
           "DEFVAR TF@counter\n"
           "DEFVAR TF@end\n"
           "DEFVAR TF@startstr\n"
           "DEFVAR TF@finstr\n"
           "DEFVAR TF@sym\n"
           "DEFVAR TF@len\n"
           "DEFVAR TF@errfound\n"
           "POPS TF@end\n"
           "POPS TF@counter\n"
           "POPS TF@startstr\n"
           "MOVE TF@finstr string@\n"
           "STRLEN TF@len TF@startstr\n"
           "LT TF@errfound TF@counter int@0\n"
           "JUMPIFEQ $%%err TF@errfound bool@true\n"
           "LT TF@errfound TF@end int@0\n"
           "JUMPIFEQ $%%err TF@errfound bool@true\n"
           "GT TF@errfound TF@counter TF@end\n"
           "JUMPIFEQ $%%err TF@errfound bool@true\n"
           "EQ TF@errfound TF@counter TF@end\n"
           "JUMPIFEQ $%%err TF@errfound bool@true\n"
           "GT TF@errfound TF@counter TF@len\n"
           "JUMPIFEQ $%%err TF@errfound bool@true\n"
           "EQ TF@errfound TF@counter TF@len\n"
           "JUMPIFEQ $%%err TF@errfound bool@true\n"
           "GT TF@errfound TF@end TF@len\n"
           "JUMPIFEQ $%%err TF@errfound bool@true\n"
           "LABEL $%%cycle\n"
           "GETCHAR TF@sym TF@startstr TF@counter\n"
           "CONCAT TF@finstr TF@finstr TF@sym\n"
           "ADD TF@counter TF@counter int@1\n"
           "JUMPIFEQ $%%cycleend TF@counter TF@end\n"
           "JUMP $%%cycle\n"
           "LABEL $%%err\n"
           "MOVE GF@%%retval nil@nil\n"
           "JUMP $%%retstr\n"
           "LABEL $%%cycleend\n"
           "MOVE GF@%%retval TF@finstr\n"
           "LABEL $%%retstr\n"
           "POPFRAME\n"
           "RETURN\n"
           "LABEL $substringEND\n");
}

void generateord()
{
    printf("JUMP $ordEND\n"
           "LABEL ord\n"
           "PUSHFRAME\n"
           "CREATEFRAME\n"
           "DEFVAR TF@str\n"
           "POPS TF@str\n"
           "JUMPIFEQ $%%nilstr TF@str nil@nil\n"
           "STRI2INT GF@%%retval TF@str int@0\n"
           "JUMP $%%ordreturn\n"
           "LABEL $%%nilstr\n"
           "MOVE GF@%%retval int@0\n"
           "LABEL $%%ordreturn\n"
           "POPFRAME\n"
           "RETURN\n"
           "LABEL $ordEND\n");
}

void generatechr()
{
    printf("JUMP $chrEND\n"
           "LABEL chr\n"
           "PUSHFRAME\n"
           "CREATEFRAME\n"
           "DEFVAR TF@sym\n"
           "POPS TF@sym\n"
           "DEFVAR TF@errfound\n"
           "LT TF@errfound TF@sym int@0\n"
           "JUMPIFEQ $%%chrerr TF@errfound bool@true\n"
           "GT TF@errfound TF@sym int@255\n"
           "JUMPIFEQ $%%echrrr TF@errfound bool@true\n"
           "JUMP $%%retsym\n"
           "LABEL $%%chrerr \n"
           "EXIT int@58\n"
           "LABEL $%%retsym\n"
           "INT2CHAR GF@%%retval TF@sym\n"
           "POPFRAME\n"
           "RETURN\n"
           "LABEL $chrEND\n");
}

void generateWrite()
{
        printf("DEFVAR GF@%%cntr$out\n"
               "DEFVAR GF@%%cntr$fr\n"
               "JUMP $writeEnd \n"
               "LABEL write \n"
               "PUSHFRAME\n"
               "CREATEFRAME\n"
               "POPS GF@%%cntr$out\n"
               "MOVE GF@%%cntr$fr GF@%%cntr$out\n"
               "JUMPIFNEQ $%%writepushcycle GF@%%cntr$fr int@0\n"
               "LABEL $%%writepushcycle\n"
               "PUSHFRAME\n"
               "CREATEFRAME\n"
               "DEFVAR TF@%%out\n"
               "POPS TF@%%out\n"
               "SUB GF@%%cntr$fr GF@%%cntr$fr int@1\n"
               "JUMPIFNEQ $%%writepushcycle GF@%%cntr$fr int@0\n"
               "JUMPIFNEQ $%%writeoutcycle GF@%%cntr$out int@0\n"
               "LABEL $%%writeoutcycle\n"
               "WRITE TF@%%out\n"
               "POPFRAME\n"
               "SUB GF@%%cntr$out GF@%%cntr$out int@1\n"
               "JUMPIFNEQ $%%writeoutcycle GF@%%cntr$out int@0\n"
               "PUSHS int@0\n"
               "POPFRAME\n"
               "RETURN\n"
               "LABEL $writeEnd\n");
}