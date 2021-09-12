prog: auxiliary_func.o instruction_func.o stage1.o stage2.o main.o 
	gcc -g -Wall -ansi -pedantic auxiliary_func.o instruction_func.o stage1.o stage2.o main.o -o prog -lm


main.o: main.c auxiliary_func.o instruction_func.o stage1.o stage2.o all_stages.h
	gcc -g -c -Wall -ansi -pedantic main.c -o main.o -lm

auxiliary_func.o: auxiliary_func.c all_stages.h
	gcc -g -c -Wall -ansi -pedantic auxiliary_func.c -o auxiliary_func.o -lm

instruction_func.o: instruction_func.c all_stages.h
	gcc -g -c -Wall -ansi -pedantic instruction_func.c -o instruction_func.o -lm

stage1.o: stage1.c auxiliary_func.o instruction_func.o all_stages.h
	gcc -g -c -Wall -ansi -pedantic stage1.c -o stage1.o -lm

stage2.o: stage2.c auxiliary_func.o instruction_func.o all_stages.h
	gcc -g -c -Wall -ansi -pedantic stage2.c -o stage2.o -lm

