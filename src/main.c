#include "stdlib.h"
#include "stdio.h"
#include "png.h"
#include "setjmp.h"
#include "stdio.h"
#include "pngconf.h"

extern int multiply(int, int);

int min(int a, int b) {
    if (a < b) return a;
    return b;
}

int max(int a, int b) {
    if (a < b) return b;
    return a;
}

void apply_darken_filter(png_bytepp row_data, int width, int height, int depth) {
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            for (int i = 0; i < 3; i++) {
                row_data[r][c * 4 + i] = row_data[r][c * 4 + i] >> 1;
            }
        }
    }
}

void apply_filter(png_bytepp in, png_bytepp out, int width, int height, int depth) {
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            for (int i = 0; i < 3; i++) {
                int src = 5 * in[r][c * 4 + i];
                src -= in[max(r-1, 0)][c * 4 + i];
                src -= in[r][max(c-1, 0) * 4 + i];
                src -= in[min(r+1, height-1)][c * 4 + i];
                src -= in[r][min(c+1, width-1) * 4 + i];
                src = max(min(src, 255), 0);
                out[r][c * 4 + i] = (unsigned char)src;
            }
        }
    }
}

int write_png(png_bytepp row_data, int width, int height) {
    FILE *fp = fopen("out.png", "wb");
    if (!fp) {
        fprintf(stderr, "Failed opening write file\n");
        return 1;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "Failed creating write png struct\n");
        fclose(fp);
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr, "Failed creating write png info\n");
        fclose(fp);
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {  
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return 1;
    }

    png_set_compression_level(png_ptr, 6);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_init_io(png_ptr, fp);
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, row_data);
    png_write_end(png_ptr, NULL);

    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid argument count\n");
        return 1;
    }

    png_structp png_ptr;
    png_infop info_ptr;
    png_infop end_ptr;

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        return 1;
    }


    unsigned char sig[8];
    fread(sig, sizeof(unsigned char), 8, fp);
    if (!png_check_sig(sig, 8)) {
        fprintf(stderr, "Invalid PNG file\n");
        return 1;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        return 1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "libpng error\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 1;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    fprintf(stdout, "Color Type: %d\n", color_type);
    fprintf(stdout, "Bit Depth: %d\n", bit_depth);
    fprintf(stdout, "Image size: %d x %d\n", width, height);

    png_bytepp row_ptrs = (png_bytepp)malloc(sizeof(png_bytep) * height);
    for (int i = 0; i < height; i++) {
        row_ptrs[i] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }

    png_read_image(png_ptr, row_ptrs);

    apply_filter(row_ptrs, row_ptrs, width, height, 8);

    write_png(row_ptrs, width, height);

    for (int i = 0;i < height; i++) {
        free(row_ptrs[i]);
    }
    free(row_ptrs);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    fclose(fp);
    return 0;
}