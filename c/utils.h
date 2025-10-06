#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdio.h>
#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) ;
#endif

#include <stdlib.h>
#define malloc_matrix(rows, cols, type) malloc(sizeof(type[rows][cols]));
#define calloc_matrix(rows, cols, type) calloc(rows * cols, sizeof(type));

uint32_t get_le_i32(uint8_t*);
uint16_t get_le_i16(uint8_t*);

void set_le_i32(uint8_t*, uint32_t);
void set_le_i16(uint8_t*, uint16_t);

#ifdef DEBUG
#define DEBUG_print_bytes(arr, sz) __DEBUG_print_bytes(arr, sz)
void __DEBUG_print_bytes(uint8_t*, int);
#else
#define DEBUG_print_bytes(arr, sz) ;
#endif // DEBUG
void print_bytes(uint8_t*, int);

#include <stdbool.h>
bool in_range(int, int, int);

typedef int stoi_error_t;
#define STOI_INVALID_STRING -1
#define STOI_SUCCESS 0
stoi_error_t my_stoi(char*, int*);

typedef int stof_error_t;
#define STOF_INVALID_STRING -1
#define STOF_SUCCESS 0
stof_error_t my_stof(char*, float*);

#define RAD_TO_DEG(r) (r * (180.f / M_PI))
#define DEG_TO_RAD(r) (r / (180.f / M_PI))

#endif // UTILS_H_INCLUDED
