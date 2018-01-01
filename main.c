/* Simple CPU */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAIN_GLOBALS /* To get definitions */
#include "main.h"

void usage(void)
{
	printf("Usage:\n\n");
	printf("main filenametoassembleandrun <optional>1\n");
	printf("1 is 'debug' flag\n");
}

int main(int argc, char *argv[])
{
	uint64_t cycles=0;

	if (argc < 2) {
		usage();
		return -1;
	}

	tokenize(argv[1]);
	if (argc == 3) {
		debugprint = atoi(argv[2]);
	}

	assemble();
	cycles = runtime();
	printf("Finished runtime, executed %llu cycles\n", cycles);
	return 0;
}
