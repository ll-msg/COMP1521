#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int bcd(int bcd_value);

int main(int argc, char *argv[]) {

    for (int arg = 1; arg < argc; arg++) {
        long l = strtol(argv[arg], NULL, 0);
        assert(l >= 0 && l <= 0x0909);
        int bcd_value = l;

        printf("%d\n", bcd(bcd_value));
    }

    return 0;
}

// given a  BCD encoded value between 0 .. 99
// return corresponding integer
int bcd(int bcd_value) {

    int n_digits = 2 * sizeof(bcd_value);
    int hex_result = 0;
    int array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int index = 0;

    for (int digit = n_digits - 1; digit >= 0; digit--){

        hex_result = (bcd_value >> (4 * (n_digits - digit - 1))) & 0xF;
        array[index] = hex_result;
        index++;
    
    }
    int result = array[1] * 10 + array[2] * 10 + array[0];
    return result;
}


