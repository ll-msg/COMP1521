#include <stdio.h>
#include <ctype.h>

int main(void) {
    int str;
	while ((str = getchar()) != EOF){
	   str = tolower(str);
	   putchar(str);
	}
	return 0;
}

