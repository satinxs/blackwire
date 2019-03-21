#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "vm.h"

void printUsage() {
	printf("  Usage:\n"
	       "       black file            // Interpret file directly.\n"
	       "       black -d file         // Dump file bytecode.\n"
	       "       black -c file fileout // Compile file and output to fileout.\n"
	      );
}

int main(int argc, char **argv) {

	int i;
	BlackProgram *p;

	if (argc < 2 || argc > 4) {
		printUsage();
		return 0;
	}

	if (argc == 2 ) {
		if (strcmp(argv[1], "--help")==0 || strcmp(argv[1], "-h")==0) {
			printUsage();
			return 0;
		}
	
		//clock_t t1 = (clock() / (CLOCKS_PER_SEC / 1000));
		p = black_CompileFile(argv[1]);
		//clock_t t2 = (clock() / (CLOCKS_PER_SEC / 1000));
		//printf("Compile time: %dms\n", t2 - t1);
		return black_Run(p);
	}
	else if (argc == 3) {
		if (strcmp("-d", argv[1]) == 0) {
			p = black_CompileFile(argv[2]);
			for (i = 0; i < p->length; i++)
				printf("[%d] %d %d\n", i, p->prog[i].op, p->prog[i].data);
		}
	}
	else if (argc == 4) {
		if (strcmp("-c", argv[1]) == 0) {
			BlackProgram *p = black_CompileFile(argv[2]);
			black_ToFile(p, argv[3]);
		}
	}
	
	return 0;
}