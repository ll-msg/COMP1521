// COMP1521 21T2 ... final exam, question 0

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int
count_leading_zeroes (uint32_t x)
{
    int count = 0;
    int index = 0;
    while (index < 32) {
        if ((x & 1) == 1) {
            count = 0;
        } else {
            count++;
        }
        x = x >> 1;
        index++;
    }
    return count;
}

