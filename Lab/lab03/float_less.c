// Compare 2 floats using bit operations only

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "floats.h"

float_components_t floating_bits(uint32_t f);
int n_an(float_components_t f);

// float_less is given the bits of 2 floats bits1, bits2 as a uint32_t
// and returns 1 if bits1 < bits2, 0 otherwise
// 0 is return if bits1 or bits2 is Nan
// only bit operations and integer comparisons are used
uint32_t float_less(uint32_t bits1, uint32_t bits2) {

    float_components_t bits_1 = floating_bits(bits1);
    float_components_t bits_2 = floating_bits(bits2);

    // when both 0
    if (bits1 == 0 && bits2 == 0){
        return 0;
    }

    // when one of them is Nan
    if (n_an(bits_1) == 1 || n_an(bits_2) == 1){
        return 0;
    }
    
    
    // main part
    // compare sign
    if (bits_1.sign == 0 && bits_2.sign == 1){
        return 0;
    } else if (bits_1.sign == 1 && bits_2.sign == 0){
        return 1;
    }
    // compare exponent
    if (bits_1.sign == 0 && bits_2.sign == 0){
        if (bits_1.exponent > bits_2.exponent){
            return 0;
        } else if (bits_1.exponent < bits_2.exponent){
            return 1;
            // compare fraction when exponents are the same
        } else {
            if (bits_1.fraction >= bits_2.fraction){
                return 0;
            } else {
                return 1;
            }
        }
    }

    if (bits_1.sign == 1 && bits_2.sign == 1){
        if (bits_1.exponent < bits_2.exponent){
            return 0;
        } else if (bits_1.exponent > bits_2.exponent){
            return 1;
            // compare fraction when exponents are the same
        } else {
            if (bits_1.fraction <= bits_2.fraction){
                return 0;
            } else {
                return 1;
            }
        }
    }
    return 0;  
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

int n_an(float_components_t f) {
    
    if (((f.fraction > 0) | (f.fraction < 0)) && f.exponent == 0xff) {
        return 1;
    } else {
        return 0;
    }
}