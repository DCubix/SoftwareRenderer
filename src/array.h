#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>

typedef struct p3d_array {
	int length, capacity;
	uint32_t data_size;
	uint8_t* data;
} Array;

void array_new_s(Array* arr, uint32_t data_size);
void array_free(Array* arr);

void array_add_ptr(Array* arr, uint8_t* data);
void array_remove(Array* arr, int i);
uint8_t* array_get_ptr(Array* arr, int i);
void array_set(Array* arr, int i, uint8_t* data);
void array_clear(Array* arr);

void __array_resize_if_full(Array* arr);

#define array_new(arr, T) array_new_s(arr, sizeof(T));
#define array_add(arr, item) array_add_ptr(arr, (uint8_t*)item);
#define array_get(arr, i, T) (*((T*)array_get_ptr(arr, i)));

#endif // ARRAY_H
