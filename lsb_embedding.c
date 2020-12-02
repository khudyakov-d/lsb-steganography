#include <stdio.h>
#include "lsb.h"

uint8_t *load_bitmap(FILE *image_file_ptr, int *bitmap_data_size, int *offset) {
    bitmap_header_t bitmap_header;

    fread(&bitmap_header, sizeof(bitmap_header), 1, image_file_ptr);

    if (bitmap_header.bf_type != 0x4D42) {
        return NULL;
    }

    bitmap_info_header_t *bitmap_info_header = (bitmap_info_header_t *) malloc(sizeof(bitmap_info_header_t));

    fread(bitmap_info_header, sizeof(bitmap_info_header_t), 1, image_file_ptr);
    fseek(image_file_ptr, bitmap_header.bf_off_bits, SEEK_SET);

    uint8_t *bitmap_data = (uint8_t *) malloc(bitmap_info_header->bi_size_image * sizeof(uint8_t));

    if (bitmap_data == NULL) {
        return NULL;
    }

    if (fread(bitmap_data, bitmap_info_header->bi_size_image, 1, image_file_ptr) == 0) {
        return NULL;
    }

    if (bitmap_data_size != NULL) {
        *bitmap_data_size = bitmap_info_header->bi_size_image;
    }

    if (offset != NULL) {
        *offset = bitmap_header.bf_off_bits;
    }

    free(bitmap_info_header);

    return bitmap_data;
}

void embed_text_size(uint8_t *bitmap_data, uint32_t text_file_size) {
    uint32_t embedded_blocks_per_byte = 8 / EMBED_BITS_COUNT;

    for (int i = 0, size = sizeof(uint32_t); i < size; ++i) {
        uint8_t temp = get_nth_byte(text_file_size, sizeof(uint32_t) - i - 1);

        for (int j = 0; j < embedded_blocks_per_byte; ++j) {
            temp = (uint8_t) ((temp >> j * EMBED_BITS_COUNT) & (0xff >> (8 - EMBED_BITS_COUNT)));

            bitmap_data[i * embedded_blocks_per_byte + j] &= (0xff << EMBED_BITS_COUNT);
            bitmap_data[i * embedded_blocks_per_byte + j] |= temp;
        }
    }
}

void embed_text(uint8_t *bitmap_data, const uint8_t *text, uint32_t text_file_size) {
    int offset = sizeof(uint32_t);
    uint32_t embedded_blocks_per_byte = 8 / EMBED_BITS_COUNT;

    for (int i = 0; i < text_file_size; ++i) {

        for (int j = 0; j < embedded_blocks_per_byte; ++j) {
            uint8_t temp = (uint8_t) ((text[i] >> j * EMBED_BITS_COUNT) & (0xff >> (8 - EMBED_BITS_COUNT)));
            bitmap_data[(i + offset) * embedded_blocks_per_byte + j] &= (0xff << EMBED_BITS_COUNT);
            bitmap_data[(i + offset) * embedded_blocks_per_byte + j] |= temp;
        }
    }
}

int get_embedded_data(FILE *text_file, uint8_t *bitmap_data, int bitmap_data_size) {
    uint32_t text_file_size = (uint32_t) file_size(text_file);
    uint32_t embedded_blocks_per_byte = 8 / EMBED_BITS_COUNT;

    if (text_file_size > (bitmap_data_size / embedded_blocks_per_byte)) {
        return 1;
    }

    uint8_t *text = (uint8_t *) malloc(text_file_size * sizeof(uint8_t));
    size_t res = fread(text, (size_t) text_file_size, 1, text_file);

    if (res != 1) {
        return 2;
    }

    embed_text_size(bitmap_data, text_file_size);
    embed_text(bitmap_data, text, text_file_size);

    free(text);

    return 0;
}

int get_bitmap_with_embedded_data(FILE *input_image, FILE *output_image, uint8_t *bitmap_data,
                                  int bitmap_data_size, int offset) {
    copy_file(input_image, output_image);

    fseek(output_image, offset, SEEK_SET);

    if (fwrite(bitmap_data, (size_t) bitmap_data_size, 1, output_image) == 0) {
        return 1;
    }

    return 0;
}

void embed_in_bitmap(char *input_image_filename, char *text_file_name, char *output_image_filename) {
    FILE *input_image_file = fopen(input_image_filename, "r");
    if (input_image_file == NULL) {
        fprintf(stderr, "Not valid input image file");
        return;
    }

    FILE *output_image_file = fopen(output_image_filename, "w");
    if (output_image_file == NULL) {
        fprintf(stderr, "Not valid output image file");
        return;
    }

    FILE *text_file = fopen(text_file_name, "r");
    if (text_file == NULL) {
        fprintf(stderr, "Not valid text file");
        return;
    }

    int bitmap_size;
    int offset;

    uint8_t *bitmap_data = load_bitmap(input_image_file, &bitmap_size, &offset);
    int res = get_embedded_data(text_file, bitmap_data, bitmap_size);

    if (res == 0) {
        if (get_bitmap_with_embedded_data(input_image_file, output_image_file, bitmap_data, bitmap_size, offset)) {
            fprintf(stderr, "Could not write to the output image file");
        }
    } else if (res == 1) {
        fprintf(stderr, "Too large text for for the given parameters and image");
    } else if (res == 2) {
        fprintf(stderr, "Could not read the file");
    }

    fclose(input_image_file);
    fclose(output_image_file);
    fclose(text_file);

    free(bitmap_data);
}
