#include <stdint.h>
#include "utils.h"
uint32_t get_le_i32(uint8_t* src) {
    uint32_t x = 0;
    for(int i = 3; i >= 0; i--) {
        x = x * 0x100 + src[i];
    }
    return x;
}
uint16_t get_le_i16(uint8_t* src) {
    uint16_t x = 0;
    for(int i = 1; i >= 0; i--) {
        x = x * 0x100 + src[i];
    }
    return x;
}

void set_le_i32(uint8_t* dst, uint32_t x) {
    for(int i = 0; i < 4; i++) {
        dst[i] = x % 0x100;
        x /= 0x100;
    }
}

void set_le_i16(uint8_t* dst, uint16_t x) {
    for(int i = 0; i < 2; i++) {
        dst[i] = x % 0x100;
        x /= 0x100;
    }
}

#ifdef DEBUG
void __DEBUG_print_bytes(uint8_t* arr, int sz) {
    DEBUG_PRINT(("["));
    for(int i = 0; i < sz; i++) {
        if(i > 0) DEBUG_PRINT((", "));
        DEBUG_PRINT(("%02X", (uint32_t)arr[i]));
    }
    DEBUG_PRINT(("]\n"));
}
#endif // DEBUG
void print_bytes(uint8_t* arr, int sz) {
    printf("[");
    for(int i = 0; i < sz; i++) {
        if(i > 0) DEBUG_PRINT((", "));
        printf("%02X", (uint32_t)arr[i]);
    }
    printf("]\n");
}

bool in_range(int x, int left, int right) {
    return left <= x && x <= right;
}

#include <limits.h>
#include <errno.h>
stoi_error_t my_stoi(char* str, int* x) {
    errno = 0;
    char* end;
    long i = strtol(str, &end, 10);
    if (*end != '\0')
        return STOI_INVALID_STRING;
    if (errno == ERANGE)
        return STOI_INVALID_STRING;
    if(!(INT_MIN <= i && i <= INT_MAX))
        return STOI_INVALID_STRING;
    *x = i;
    return STOI_SUCCESS;
}

stof_error_t my_stof(char* str, float* x) {
    errno = 0;
    char* end;
    float i = strtod(str, &end);
    if (*end != '\0')
        return STOF_INVALID_STRING;
    if (errno == ERANGE)
        return STOF_INVALID_STRING;
    *x = i;
    return STOF_SUCCESS;
}
