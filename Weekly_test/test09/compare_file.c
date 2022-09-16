#include <stdio.h>

int main(int argc, char* argv[]) {
    FILE *test1 = fopen(argv[1], "r");
    FILE *test2 = fopen(argv[2], "r");
    FILE *test3 = fopen(argv[1], "r");
    FILE *test4 = fopen(argv[2], "r");

    int charac1 = 0;
    int charac2 = 0;

    int i = 0;
    int num1 = 0;
    int num2 = 0;
    while ((charac1 = fgetc(test1)) != EOF) {
        num1++;
    }
     while ((charac2 = fgetc(test2)) != EOF) {
        num2++;
    }

    fclose(test1);
    fclose(test2);

    while ((charac1 = fgetc(test3)) != EOF && (charac2 = fgetc(test4)) != EOF) {
        if (charac1 != charac2) {
            printf("Files differ at byte %d\n", i);
            return 0;
        }
        i++;
    }
    if (num1 < num2) {
        printf("EOF on %s\n", argv[1]);
        return 0;
    } else if (num1 > num2) {
        printf("EOF on %s\n", argv[2]);
        return 0;
    }

    fclose(test3);
    fclose(test4);
    printf("Files are identical\n");
    return 0;
}