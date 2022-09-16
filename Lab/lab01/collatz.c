#include <stdio.h>
#include <stdlib.h>

int collatz(int num);
int main(int argc, char **argv)
{
	collatz(strtol(argv[1], NULL, 10));
	return 0;
}

int collatz(int num) {
    
    printf("%d\n", num);
    
    if (num == 1) {
        return num;
    } else if (num % 2 != 0) {
        return collatz(num * 3 + 1);
    } else {
        return collatz(num / 2);
    }
    
}
    
