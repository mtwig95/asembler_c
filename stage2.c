#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "all_stages.h"

/*The functions that belong to Stage 2 - which aims to parsee the line, encode it, and export the output files*/

void stage2_parse_row(char myrow[], char label[], char is_label[], char *fname) /*the function parse the row for second time, with stage 2 algoritm*/
{
	int i, len, lenword, opcode_group;
	char word[MAX_WORD], row[REST_ROW], nlabel[REST_ROW], temp[REST_ROW], rest_row[REST_ROW], is_parse[6] = FALSE, is_intruction[6] = FALSE;
	char no_space[MAX_WORD];
	strcpy(row, myrow);
	len = strlen(row);
	strcpy(temp, row);
	get_word(temp, word); /*the first word in the row*/
	lenword = strlen(word);
	if (len == 0) /*check if empty row*/
		return;

	if (myrow[0] == ';') /*check if comment statement*/
		return;

	if (strcmp(is_label, FALSE) == 0) /*check if label*/
	{
		if (word[lenword - 1] == ':')
		{
			strcpy(rest_row, row);
			get_rest_row(row, rest_row);
			strcpy(temp, row);
			get_word(temp, nlabel);
			lenword = strlen(nlabel);
			substring(nlabel, nlabel, 0, lenword - 1);
			strcpy(is_label, TRUE);
			get_rest_row(myrow, myrow);
			get_word(myrow, word);
		}
	}

	if (word[0] == '.') /*check if directive word*/
	{
		stage2_coding_directive(label, word, row, fname);
		strcpy(is_parse, TRUE);
	}
	substring(word, no_space, 0, strlen(word) - 1);
	for (i = 0; i < 6; i++) /*check if instruction word*/
	{
		opcode_group = check_opcode_group(word);
		if ((opcode_group != 0))
		{
			get_rest_row(temp, row);
			stage2_coding_instruction(temp, opcode_group, fname);
			strcpy(is_intruction, TRUE);
			IC += 4;
			return;
		}
	}

	if (strcmp(is_intruction, FALSE) == 0)
	{
		if (strcpy(is_parse, FALSE) == 0)
		{
			fprintf(stderr, "Line is invalid\n");
			error++;
		}
	}
}

void stage2_coding_directive(char label[], char word[], char row[], char *fname) /*the function coding directive statement*/
{
	if (strcmp(word, ".extern") == 0)/*check if extern*/
	{
		get_rest_row(row, row);
		get_word(row, label);
		return;
	}

	else if (strcmp(word, ".entry") == 0) /*check if entry*/
	{
		get_rest_row(row, row);
		stage2_parse_entry(row, fname);
	}
	else if ((strcmp(word, ".asciz") == 0) || (strcmp(word, ".db") == 0) || (strcmp(word, ".dh") == 0) || (strcmp(word, ".dw") == 0)) /*data statement already coded in stage 1*/
		return;																														  
}

void stage2_parse_entry(char row[], char *fname) /*the function coding entry statement and add information to the .ent*/
{
	symbol *s = S_head;
	FILE *f;
	int val, n;
	char symbol[MAX_WORD], name[MAX_WORD], sub[MAX_WORD];
	/*create or update entry file*/
	substring(row, symbol, 0, strlen(row) - 2);
	n = strlen(fname);
	substring(fname, sub, n - 4, n);
	strcpy(name, fname);
	if (strcmp(sub, ".ent") != 0)
		strcat(name, ".ent");

	while (s)
	{
		/*Add entry property, build and update the output file .ent */
		if ((strcmp(s->name, symbol) == 0) || (strcmp(s->name, row) == 0))
		{
			strcat(s->type, ", entry");
			f = fopen(name, "a"); /*open file*/
			val = s->val;
			fprintf(f, "%s\t0%d\n ", symbol, val);
			fclose(f);
			return;
		}
		s = s->next;
	}
	fprintf(stderr, "\nThis symbol is not exists in the symbols table - %s\n", row);
	error++;
}

void stage2_parse_ext(char symbol[], int ic, char *fname) /*the function coding extern statement and add information to the ps.ext*/
{
	int len;
	char sub[MAX_WORD], name[MAX_WORD];
	FILE *f;
	/*create or update ext file*/
	len = strlen(fname);
	substring(fname, sub, len - 4, len);
	strcpy(name, fname);
	if (strcmp(sub, ".ext") != 0)
		strcat(name, ".ext");
	f = fopen(name, "a");
	fprintf(f, "%s\t0%d\n", symbol, ic);
	fclose(f);
}

void stage2_coding_instruction(char myrow[], int opcode_group, char *fname) /*The function completes the encoding for instructions that require the use of a symbols table*/
{
	code my_code;
	unsigned int cur_code;
	char args[REST_ROW];
	char opcode[MAX_WORD];
	get_word(myrow, opcode);
	get_rest_row(myrow, args);
	if ((opcode_group == 4)) /*group I*/
	{
		my_code.type = I;
		my_code = code_opcode(opcode, my_code);
		my_code = i_second(args, my_code);
		cur_code = code_32bit_I(my_code);
		insert_to_code_table(IC, cur_code);
	}
	if ((opcode_group == 6)) /*group J*/
	{
		if (args[0] != '$') /*it is not a register*/
		{
			my_code.type = J;
			my_code = code_opcode(opcode, my_code);
			my_code = j_second(args, my_code, fname);
			if (my_code.my_ops.J.opcode == 63)
			{
				my_code.my_ops.J.address = 0;
			}
			cur_code = code_32bit_J(my_code, 2);
			insert_to_code_table(IC, cur_code);
		}
	}
}

void stage2_finish_outfput(char *fname)
{
	FILE *f;

	strcat(fname, ".ob"); /*Creates a new ob file*/
	f = fopen(fname, "w");

	print_code_table_to_file(f);
	print_data_table_to_file(f);
	fclose(f);
}