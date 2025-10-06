#include <stdlib.h>
#include "bmp_lib.h"
#include "utils.h"

#ifdef DEBUG
void __DEBUG_print_pixel(pixel p) {
    DEBUG_PRINT(("(%02X %02X %02X)", (uint32_t)p.r, (uint32_t)p.g, (uint32_t)p.b));
}
#endif // DEBUG
void print_pixel(pixel p) {
    printf("(%02X %02X %02X)", (uint32_t)p.r, (uint32_t)p.g, (uint32_t)p.b);
}

#define BMP_HEADER_SIZE 14
#define DIB_HEADER_SIZE 40
#define nearest4(x) ((x % 4 == 0) ? x : x/4*4+4)

// Supports 24bpp BMPs
BMP_error_t read_image_data(pixel *image_data, const char* file_name) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL)
        return FILE_OPEN_ERROR;

    uint8_t header[BMP_HEADER_SIZE];
    if(fread(header, sizeof(header[0]), BMP_HEADER_SIZE, file) != BMP_HEADER_SIZE) // Reading the header
        return FILE_READ_ERROR;
    DEBUG_print_bytes(header, BMP_HEADER_SIZE);
    if(get_le_i16(&header[0]) != 0x4D42) // All BMPs must start with 0x4D42 (little endian)
        return UNSUPPORTED_IMAGE_FORMAT;

    uint8_t DIB_header[DIB_HEADER_SIZE];
    if(fread(DIB_header, sizeof(DIB_header[0]), DIB_HEADER_SIZE, file) != DIB_HEADER_SIZE) // Reading the DIB header
        return FILE_READ_ERROR;
    DEBUG_print_bytes(DIB_header, DIB_HEADER_SIZE);
    if(get_le_i32(&DIB_header[0]) != DIB_HEADER_SIZE)
        return UNSUPPORTED_BMP_FORMAT;

    DEBUG_PRINT(("Getting width and height...\n"));
    uint32_t width = get_le_i32(&DIB_header[4]);
    uint32_t height = get_le_i32(&DIB_header[8]);
    pixel (*image_matrix)[width] = (pixel (*)[width])(image_data);

    uint16_t color_planes = get_le_i16(&DIB_header[12]);
    if(color_planes != 1)
        return INVALID_BMP_FORMAT;
    uint16_t bpp = get_le_i16(&DIB_header[14]);
    if(bpp != 24)
        return UNSUPPORTED_BMP_FORMAT;
    uint32_t compression = get_le_i32(&DIB_header[16]);
    if(compression != 0)
        return UNSUPPORTED_BMP_FORMAT;

    DEBUG_PRINT(("Getting pixel array offset...\n"));
    uint32_t pixel_array_offset = get_le_i32(&header[10]);
    size_t row_size_on_disk = nearest4(width * 3); // 3 bytes per pixel, padded to nearest 4
    DEBUG_PRINT(("Reading pixel matrix...%llu\n", row_size_on_disk));
    uint8_t (*pixel_matrix)[row_size_on_disk] = malloc_matrix(height, row_size_on_disk, uint8_t);

    if(fseek(file, pixel_array_offset, SEEK_SET) != 0)
        return FILE_READ_ERROR;
    if(fread(pixel_matrix, sizeof(uint8_t), height * row_size_on_disk, file) != height * row_size_on_disk)
        return FILE_READ_ERROR;

    // pixel data is stored from the bottom up, left to right
    for(int i = height - 1, row = 0; i >= 0; i--, row++) {
        int pixel = 0, channel = 0;
        for(int j = 0; j < 3 * width; j++) {
            switch (channel) {
            case 0: // file is in BGR format (reverse RGB)
                image_matrix[row][pixel].b = pixel_matrix[i][j];
                channel = 1;
                break;
            case 1:
                image_matrix[row][pixel].g = pixel_matrix[i][j];
                channel = 2;
                break;
            case 2:
                image_matrix[row][pixel].r = pixel_matrix[i][j];
                channel = 0;
                pixel++;
                break;
            }
        }
    }

    free(pixel_matrix);
    fclose(file);

    return IMAGE_READ_SUCCESS;
}

// Supports 24bpp BMPs
BMP_error_t write_image_data(pixel *image_data, int32_t height, int32_t width, const char* file_name) {
    pixel (*image_matrix)[width] = (pixel (*)[width])image_data;

    size_t row_size_on_disk = nearest4(width * 3); // 3 bytes per pixel, padded to nearest 4
    size_t size_of_array = row_size_on_disk * height;

    // BMP Header - 14 bytes
    uint8_t header[BMP_HEADER_SIZE];
    set_le_i16(&header[ 0], 0x4D42); // all BMP files begin with 0x4D42 (little endian)
    set_le_i32(&header[ 2], size_of_array + BMP_HEADER_SIZE + DIB_HEADER_SIZE); // size of file = size of pixel array + size of headers
    set_le_i16(&header[ 6],      0); // unused value
    set_le_i16(&header[ 8],      0); // unused value
    set_le_i32(&header[10], BMP_HEADER_SIZE + DIB_HEADER_SIZE); // beginning of pixel array

    uint8_t DIB_header[DIB_HEADER_SIZE];
    set_le_i32(&DIB_header[ 0],     40); // We are using the standard BITMAPINFOHEADER, so the size is 40 bytes
    set_le_i32(&DIB_header[ 4],  width); // width of image in px
    set_le_i32(&DIB_header[ 8], height); // height of image in px
    set_le_i16(&DIB_header[12],      1); // number of color planes; always 1
    set_le_i16(&DIB_header[14],     24); // Bits per pixel
    set_le_i32(&DIB_header[16],      0); // no compression
    set_le_i32(&DIB_header[20], size_of_array); // size of raw bitmap data
    set_le_i32(&DIB_header[24], 0x0EC4); // horizontal resolution (in px/m), dummy value i got from paint
    set_le_i32(&DIB_header[28], 0x0EC4); // vertucal resolution (in px/m), dummy value i got from paint
    set_le_i32(&DIB_header[32],      0); // number of colors in color palette; we aren't using one, so 0 for defalt
    set_le_i32(&DIB_header[36],      0); // number of "important colors"; 0 for default

    FILE* file = fopen(file_name, "wb");
    if (file == NULL)
        return FILE_OPEN_ERROR;

    if(fwrite(header, sizeof(header[0]), BMP_HEADER_SIZE, file) != BMP_HEADER_SIZE)
        return FILE_WRITE_ERROR;
    if(fwrite(DIB_header, sizeof(DIB_header[0]), DIB_HEADER_SIZE, file) != DIB_HEADER_SIZE)
        return FILE_WRITE_ERROR;

    for(int i = height - 1; i >= 0; i--) {
        for(int j = 0; j < width; j++) {
            if(fwrite(&image_matrix[i][j].b, sizeof(image_matrix[i][j].r), 1, file) != 1)
                return FILE_WRITE_ERROR;
            if(fwrite(&image_matrix[i][j].g, sizeof(image_matrix[i][j].r), 1, file) != 1)
                return FILE_WRITE_ERROR;
            if(fwrite(&image_matrix[i][j].r, sizeof(image_matrix[i][j].r), 1, file) != 1)
                return FILE_WRITE_ERROR;
        }

        // padding
        for(int j = width * 3; j < row_size_on_disk; j++) {
            if(fwrite("\0", sizeof(char), 1, file) != 1)
                return FILE_WRITE_ERROR;
        }
    }

    fclose(file);

    return IMAGE_WRITE_SUCCESS;
}


const int blur_kernel[][KERNEL_SIZE] = {
     {  1,  4,  6,  4,  1 },
     {  4, 16, 24, 16,  4 },
     {  6, 24, 36, 24,  6 },
     {  4, 16, 24, 16,  4 },
     {  1,  4,  6,  4,  1 }
}; // got these values from here https://computergraphics.stackexchange.com/questions/39/how-is-gaussian-blur-implemented
#include <math.h>
void blur(pixel* image_data, size_t height, size_t width) {
    pixel (*image_matrix)[width] = (pixel (*)[width])image_data;

    pixel (*new_image)[width] = malloc_matrix(height, width, pixel);

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            int cnt = 0;
            int sum = 0;
            int newr = 0, newg = 0, newb = 0;
            for(int i0 = 0; i0 < KERNEL_SIZE; i0++) {
                for(int j0 = 0; j0 < KERNEL_SIZE; j0++) {
                    int i1 = i + i0 - KERNEL_SIZE/2;
                    int j1 = j + j0 - KERNEL_SIZE/2;
                    if(in_range(i1, 0, height - 1) && in_range(j1, 0, width - 1)) {
                        cnt++;
                        sum += blur_kernel[i0][j0];
                        newr += blur_kernel[i0][j0] * (int)image_matrix[i1][j1].r;
                        newg += blur_kernel[i0][j0] * (int)image_matrix[i1][j1].g;
                        newb += blur_kernel[i0][j0] * (int)image_matrix[i1][j1].b;
                    }
                }
            }
            newr = round((float)newr / (float)sum);
            newg = round((float)newg / (float)sum);
            newb = round((float)newb / (float)sum);
            new_image[i][j] = (pixel){ newr, newg, newb };
        }
    }

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            image_matrix[i][j] = new_image[i][j];
        }
    }

    //memcpy(image_matrix, new_image, sizeof(new_image[0]) * width * height);

    free(new_image);
}
