#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * Диапазон             Вероятность
 * -------------------- -----------
 * [0; 128)             90%
 * [128; 16384)         5%
 * [16384; 2097152)     4%
 * [2097152; 268435455) 1%
 */
uint32_t generate_number()
{
    const int r = rand();
    const int p = r % 100;
    if (p < 90)
    {
        return r % 128;
    }
    if (p < 95)
    {
        return r % 16384;
    }
    if (p < 99)
    {
        return r % 2097152;
    }
    return r % 268435455;
}

size_t encode_varint(uint32_t value, uint8_t *buf)
{
    assert(buf != NULL);
    uint8_t *cur = buf;
    while (value >= 0x80)
    {
        const uint8_t byte = (value & 0x7f) | 0x80;
        *cur = byte;
        value >>= 7;
        ++cur;
    }
    *cur = value;
    ++cur;
    return cur - buf;
}

uint32_t decode_varint(const uint8_t **bufp)
{
    const uint8_t *cur = *bufp;
    uint8_t byte = *cur++;
    uint32_t value = byte & 0x7f;
    size_t shift = 7;
    while (byte >= 0x80)
    {
        byte = *cur++;
        value += (byte & 0x7f) << shift;
        shift += 7;
    }
    *bufp = cur;
    return value;
}

size_t file_size(FILE *file)
{
    size_t size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

int main()
{
    FILE *uncom_file = fopen("uncompressed", "wb");
    FILE *com_file = fopen("compressed", "wb");

    for (int i = 0; i < 1000000; i++) {
        uint8_t buf[4] = {0};
        uint32_t num = generate_number();
        size_t e_size = encode_varint(num, buf);
        fwrite(&num, 4, 1, uncom_file);
        fwrite(buf, 1, e_size, com_file);
    }

    fclose(com_file);
    fclose(uncom_file);

    uncom_file = fopen("uncompressed", "rb");
    com_file = fopen("compressed", "rb");
    uint32_t num;
    size_t com_file_size = file_size(com_file); 
    uint8_t *buf = malloc(sizeof(uint8_t) * com_file_size);
    uint8_t *buf_free = buf;
    assert(buf != NULL && "Error: memory not allocated");

    for (size_t i = 0; i < com_file_size; i++) {
        fread(buf + i, 1, 1, com_file);
    }

    while (fread(&num, sizeof(int), 1, uncom_file)) {
        assert(num == decode_varint((const uint8_t **)&buf) && "Error: num != decode");
    }

    double com_ratio = (double)file_size(com_file) / (double)file_size(uncom_file);
    printf("compression ratio - %lf\n", com_ratio);
    fclose(com_file);
    fclose(uncom_file);
    free(buf_free);
}
