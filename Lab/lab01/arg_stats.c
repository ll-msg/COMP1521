#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {
    int min = strtol(argv[1], NULL, 10);
    int max = 0;
    int sum = 0;
    int prod = 1;
    int mean = 0;
    for (int num = 1; num < argc; num++) {
        int cal = strtol(argv[num], NULL, 10);
        if (min >= cal){
            min = cal;
        }
        if (max <= cal){
            max = cal;
        }
        sum += cal;
        prod *= cal;
        mean = sum / (argc - 1);  
    }
    printf ("MIN:  ");
    printf ("%d\n", min);
    printf ("MAX:  ");
    printf ("%d\n", max);
    printf ("SUM:  ");
    printf ("%d\n", sum);
    printf ("PROD: ");
    printf ("%d\n", prod);
    printf ("MEAN: ");
    printf ("%d\n", mean);
}