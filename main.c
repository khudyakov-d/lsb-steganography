#include <stdio.h>


int main(int argc, char **argv) {

    if (strcmp(argv[1], "embed") == 0) {
        if (argc != 5) {
            fprintf(stderr, "For embed enter params to the input of the program: " \
                        "bmp picture filename \n" \
                        "text filename \n" \
                        "result bmp file name");
            return 1;
        } else {
            embed_in_bitmap(argv[2], argv[3], argv[4]);
        }
    } else if (strcmp(argv[1], "extract") == 0) {
        if (argc != 4) {
            fprintf(stderr, "For extract enter params to the input of the program: " \
                        "bmp picture filename \n" \
                        "result text filename \n");
            return 1;
        } else {
            extract_from_bitmap(argv[2], argv[3]);
        }
    } else {
        fprintf(stderr, "Unknown command %s", argv[1]);
    }

    return 0;
}