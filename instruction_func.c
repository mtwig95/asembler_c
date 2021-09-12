#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "all_stages.h"

int check_opcode_group(char opcode[]) /*the function check what group instruction word belongs to*/
{
    int i;
    for (i = 0; i < 6; i++)
    {
        if (strcmp(opcode, GROUP1[i]) == 0) /*the instruction word belongs to group number 1*/
            return 1;
        else if (strcmp(opcode, GROUP2[i]) == 0) /*the instruction word belongs to group number 2*/
            return 2;
        else if (strcmp(opcode, GROUP3[i]) == 0) /*the instruction word belongs to group number 3*/
            return 3;
        else if (strcmp(opcode, GROUP4[i]) == 0) /*the instruction word belongs to group number 2*/
            return 4;
        else if (strcmp(opcode, GROUP5[i]) == 0) /*the instruction word belongs to group number 2*/
            return 5;
        else if (strcmp(opcode, GROUP6[i]) == 0) /*the instruction word belongs to group number 2*/
            return 6;
    }
    return 0;
}
int code_32bit_J(code my_code, int stage)
{
    unsigned int my_32bit = 0, temp;
    temp = my_code.my_ops.J.opcode;
    my_32bit |= (temp << 26);
    temp = my_code.my_ops.J.reg;
    my_32bit |= (temp << 25);
    if (my_code.my_ops.J.reg == 1)
    {
        temp = my_code.my_ops.J.address & 0x0000000000ff;
        my_32bit |= (temp);
    }
    if ((stage = 2) && (my_code.my_ops.J.reg == 0))
    {
        temp = my_code.my_ops.J.address & 0x00000000ffff;
        my_32bit |= (temp);
    }
    return my_32bit;
}
int code_32bit_R(code my_code)
{
    unsigned int my_32bit = 0;
    unsigned int temp;

    temp = my_code.my_ops.R.opcode;
    my_32bit |= (temp << 26); /*move 26 bits left, now our 6 bits of opcode are the most significant*/
    temp = my_code.my_ops.R.rs;
    my_32bit |= (temp << 21); /* why 21 ? we had 6 bits from the left, now we need to fit 5 bits, so we need to start from 5 bits less from where we are at now
                                so thats 26-5 = 21*/
    temp = my_code.my_ops.R.rt;
    my_32bit |= (temp << 16); /* why 16 ? same thing... we need 5 bits from the last 5 bits, therefore
                            21-5 = 16 */
    temp = my_code.my_ops.R.rd;
    my_32bit |= (temp << 11); /* why 11 ? same thing we used 5 bits now we need more 5 bits starting from last 5 bits
                            therefore 16-5 = 11 */
    temp = my_code.my_ops.R.funct;
    my_32bit |= (temp << 6); /* why 6 ? same thing we used 5 bites now we need more 5 its
                            therefore 11-5 = 6 */
    temp = my_code.my_ops.R.unused;
    my_32bit |= (temp);

    return my_32bit;
}
int code_32bit_I(code my_code)
{
    unsigned int my_32bit = 0;
    unsigned int temp;

    temp = my_code.my_ops.I.opcode;
    my_32bit |= (temp << 26); /*move 26 bits left, now our 6 bits of opcode are the most significant*/
    temp = my_code.my_ops.I.rs;
    my_32bit |= (temp << 21); /* why 21 ? we had 6 bits from the left, now we need to fit 5 bits, so we need to start from 5 bits less from where we are at now
                                so thats 26-5 = 21*/
    temp = my_code.my_ops.I.rt;
    my_32bit |= (temp << 16); /* why 16 ? same thing... we need 5 bits from the last 5 bits, therefore
                            21-5 = 16 */
    temp = (my_code.my_ops.I.immed & 0x0000ffff);
    temp |= my_32bit;
    return temp;
}

/*return code function*/
code code_opcode(char opcode[], code my_code)
{
    if (strcmp(opcode, "add") == 0)
    {
        my_code.my_ops.R.opcode = 0;
        my_code.my_ops.R.funct = 1;
    }
    else if (strcmp(opcode, "sub") == 0)
    {
        my_code.my_ops.R.opcode = 0;
        my_code.my_ops.R.funct = 2;
    }
    else if (strcmp(opcode, "and") == 0)
    {
        my_code.my_ops.R.opcode = 0;
        my_code.my_ops.R.funct = 3;
    }
    else if (strcmp(opcode, "or") == 0)
    {
        my_code.my_ops.R.opcode = 0;
        my_code.my_ops.R.funct = 4;
    }
    else if (strcmp(opcode, "nor") == 0)
    {
        my_code.my_ops.R.opcode = 0;
        my_code.my_ops.R.funct = 5;
    }
    else if (strcmp(opcode, "move") == 0)
    {
        my_code.my_ops.R.opcode = 1;
        my_code.my_ops.R.funct = 1;
    }
    else if (strcmp(opcode, "mvhi") == 0)
    {
        my_code.my_ops.R.opcode = 1;
        my_code.my_ops.R.funct = 2;
    }
    else if (strcmp(opcode, "mvlo") == 0)
    {
        my_code.my_ops.R.opcode = 1;
        my_code.my_ops.R.funct = 3;
    }
    else if (strcmp(opcode, "addi") == 0)
    {
        my_code.my_ops.I.opcode = 10;
    }
    else if (strcmp(opcode, "subi") == 0)
    {
        my_code.my_ops.I.opcode = 11;
    }
    else if (strcmp(opcode, "andi") == 0)
    {
        my_code.my_ops.I.opcode = 12;
    }
    else if (strcmp(opcode, "ori") == 0)
    {
        my_code.my_ops.I.opcode = 13;
    }
    else if (strcmp(opcode, "nori") == 0)
    {
        my_code.my_ops.I.opcode = 14;
    }
    /*I second*/
    else if (strcmp(opcode, "bne") == 0)
    {
        my_code.my_ops.I.opcode = 15;
    }
    else if (strcmp(opcode, "beq") == 0)
    {
        my_code.my_ops.I.opcode = 16;
    }
    else if (strcmp(opcode, "blt") == 0)
    {
        my_code.my_ops.I.opcode = 17;
    }
    else if (strcmp(opcode, "bgt") == 0)
    {
        my_code.my_ops.R.opcode = 18;
    }
    /*I third*/
    else if (strcmp(opcode, "lb") == 0)
    {
        my_code.my_ops.I.opcode = 19;
    }
    else if (strcmp(opcode, "sb") == 0)
    {
        my_code.my_ops.I.opcode = 20;
    }

    else if (strcmp(opcode, "lw") == 0)
    {
        my_code.my_ops.I.opcode = 21;
    }
    else if (strcmp(opcode, "sw") == 0)
    {
        my_code.my_ops.I.opcode = 22;
    }

    else if (strcmp(opcode, "lh") == 0)
    {
        my_code.my_ops.I.opcode = 23;
    }
    else if (strcmp(opcode, "sh") == 0)
    {
        my_code.my_ops.I.opcode = 24;
    }
    /*J ins*/
    else if (strcmp(opcode, "jmp") == 0)
    {
        my_code.my_ops.J.opcode = 30;
    }
    else if (strcmp(opcode, "la") == 0)
    {
        my_code.my_ops.J.opcode = 31;
    }

    else if (strcmp(opcode, "call") == 0)
    {
        my_code.my_ops.J.opcode = 32;
    }
    else if (strcmp(opcode, "stop") == 0)
    {
        my_code.my_ops.J.opcode = 63;
    }

    return my_code;
}
code three_reg(char args[], code my_code) /*codind: reg, reg, reg*/
{
    char temp1[REST_ROW], temp2[REST_ROW];
    get_rest_row(args, temp1);
    get_rest_row(temp1, temp2);

    my_code.my_ops.R.rs = get_register(args);
    my_code.my_ops.R.rt = get_register(temp1);
    my_code.my_ops.R.rd = get_register(temp2);
    return my_code;
}
code two_reg(char args[], code my_code) /*codind: reg, reg*/
{
    char temp1[REST_ROW];
    get_rest_row(args, temp1);
    my_code.my_ops.R.rs = get_register(args);
    my_code.my_ops.R.rd = get_register(temp1);
    my_code.my_ops.R.rt = 0;
    return my_code;
}
code i_first(char args[], code my_code) /*codind: reg, immed, reg*/
{
    char temp1[REST_ROW], temp2[REST_ROW];
    get_rest_row(args, temp1);
    get_rest_row(temp1, temp2);

    my_code.my_ops.I.rs = get_register(args);
    my_code.my_ops.I.immed = atoi(temp1);
    my_code.my_ops.I.rt = get_register(temp2);

    return my_code;
}
code i_second(char args[], code my_code) /*codind reg, immed, reg*/
{
    symbol *s = S_head;
    char temp1[REST_ROW], symbol[MAX_WORD];
    get_rest_row(args, temp1);
    get_rest_row(temp1, symbol);
    my_code.my_ops.I.rs = get_register(args);
    my_code.my_ops.I.rt = get_register(temp1);
    substring(symbol, symbol, 0, strlen(symbol) - 2);
    while (s)
    {
        if (strcmp(s->name, symbol) == 0)
        {
            if (strcmp(s->type, "external") == 0)
            {
                fprintf(stderr, "This symbol type is external - %s\n", symbol);
                error++;
                /*need to add error*/
            }
            my_code.my_ops.I.immed = (s->val) - IC;
        }
        s = s->next;
    }
    return my_code;
}
code i_second2(char args[], code my_code) /*codind reg, 0, reg*/
{
    char temp1[REST_ROW], symbol[MAX_WORD];

    get_rest_row(args, temp1);
    get_rest_row(temp1, symbol);
    my_code.my_ops.I.rs = get_register(args);
    my_code.my_ops.I.rt = get_register(temp1);
    my_code.my_ops.I.immed = 0;
    return my_code;
}
code j_ins(char args[], code my_code) /*codind reg, address*/
{
    if (args[0] == '$')
    {
        my_code.my_ops.J.reg = 1;
        my_code.my_ops.J.address = get_register(args);
    }
    else
    {
        my_code.my_ops.J.reg = 0;
        /*my_code.my_ops.I.address = (temp2);*/
    }
    return my_code;
}
code j_second(char args[], code my_code, char *fname) /*codind external for j instructions*/
{

    symbol *s = S_head;
    char temp1[REST_ROW], symbol[MAX_WORD];
    my_code.my_ops.J.reg = 0;
    get_word(args, symbol);
    get_word(args, temp1);
    substring(args, symbol, 0, strlen(symbol) - 1);

    while (s)
    {
        if ((strcmp(s->name, symbol) == 0) || (strcmp(s->name, temp1) == 0))
        {
            if (strcmp(s->type, "external") == 0)
            {
                stage2_parse_ext(symbol, IC, fname);
                my_code.my_ops.J.address = 0;
            }
            else
            {
                my_code.my_ops.J.address = s->val;
            }
        }
        s = s->next;
    }
    return my_code;
}
