#include <glob.h>
#include <stdio.h>
#include "utils.h"

int file_size(FILE *fp) {
    int prev = (int) ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int size = (int) ftell(fp);
    fseek(fp, prev, SEEK_SET);
    return size;
}

uint8_t get_nth_byte(uint32_t value, int n) {
    return (uint8_t) (value >> (8 * n));
}

void copy_file(FILE *input, FILE *output) {
    char buffer[BUFSIZ];
    size_t nread;
    fseek(input, 0, SEEK_SET);

    while ((nread = fread(buffer, sizeof(char), sizeof(buffer), input)) != 0) {
        fwrite(buffer, sizeof(char), nread, output);
    }
}

