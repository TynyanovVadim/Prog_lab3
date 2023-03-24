#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "coder.h"

int encode(uint32_t code_point, CodeUnits *code_units)
{
    if (!code_units) {
        return -1;
    }
    uint8_t buf[4] = {0};

    if (code_point < 128) {
        code_units->code[0] = (code_point);
        code_units->length = 1;
        return 0;
    }
    size_t bytes_count = 1;
    while (code_point >= 0x80 >> (bytes_count)) {
        buf[bytes_count - 1] = ((code_point & 0x3f) | 0x80);
        bytes_count++;
        code_point >>= 6;
        if (bytes_count > 4) {
            return -1;
        }
    }
    buf[bytes_count - 1] = (code_point | (0xff << (8 - bytes_count)));
    for (int i = 0; i < bytes_count; i++) {
        code_units->code[i] = buf[bytes_count - i - 1];
    }
    code_units->length = bytes_count;
    return 0;
}

uint32_t decode(const CodeUnits *code_units)
{
    uint32_t value = ((code_units->code)[0] & (0xff >> code_units->length));
    for (size_t i = 1; i < code_units->length; i++) {
        value <<= 6;
        value += ((code_units->code)[i] & 0x3f);
    }
    return value;
}

int read_next_code_unit(FILE *in, CodeUnits *code_units)
{
    uint8_t byte;
    size_t bytes_count;
    while (1) {
        if (fread(&byte, 1, 1,in) != 1) {
            return -1;
        }
        bytes_count = 0;
        if (byte > 0xf0) { //0xf0 == 1111 0000
            bytes_count = 4;
        } else if (byte > 0xe0) { //0xe0 == 1110 0000
            bytes_count = 3;
        } else if (byte > 0xc0) { //0xc0 == 1100 0000
            bytes_count = 2;
        } else if (byte < 0x80) {
            bytes_count = 1;
        } else { 
            continue;
        }
        break;
    }
    code_units->length = bytes_count;
    code_units->code[0] = byte;
    for (size_t i = 1; i < bytes_count; i++) {
        fread(&byte, 1, 1, in);
        code_units->code[i] = byte;
    }
    return 0;
}

int write_code_unit(FILE *out, const CodeUnits *code_units)
{
    fwrite(code_units->code, 1, code_units->length, out);
    return 0;
}
