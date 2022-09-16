#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define N_BCD_DIGITS 8

uint32_t packed_bcd(uint32_t packed_bcd);
int power(int num_1, int num_2);

int main(int argc, char *argv[]) {

    for (int arg = 1; arg < argc; arg++) {
        long l = strtol(argv[arg], NULL, 0);
        assert(l >= 0 && l <= UINT32_MAX);
        uint32_t packed_bcd_value = l;

        printf("%lu\n", (unsigned long)packed_bcd(packed_bcd_value));
    }

    return 0;
}

// given a packed BCD encoded value between 0 .. 99999999
// return the corresponding integer
uint32_t packed_bcd(uint32_t packed_bcd_value) {

    int n_digits = 2 * sizeof(packed_bcd_value);
    int hex_result = 0;
    int array[64] = {0};
    int index = 0;

    for (int digit = n_digits - 1; digit >= 0; digit--){

        hex_result = (packed_bcd_value >> (4 * (n_digits - digit - 1))) & 0xF;
        array[index] = hex_result;
        index++;
    
    }
    int result = 0;
    for (int i = 9; i >= 0; i--){
        result = result + array[i] * power(10, i);
    }
    return result;

}

int power(int num_1, int num_2) {

    uint64_t result = 1;
    for (int i = 0; i < num_2; ++i) {
        result = result * num_1;
    }
    return result;
}
