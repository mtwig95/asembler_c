#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "all_stages.h"

/*The functions that belong to Stage 1 - which aims to perform initial parseing of rows, and update the symbol table, data table ,IC and DC*/

void stage1_parse_row(char myrow[], char label[], char is_label[]) /*the function parse the row for first time, with stage 1 algoritm*/
{
	int i, len, lenword;
	char word[MAX_WORD], rest_row[REST_ROW], nlabel[MAX_WORD], row[REST_ROW], temp[REST_ROW], is_parse[6] = FALSE;
	char check[MAX_ROW];
	strcpy(check, myrow);
	substring(myrow, check, 0, strlen(myrow) - 2);
	strcpy(row, myrow);
	len = strlen(row);

	if (len == 0) /*check if empty row*/
		return;

	strcpy(temp, row);
	get_word(temp, word); /*the first word in the row*/
	lenword = strlen(word);

	for (i = 0; i < 6; i++) /*check if instruction statement*/
	{
		if (check_opcode_group(word) != 0)
		{
			strcpy(is_parse, TRUE);
			stage1_parse_instruction(row, label, is_label);
			IC += 4; /*instructions counter*/
			return;
		}
		if (check_opcode_group(check) != 0)
		{
			stage1_parse_instruction(check, label, is_label);
			return;
		}
	}

	if (word[0] == ';') /*check if comment statement*/
	{
		strcpy(is_parse, TRUE);
		return;
	}

	else if (strcmp(is_label, FALSE) == 0) /*check if label, With syntax analysis*/
	{
		if (word[lenword - 1] == ':')
		{
			strcpy(is_parse, TRUE);
			strcpy(rest_row, row);
			get_rest_row(row, rest_row);
			strcpy(temp, row);
			get_word(temp, nlabel);

			lenword = strlen(nlabel);
			substring(nlabel, nlabel, 0, lenword - 1);
			strcpy(is_label, TRUE);
			stage1_parse_row(rest_row, nlabel, is_label); /*check the rest of the row, When the label is updated*/
		}
	}
	if (word[0] == '.') /*check if directive statement*/
	{
		stage1_parse_directive(temp, word, label, is_label);
		strcpy(is_parse, TRUE);
	}
	if (strcpy(is_parse, FALSE) == 0) /*the row is invalid, no good parser, return an error*/
	{
		fprintf(stderr, "Line is invalid");
		error++;
	}
}

void stage1_parse_directive(char myrow[], char word[], char label[], char is_label[]) /*the function parse directive statement*/
{
	char row[REST_ROW];
	strcpy(row, myrow);

	if (strcmp(word, ".entry") == 0) /*entry statement - will check in stage 2*/
	{
		return;
	}
	else if (strcmp(word, ".extern") == 0) /*external statement*/
	{
		get_rest_row(row, row);
		get_word(row, word);
		insert_to_symbol_table(word, 0, "external");
	}
	else if (strcmp(word, ".asciz") == 0) /*asciz statement*/
	{
		get_rest_row(row, row);
		if (strcmp(is_label, TRUE) == 0)
		{
			stage1_insert_to_symbol_table(row, label, is_label, "data", 0);
		}
		stage1_parse_asciz(row);
	}

	else if ((strcmp(word, ".db") == 0)) /*db statement*/
	{
		get_rest_row(row, row);
		if (strcmp(is_label, TRUE) == 0)
		{
			stage1_insert_to_symbol_table(row, label, is_label, "data", 0);
		}
		stage1_parse_data(row, CODE_DB, 0);
	}
	else if ((strcmp(word, ".dh") == 0)) /*dh statement*/
	{
		get_rest_row(row, row);
		if (strcmp(is_label, TRUE) == 0)
		{
			stage1_insert_to_symbol_table(row, label, is_label, "data", 0);
		}
		stage1_parse_data(row, CODE_DH, 1);
	}
	else if ((strcmp(word, ".dw") == 0)) /*dw statement*/
	{
		get_rest_row(row, row);
		if (strcmp(is_label, TRUE) == 0)
		{
			stage1_insert_to_symbol_table(row, label, is_label, "data", 0);
		}
		stage1_parse_data(row, CODE_DW, 3);
	}
	else /*no legal directive*/
	{
		fprintf(stderr, "There is no legal directive.\n");
		error++;
	}
}

void stage1_insert_to_symbol_table(char row[], char label[], char is_label[], char type[], int macro_val) /*the function use the insret_to_symbol_table function, and insert all the symbols as per the requirements*/
{
	int val;
	char name[MAX_WORD];
	symbol *s = S_head;

	if (strcmp(type, "external") == 0)
		val = 0; /*an external val*/
	if (strcmp(is_label, TRUE) == 0)
		strcpy(name, label);
	if (strcmp(type, "code") == 0)
		val = IC;
	else /*(strcmp(type, "data") == 0)*/
	{
		val = DC;
	}
	if (!s) /*check the current symbol table and enter the new symbol*/
	{
		insert_to_symbol_table(name, val, type);
	}
	else
	{
		while (s) /*check were to enterd and if the symbol is not exists*/
		{
			if (strcmp(s->name, name) == 0)
			{
				fprintf(stderr, "This symbol is already exists in the symbols table - %s\n", name);
				error++;
				return;
			}
			s = s->next;
		}
		insert_to_symbol_table(name, val, type);
	}
}

void stage1_parse_asciz(char row[]) /*the function parse a asciz statement*/
{
	int i, len;
	int val[CODE_DB];
	char asciz[MAX_WORD];
	char c;
	unsigned int zero = 0x00;
	strcpy(asciz, row);
	len = strlen(asciz);

	for (i = 0; i < len - 4; i++)
	{
		c = asciz[i + 1]; /*Analysis of each letter*/
		val[i] = (int)c;
		insert_to_data_table(DC, val[i], CODE_DB); /*with asciz value*/
		DC++;									   /*number of bytes to the data counter*/
	}
	insert_to_data_table(DC, zero, CODE_DB); /* the last character in the string '\0' */
	DC++;									 /*number of bytes to the data counter*/
}

void stage1_parse_data(char row[], int size, int num_byte) /*the function parse a data statement*/
{
	int val;
	char is_symbol[] = FALSE;
	symbol *s = S_head;
	char name_s[MAX_WORD], temp[REST_ROW];
	strcpy(temp, row);

	while (!(strcmp(temp, "") == 0))
	{
		get_word(temp, name_s);
		if (find_end_row(temp) == findspace(temp))
			strcpy(temp, "");
		else
			get_rest_row(temp, temp);
		s = S_head;
		while (s)
		{
			if (strcmp(s->name, name_s) == 0)
			{
				val = s->val;
				strcpy(is_symbol, TRUE);
			}
			s = s->next;
		}
		if (strcmp(is_symbol, FALSE) == 0)
		{
			val = atof(name_s);
		}

		insert_to_data_table(DC, val, size);
		DC = DC + size / 8; /*number of bytes to the data counter*/
	}
}

void stage1_parse_instruction(char row[], char label[], char is_label[]) /*the function parse instruction statement*/
{
	unsigned int cur_code;
	int opcode_group;
	char opcode[MAX_WORD], args[REST_ROW];
	code my_code;
	strcpy(args, row);
	if (strcmp(is_label, TRUE) == 0) /*if this is a labael we will insert to symbol table*/
	{
		stage1_insert_to_symbol_table(row, label, is_label, "code", 0);
	}
	get_word(args, opcode);
	get_rest_row(args, args);
	opcode_group = check_opcode_group(opcode);
	if (opcode_group == 1) /*R - Arithmetic and logical instructions*/
	{
		my_code.type = R;
		my_code.my_ops.R.unused = 0;
		my_code = code_opcode(opcode, my_code);
		my_code = three_reg(args, my_code);
		cur_code = code_32bit_R(my_code);
		insert_to_code_table(IC, cur_code);
	}
	else if (opcode_group == 2) /*R - Copy instructions*/
	{
		my_code.type = R;
		my_code.my_ops.R.unused = 0;
		my_code = code_opcode(opcode, my_code);
		my_code = two_reg(args, my_code);
		cur_code = code_32bit_R(my_code);
		insert_to_code_table(IC, cur_code);
	}
	else if (opcode_group == 3) /*I - Arithmetic and logical instructions*/
	{
		my_code.type = I;
		my_code = code_opcode(opcode, my_code);
		my_code = i_first(args, my_code);
		cur_code = code_32bit_I(my_code);
		insert_to_code_table(IC, cur_code);
	}
	else if (opcode_group == 4) /*I - Conditional branching instructions*/
	{
		my_code.type = I;
		my_code = code_opcode(opcode, my_code);
		my_code = i_second2(args, my_code);
		cur_code = code_32bit_I(my_code);
		insert_to_code_table(IC, cur_code);
	}
	else if (opcode_group == 5) /*I - Instructions for loading and keeping in memory*/
	{
		my_code.type = I;
		my_code = code_opcode(opcode, my_code);
		my_code = i_first(args, my_code);
		cur_code = code_32bit_I(my_code);
		insert_to_code_table(IC, cur_code);
	}
	else if (opcode_group == 6) /*J Instructions*/
	{
		my_code.type = J;
		my_code = code_opcode(opcode, my_code);
		my_code = j_ins(args, my_code);
		if (my_code.my_ops.J.opcode == 63) /*the instrucsion stop get the address 0*/
			my_code.my_ops.J.address = 0;
		cur_code = code_32bit_J(my_code, 1);
		insert_to_code_table(IC, cur_code);
	}
	else /*(opcode_group == 0)*/
	{
		fprintf(stderr, "This instruction word does not exists");
		error++;
	}
}

void stage1_finish() /*the function parse update the addresses after we have done stage1, cheack the IC and DC*/
{
	symbol *s = S_head;
	data *d = D_head;
	ICF += 4;
	while (s) /*Updates the counters in the tables*/
	{
		if (strcmp(s->type, "data") == 0)
			s->val += ICF;
		s = s->next;
	}

	while (d)
	{
		d->dc += ICF;
		d = d->next;
	}
}
