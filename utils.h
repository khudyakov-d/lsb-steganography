#ifndef LSB_UTILS_H
#define LSB_UTILS_H

#include <stdint.h>
#include <bits/types/FILE.h>

int file_size(FILE *fp);

uint8_t get_nth_byte(uint32_t value, int n);

void copy_file(FILE *input, FILE *output);

#endif //LSB_UTILS_H
