/* Simple CPU */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAIN_GLOBALS /* To get definitions */
#include "main.h"

void usage(void)
{
	printf("Usage: simplecpu filenametoassembleandrun [Y]\n");
	printf("Where Y is optionally \"Print debug\"\n");
}

int main(int argc, char *argv[])
{
	char* debugVal=0;

	if (argc < 2) {
		usage();
		return -1;
	}

	if (argv[2] && (*argv[2] == 'Y' || *argv[2] == 'y'))
		debugprint = 1;

	tokenize(argv[1]);
	assemble();
	runtime();
	return 0;
}
