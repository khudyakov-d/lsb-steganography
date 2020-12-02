//
// Created by daniel on 01.12.20.
//

#ifndef LSB_LSB_H
#define LSB_LSB_H

#define EMBED_BITS_COUNT 1

#include <stdint.h>
#include <bits/types/FILE.h>
#include "utils.h"

#pragma pack(push, 1)

typedef struct bitmap_header {
    uint16_t bf_type;
    uint32_t bf_size;
    uint32_t bf_reserved;
    uint32_t bf_off_bits;
} bitmap_header_t;

typedef struct bitmap_info_header {
    uint32_t bi_size;
    int32_t bi_width;
    int32_t bi_height;
    uint16_t bi_planes;
    uint16_t bi_bitCount;
    uint32_t bi_compression;
    uint32_t bi_size_image;
    int32_t bi_x_pels_per_meter;
    int32_t bi_y_pels_per_meter;
    uint32_t bi_clr_used;
    uint32_t bi_clr_important;
} bitmap_info_header_t;

#pragma pack(pop)

uint8_t *load_bitmap(FILE *image_file_ptr, int *bitmap_data_size, int *offset);

void embed_in_bitmap(char *input_image_filename, char *text_file_name, char *output_image_filename);

void extract_from_bitmap(char *input_image_filename, char *text_file_name);

#endif //LSB_LSB_H
