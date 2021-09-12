#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "all_stages.h"

/*data table functions*/
void insert_to_data_table(int dc, int val, int type) /*the function insert data to data table*/
{
	data *curr_d;
	data *new_d = NULL;
	new_d = (data *)malloc(sizeof(data));
	new_d->type = type;
	new_d->dc = dc;
	new_d->val = val;
	new_d->next = NULL;

	if (!D_head) /*if the data table is empty*/
	{
		D_head = new_d;
	}
	else
	{
		curr_d = D_head;
		while (curr_d->next) /*insert to the end of the table*/
			curr_d = curr_d->next;
		curr_d->next = new_d;
	}
}
void print_data_table() /*the function print data table*/
{
	int i;
	data *d = D_head;

	while (d)
		for (i = 0; d != NULL; i++)
		{
			printf("\n%d\t", d->dc);
			/*	for (i = 0; i < (d->type); i++)*/
			printf("%x", d->val);
			printf(" ");
			d = d->next;

			if (i == 3)
			{
				printf("\n");
				i = 0;
			}
		}
}
int print_data_table_to_file(FILE *file)
{
	int i, j, temp, left = 0;
	data *d = D_head;
	while (d)
	{
		for (i = 0; i < 4; i++)
		{
			if (!d)
				return 0;

			if (i == 0)
				fprintf(file, "\n%d\t", (d->dc - (d->dc % 4)));

			if (left > 0)
			{
				i = rest_code(left, temp, i, file);
				if (d->next != NULL)
				{
					d = d->next;
				}
				else
					return 0;
			}

			if (d->type == CODE_DB)
			{
				print_hex(d->val, file);
				temp = 0;
				left = 0;
				if (d->next != NULL)
				{
					d = d->next;
				}
				else
					return 0;
			}

			else if (d->type == CODE_DW)
			{
				if (i == 0)
				{
					for (j = 0; j < 4; j++)
					{
						d->val = d->val;
						print_hex(d->val, file);
						d->val >>= 8;
					}
					if (d->next != NULL)
					{
						d = d->next;
						i = 0;
						left = 0;
					}
					else
						return 0;
				}
				else
				{
					left = 4;
					temp = d->val;
					while (i < 4)
					{
						print_hex(temp, file);
						temp >>= 8;
						left--;
						i++;
					}
				}
			}

			else if (d->type == CODE_DH)
			{
				print_hex(d->val, file);

				d->val >>= 8;
				if (i == 3)
				{
					temp = d->val;
					left = 1;
				}
				else
				{
					print_hex(d->val, file);

					i++;
					left = 0;
					temp = 0;
					if (d->next != NULL)
					{
						d = d->next;
					}
				}
			}
		}
	}
	return 0;
}

/*code table functions*/
void insert_to_code_table(int ic, unsigned int code) /*the function insert code to code table UNSIGNED?!?!*/
{
	int replace_code = 1;
	coding *curr_c;
	coding *new_c = NULL;
	new_c = (coding *)malloc(sizeof(coding));
	new_c->ic = ic;
	new_c->code = code;
	new_c->next = NULL;

	if (!C_head) /*if the			code table is empty*/
	{
		C_head = new_c;
	}
	else
	{
		curr_c = C_head;
		while (curr_c->next) /*insert to the end of the table*/
		{
			if (curr_c->ic == ic)
			{
				curr_c->code = new_c->code;
				replace_code = 0;
			}
			curr_c = curr_c->next;
		}
		if ((replace_code))
			curr_c->next = new_c;
	}
}
void print_code_table_to_file(FILE *file)
{
	int i;
	coding *c = C_head;
	fprintf(file, "\t\t%d %d", IC - 96, DC);

	while (c)
	{
		fprintf(file, "\n%d\t", c->ic);
		for (i = 0; i < 4; i++)
		{
			fprintf(file, "%X", (c->code & 0xF0) >> 4); /* high nibble */
			fprintf(file, "%X ", c->code & 0x0F);		/* low nibble */
			/*finished 1 byte, now shift right 8 bit to get the next byte */
			c->code >>= 8;
		}
		c = c->next;
	}
}

/*symbol table functions*/
void insert_to_symbol_table(char name[], int val, char type[]) /*the function insert symbol to symbol table*/
{
	symbol *curr_s;
	symbol *new_s = NULL;
	new_s = (symbol *)malloc(sizeof(symbol));

	strcpy(new_s->name, name);
	new_s->val = val;
	strcpy(new_s->type, type);
	new_s->next = NULL;

	if (!S_head) /*if the symbol table is empty*/
	{
		S_head = new_s;
	}
	else
	{
		curr_s = S_head;
		while (curr_s->next) /*insert to the end of the table*/
			curr_s = curr_s->next;
		curr_s->next = new_s;
	}
}
void print_symbol_table() /*the function print symbol table*/
{
	symbol *s = S_head;
	while (s)
	{
		printf("\n%s\t%d\t%s", s->name, s->val, s->type);
		s = s->next;
	}
	printf("\n");
}

/*syntax analysis*/
void substring(char s[], char sub[], int from, int to) /*the function cat row form 'from' index to 'to' index*/
{
	int c = 0;
	while (from < to)
	{
		sub[c] = s[from];
		c++;
		from++;
	}
	sub[c] = '\0';
}
int findspace(char row[]) /*the function find the next space in the row end return his index*/
{
	int len = strlen(row);
	int i;
	for (i = 0; i < len; i++)
	{
		if (row[i] == ' ' || row[i] == '\t' || row[i] == '\n' || row[i] == ',' || row[i] == EOF)
			return i;
	}
	return 0;
}
int find_not_space(char row[]) /*the function find the next char that is not space in the row end return his index*/
{
	int len = strlen(row);
	int i;

	for (i = 0; i < len; i++)
	{
		if (row[i] != ' ' && row[i] != '\t' && row[i] != '\n' && row[i] != ',' && row[i] != EOF)
			return i;
	}
	return 0;
}
void get_word(char row[], char word[]) /*the function find the first word in the row*/
{
	int s, e, len; /*start of word and end of word*/
	char rest_row[REST_ROW];

	len = strlen(row);

	s = find_not_space(row);
	e = 0;
	if (s == 0)
	{
		e = findspace(row);
		if (e != 0)
			substring(row, word, s, e);
		else
		{
			len = strlen(row);
			substring(row, word, s, len);
		}
	}
	else
	{
		substring(row, rest_row, s, len);
		e = findspace(rest_row);
		substring(rest_row, word, 0, e);
	}
}
int find_end_row(char row[]) /*the function find the index that is the end of the row*/
{
	int i, len = strlen(row);

	for (i = 0; i < len; i++)
	{
		if (row[i] == '\n' || row[i] != EOF)
			return i;
	}
	return 0;
}
void get_rest_row(char row[], char rest_row[]) /*the function find the rest of the row, after the first word in the row*/
{
	int s, e; /*start of word and end of word*/

	e = strlen(row);

	s = findspace(row);
	substring(row, rest_row, s + 1, e);
	s = find_not_space(rest_row);
	if (s != 0)
		substring(rest_row, rest_row, s, e);
}

/*codinf function*/
void print_hex(int val, FILE *file)

{
	fprintf(file, "%X", (val & 0xF0) >> 4);
	fprintf(file, "%X ", val & 0x0F);
}
int rest_code(int left, int temp, int i, FILE *file)
{
	while (left > 0)
	{
		print_hex(temp, file);
		temp >>= 8;
		i++;
		left--;
	}
	return i;
}
int get_register(char row[])
{
	int regi = 0;
	char reg[3];
	get_word(row, row);
	if (row[0] == '$')
	{
		substring(row, reg, 1, strlen(row));
		regi = atoi(reg);
	}
	if ((regi < 0) || (regi > 31))
	{
		fprintf(stderr, "This register does not exists\n");
		error++;
	}
	return regi;
}
