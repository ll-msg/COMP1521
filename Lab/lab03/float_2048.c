// Multiply a float by 2048 using bit operations only

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "floats.h"
float_components_t floating_bits(uint32_t f);
int n_an(float_components_t f);
int positive_infinity(float_components_t f);
int negative_infinity(float_components_t f);
int zero(float_components_t f);

// float_2048 is given the bits of a float f as a uint32_t
// it uses bit operations and + to calculate f * 2048
// and returns the bits of this value as a uint32_t
//
// if the result is too large to be represented as a float +inf or -inf is returned
//
// if f is +0, -0, +inf or -inf, or Nan it is returned unchanged
//
// float_2048 assumes f is not a denormal number
//
uint32_t float_2048(uint32_t f) {
    
    float_components_t num = floating_bits(f);
    if (n_an(num) == 1 || negative_infinity(num) == 1 || positive_infinity(num) == 1 || zero(num) == 1) {
        return f;
    }
    
    uint32_t result = (num.sign << 31) + ((num.exponent + 11) << 23) + num.fraction;

    if ((num.sign == 0) && ((num.exponent + 11) > 254)) {
        return 0x7F800000;
    } else if ((num.sign == 1) && (num.exponent + 11) > 254) {
        return 0xFF800000;
    }
    return result;
}

int n_an(float_components_t f) {
    
    if (((f.fraction > 0) | (f.fraction < 0)) && f.exponent == 0xff) {
        return 1;
    } else {
        return 0;
    }
}

// given the 3 components of a float
// return 1 if it is inf, 0 otherwise
int positive_infinity(float_components_t f) {
    
    if (f.sign == 0 && f.exponent == 0xff && f.fraction == 0) {
        return 1;
    } else {
        return 0;
    }

}

// given the 3 components of a float
// return 1 if it is -inf, 0 otherwise
int negative_infinity(float_components_t f) {
    
    if (f.sign == 1 && f.exponent == 0xff && f.fraction == 0) {
        return 1;
    } else {
        return 0;
    }
}

// given the 3 components of a float
// return 1 if it is 0 or -0, 0 otherwise
int zero(float_components_t f) {
    
    if (f.exponent == 0 && f.fraction == 0) {
        return 1;
    } else {
        return 0;
    }
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

