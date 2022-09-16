// Print out a float.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "floats.h"
float_components_t floating_bits(uint32_t f);

//
// float_print is given the bits of a float as a uint32_t
// it prints out the float in the same format as "%.9g\n"
// using only putchar & puts
//
void float_print(uint32_t bits) {

    float_components_t bit = floating_bits(bits);
    
    if (bit.sign == 1) {
        putchar('-');
    }
    
    
    putchar('\n');
    
}

float_components_t floating_bits(uint32_t f) {
    
    float_components_t result;
    // sign
    result.sign = (f >> 31) & 0xF;

    // exponent
    result.exponent = (f >> 23) & 255;
    
    // fraction
    result.fraction = f & 8388607;

    return result;
}