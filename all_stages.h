#define MAX_ROW 80
#define MAX_WORD 20
#define REST_ROW 40
#define CODE_DB 8
#define CODE_DH 16
#define CODE_DW 32

#define TRUE "truee"
#define FALSE "false"

extern int IC;
extern int ICF;
extern int DCF;
extern int DC;
extern int error;
extern char SAME_MEM_WORD[MAX_WORD];

extern char *GROUP1[]; /*R - Arithmetic and logical instructions*/
extern char *GROUP2[]; /*R - Copy instructions*/
extern char *GROUP3[]; /*I - Arithmetic and logical instructions*/
extern char *GROUP4[]; /*I - Conditional branching instructions*/
extern char *GROUP5[]; /*I - Instructions for loading and keeping in memory*/
extern char *GROUP6[]; /*J Instructions*/

enum op_type
{
    R = 0,
    I,
    J
};
typedef struct R_CMD
{
    unsigned int opcode : 6;
    unsigned int rs : 5;
    unsigned int rt : 5;
    unsigned int rd : 5;
    unsigned int funct : 5;
    unsigned int unused : 6;
} R_CMD;

typedef struct I_CMD
{
    unsigned int opcode : 6;
    unsigned int rs : 5;
    unsigned int rt : 5;
    signed int immed : 16;
} I_CMD;

typedef struct J_CMD
{
    unsigned int opcode : 6;
    unsigned int reg : 5;
    unsigned int address : 25;
} J_CMD;
union ops
{
    R_CMD R;
    I_CMD I;
    J_CMD J;
};
typedef struct code
{
    union ops my_ops;
    int IC;
    enum op_type type; /* R I J */
} code;
typedef struct code_array
{
    code *arr;
    int size;
} code_array;

typedef struct symbol_table /*symbol table - linked list*/
{
    char name[MAX_WORD];
    int val;             /*val or address*/
    char type[MAX_WORD]; /*macro or directive or instruction*/
    struct symbol_table *next;
} symbol;

typedef struct data_table /*data table - linked list*/
{
    int dc;
    int val;
    int type;
    struct data_table *next;
} data;

typedef struct code_table /*data table - linked list*/
{
    int ic;
    unsigned int code;
    code mycode;
    struct code_table *next;
} coding;

extern symbol *S_head;
extern data *D_head;
extern coding *C_head;

/*main functions*/
int stage1(FILE *fp);
int stage2(FILE *fp, char *fname);

/*The functions that belong to Stage 1 - which aims to perform initial parseing of rows, and update the symbol table, data table ,IC and DC*/
void stage1_parse_row(char row[], char label[], char is_label[]);
void stage1_parse_directive(char row[], char word[], char label[], char is_label[]);                       /*directive statement*/
void stage1_insert_to_symbol_table(char row[], char label[], char is_label[], char type[], int macro_val); /*update symbol table*/
void stage1_parse_asciz(char row[]);                                                                       /*asciz*/
void stage1_parse_data(char row[], int size, int num_byte);                                                /*data*/
void stage1_parse_instruction(char row[], char word[], char is_label[]);                                   /*instruction statement*/
void stage1_finish();                                                                                      /*final update IC for data statement*/

/*The functions that belong to Stage 2 - which aims to parsee the line, encode it, and export the output files*/
void stage2_parse_row(char row[], char label[], char is_label[], char *fname);
void stage2_coding_directive(char label[], char word[], char row[], char *fname);
void stage2_parse_entry(char row[], char *fname);                            /*entry*/
void stage2_parse_ext(char symbol[], int ic, char *fname);                   /*external*/
void stage2_coding_instruction(char myrow[], int opcode_group, char *fname); /*the function coding instruction statement - the first memmory word, and after the added memmory word that we need to add*/
void stage2_finish_outfput(char *fname);                                     /*write into the object file*/

/*auxiliary_func*/
/*data table functions*/
void insert_to_data_table(int dc, int val, int type);
void print_data_table();
int print_data_table_to_file(FILE *file);

/*code table functions*/
void insert_to_code_table(int IC, unsigned int code);
void print_code_table_to_file(FILE *file);

/*symbol table functions*/
void insert_to_symbol_table(char name[], int val, char type[]);
void print_symbol_table();

/*syntax analysis*/
void substring(char s[], char sub[], int from, int to);
int findspace(char row[]);
int find_not_space(char row[]);
void get_word(char row[], char word[]);
int find_end_row(char row[]);
void get_rest_row(char row[], char restrow[]);

/*codinf function*/
int rest_code(int left, int temp, int i, FILE *file);
void print_hex(int val, FILE *file);
int get_register(char row[]);

/*functions for the instructions*/
int check_opcode_group(char opcode[]);
int code_32bit_J(code my_code, int reg);
int code_32bit_R(code my_code);
int code_32bit_I(code my_code);

code code_opcode(char opcode[], code my_code);
code three_reg(char args[], code my_code);
code two_reg(char args[], code my_code);
code i_first(char args[], code my_code);               /*codind reg, immed, reg*/
code i_second(char args[], code my_code);              /*codind reg, immed, reg*/
code i_second2(char args[], code my_code);             /*codind reg, immed, reg*/
code j_ins(char args[], code my_code);                 /*codind reg, immed, reg*/
code j_second(char args[], code my_code, char *fname); /*codind reg, immed, reg*/
