// COMP1521 21T2 ... final exam, question 6

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void
convert_hexdump_to_bytes (FILE *hexdump_input, FILE *byte_output)
{
	// TODO
	// Hint: `man 3 fscanf`
	// Hint: `man 3 fseek`
	// Hint: See question text for
	//        third hint.
	char arr1[8];
	char arr2[8];
	char arr3[8];
	char arr4[8];
	char arr5[8];
	char arr6[8];
	char arr7[8];
	char arr8[8];
	char arr9[8];
	char arr10[8];

	int charac;
	int index = 0;
	while (strcmp(fscanf(hexdump_input, ), "|")) {
		arr1[index] = charac;
		index++;
	}
	printf("%s\n", arr1);
	
}
