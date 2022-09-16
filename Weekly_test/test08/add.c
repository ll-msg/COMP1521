#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "add.h"

// return the MIPS opcode for add $d, $s, $t
uint32_t make_add(uint32_t d, uint32_t s, uint32_t t) {

    s = s << 21;
    t = t << 16;
    d = d << 11;
    uint32_t num = (s | t) | d;
    num = num | (1 << 5);

    return num;

}
