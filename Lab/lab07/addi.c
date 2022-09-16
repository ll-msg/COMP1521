// generate the opcode for an addi instruction

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "addi.h"

// return the MIPS opcode for addi $t,$s, i
uint32_t addi(int t, int s, int i) {

    s = s << 21;
    t = t << 16;
    if (i < 0) {
        i = -i;
        i = ~i;
        uint16_t number = i + 1;
        i = number;
    }
    uint32_t num = 0x20000000;

    return ((num | s) | t) | i;
}
