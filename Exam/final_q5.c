// COMP1521 21T2 ... final exam, question 5
// COMP1521 21T2 ... final exam, question 5

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void
print_utf8_count (FILE *file)
{
	unsigned long amount_1_byte = 0;
	unsigned long amount_2_byte = 0;
	unsigned long amount_3_byte = 0;
	unsigned long amount_4_byte = 0;
    
	ssize_t utf8_count;
    int utf8_bytes;
    
	for (utf8_count = 0; (utf8_bytes = fgetc(file)) != EOF; utf8_count++) {
        if ((utf8_bytes & 0x80) == 0x00) {
   			amount_1_byte++;
        } else if ((utf8_bytes & 0xE0) == 0xC0) {
   			amount_2_byte++;
        } else if ((utf8_bytes & 0xF0) == 0xE0) {
   			amount_3_byte++;
        } else if ((utf8_bytes & 0xF8) == 0xF0) {
   			amount_4_byte++;
        }

    }

	printf("1-byte UTF-8 characters: %lu\n", amount_1_byte);
	printf("2-byte UTF-8 characters: %lu\n", amount_2_byte);
	printf("3-byte UTF-8 characters: %lu\n", amount_3_byte);
	printf("4-byte UTF-8 characters: %lu\n", amount_4_byte);
}
