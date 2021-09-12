#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "all_stages.h"

int IC = 100, DC = 0, ICF = 0, DCF = 0, error = 0;

char *GROUP1[] = {"add", "sub", "and", "or", "nor", ""};
char *GROUP2[] = {"move", "mvhi", "mvlo", "", "", ""};
char *GROUP3[] = {"addi", "subi", "andi", "ori", "nori", ""};
char *GROUP4[] = {"bne", "beq", "blt", "bgt", "", ""};
char *GROUP5[] = {"lb", "sb", "lw", "sw", "lh", "sh"};
char *GROUP6[] = {"jmp", "la", "call", "stop", "", ""};

symbol *S_head = NULL;
data *D_head = NULL;
coding *C_head = NULL;

int main(int argc, char *argv[])
{
	int i = 1;
	char *fname;
	FILE *fp;

	if (argc < 2) /*check if we get the file*/
	{
		fprintf(stderr, "Usage: ./prog file_name\n");
		exit(1);
	}
	if (argc >= 2) /*check if we get the file*/
	{
		for (i = 0; i < argc; i++)
		{
			fname = argv[i];
			fp = fopen(fname, "r");
			if (!fp)
			{
				fprintf(stderr, "can not open file %s\n", fname);
				exit(1);
			}
		}
		/*~~~~~~~~~~~stage 1~~~~~~~~~~~~~~*/ /*if an error is detected at this stage the will be stopped*/
		if (stage1(fp) > 0)
		{
			fprintf(stderr, "An error was detected in stage 1, the program stopped.\n");
			return 0;
		}
		/*~~~~~~~~~~~stage 2~~~~~~~~~~~~~~*/ /*if an error is detected at this stage the will be stopped*/
		if (stage2(fp, fname) > 0)
		{
			fprintf(stderr, "An error was detected in stage 2, the program stopped.\n");
			return 0;
		}
	}
	printf("Finish\n");
	return 0;
}

int stage1(FILE *fp)
{
	char myrow[MAX_ROW], temprow[MAX_ROW], is_label[] = FALSE, label[MAX_WORD];
	while (!feof(fp)) /*read all the file - stage1*/
	{
		if (fgets(myrow, MAX_ROW, fp) != NULL) /*parsee every line*/
		{
			strcpy(temprow, myrow);
			strcpy(is_label, FALSE); /*initialization is label befor parseing the row*/
			stage1_parse_row(temprow, label, is_label);
		}
	}
	ICF += IC; /*At the end of the first step we will update the instruction counter*/
	DCF += DC; /*t the end of the first step we will update the data counter*/
	stage1_finish(); /*finish function for stage 1*/
	fclose(fp);		 /*close the file and open again to parse every line from the first line*/
	return error;
}

int stage2(FILE *fp, char *fname)
{
	char myrow[MAX_ROW], is_label[] = FALSE, label[MAX_WORD], name[MAX_ROW];
	int n;
	IC = 100;
	n = strlen(fname); /*length of gile name*/
	fp = fopen(fname, "r");
	substring(fname, name, 0, n - 3); /*Get the file name*/
	while (!feof(fp))				  /*read all the file again - stage 2*/
	{
		if (fgets(myrow, MAX_ROW, fp) != NULL) /*parse every line*/
		{
			strcpy(is_label, FALSE);						/*initialization is label befor parseing the row*/
			stage2_parse_row(myrow, label, is_label, name); /*parse with stage 2 algorithm*/
		}
	}
	stage2_finish_outfput(name); /*finish function for stage 2 and all the algorithm*/
	fclose(fp);					 /*close the file*/
	return error;
}