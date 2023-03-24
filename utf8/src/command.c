#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include "coder.h"
#include "command.h"

int encode_file(const char *in_file_name, const char *out_file_name)
{
    FILE *in = fopen(in_file_name, "rb");
    if (in == NULL) {
        return -1;
    }
    FILE *out = fopen(out_file_name, "wb");

    uint32_t num;
    CodeUnits code_units;
    while (fscanf(in,"%" SCNx32, &num) == 1) {
        if (!encode(num, &code_units)) {
            write_code_unit(out, &code_units);
        }
    }

    fclose(out);
    fclose(in);

    return 0;
}

int decode_file(const char *in_file_name, const char *out_file_name)
{
    FILE *in = fopen(in_file_name, "rb");
    if (in == NULL) {
        return -1;
    }
    FILE *out = fopen(out_file_name, "wb");

    CodeUnits code_units;
    uint32_t num;
    while (!read_next_code_unit(in, &code_units)) {
        num = decode(&code_units);
        fprintf(out, "%" PRIx32 "\n", num);
    }
    fclose(in);
    fclose(out);
    return 0;
}