#include <stdio.h>
#include "lsb.h"

int extract_text_size(const uint8_t *bitmap_data) {
    uint32_t embedded_blocks_per_byte = 8 / EMBED_BITS_COUNT;
    uint32_t text_file_size = 0;

    for (int i = 0, size = sizeof(uint32_t); i < size; ++i) {

        for (int j = embedded_blocks_per_byte - 1; j >= 0; --j) {
            uint8_t temp = (uint8_t) (bitmap_data[i * embedded_blocks_per_byte + j] & (0xff >> (8 - EMBED_BITS_COUNT)));
            text_file_size |= temp;
            text_file_size <<= EMBED_BITS_COUNT;
        }
    }
    text_file_size >>= EMBED_BITS_COUNT;

    return text_file_size;
}

uint8_t *extract_text(const uint8_t *bitmap_data, int text_file_size) {
    uint8_t *text = (uint8_t *) malloc(sizeof(uint8_t) * text_file_size);

    uint32_t embedded_blocks_per_byte = 8 / EMBED_BITS_COUNT;
    int offset = sizeof(uint32_t);

    for (int i = 0; i < text_file_size; ++i) {
        text[i] = 0;

        for (int j = embedded_blocks_per_byte - 1; j >= 0; --j) {
            uint8_t temp = (uint8_t) (bitmap_data[(i + offset) * embedded_blocks_per_byte + j] &
                                      (0xff >> (8 - EMBED_BITS_COUNT)));
            text[i] |= temp;

            if (j != 0) {
                text[i] <<= EMBED_BITS_COUNT;
            }
        }
    }

    return text;
}

uint8_t *extract_embedded_data(uint8_t *bitmap_data, int *text_file_size) {
    *text_file_size = extract_text_size(bitmap_data);

    return extract_text(bitmap_data, *text_file_size);;
}

void extract_from_bitmap(char *input_image_filename, char *text_file_name) {
    FILE *input_image_file = fopen(input_image_filename, "r");
    if (input_image_file == NULL) {
        fprintf(stderr, "Not valid input image file");
        return;
    }

    FILE *text_file = fopen(text_file_name, "w");
    if (text_file == NULL) {
        fprintf(stderr, "Not valid text file");
        return;
    }

    uint8_t *bitmap_data = load_bitmap(input_image_file, NULL, NULL);

    int text_file_size;
    uint8_t *text = extract_embedded_data(bitmap_data, &text_file_size);

    if (fwrite(text, (size_t) text_file_size, 1, text_file) == 0) {
        fprintf(stderr, "Couldn't write to text file");
        return;
    }

    fclose(input_image_file);
    fclose(text_file);

    free(bitmap_data);
    free(text);
}
