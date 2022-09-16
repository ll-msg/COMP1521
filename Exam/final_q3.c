// COMP1521 21T2 ... final exam, question 3

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void
cp (char *path_from, char *path_to)
{
	// open the from file
	FILE *from_file = fopen(path_from, "r");
	// open the to file
	FILE *to_file = fopen(path_to, "w");
	// copy
	int charac;
	while ((charac = fgetc(from_file)) != EOF) {
		fputc(charac, to_file);
	}
	fclose(from_file);
	fclose(to_file);
}

