#ifndef BMP_LIB_H_INCLUDED
#define BMP_LIB_H_INCLUDED

#include <stdint.h>
typedef struct {
    uint8_t r, g, b;
} pixel;

#ifdef DEBUG
#define DEBUG_print_pixel(p) __DEBUG_print_pixel(p)
void __DEBUG_print_pixel(pixel);
#else
#define DEBUG_print_pixel(p) ;
#endif // DEBUG
void print_pixel(pixel);

typedef int BMP_error_t;
#define FILE_OPEN_ERROR -1
#define FILE_READ_ERROR -2
#define UNSUPPORTED_IMAGE_FORMAT -3
#define UNSUPPORTED_BMP_FORMAT -4
#define INVALID_BMP_FORMAT -5
#define FILE_WRITE_ERROR -6
#define IMAGE_WRITE_SUCCESS 1
#define IMAGE_READ_SUCCESS 0
BMP_error_t read_image_data(pixel*, const char*);
BMP_error_t write_image_data(pixel*, int32_t, int32_t, const char*);

// KERNEL_SIZE must be an odd positive integer
#define KERNEL_SIZE 5
void blur(pixel*, size_t, size_t);

#endif // BMP_LIB_H_INCLUDED
