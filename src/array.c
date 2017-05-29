#include "array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

void array_new_s(Array* arr, uint32_t data_size) {
	arr->capacity = 100;
	arr->data_size = data_size;
	arr->length = 0;
	arr->data = (uint8_t*) malloc(arr->capacity * arr->data_size);
	memset(arr->data, '\0', arr->capacity * arr->data_size);
}

void array_free(Array* arr) {
	free(arr->data);
}

void array_add_ptr(Array* arr, uint8_t* data) {
	__array_resize_if_full(arr);
	memcpy(arr->data + arr->length * arr->data_size, data, arr->data_size);
	arr->length++;
}

void __array_resize_if_full(Array* arr) {
	if (arr->length >= arr->capacity) {
		arr->capacity *= 2;
		arr->data = (uint8_t*) realloc(arr->data, arr->capacity * arr->data_size);
	}
}

uint8_t* array_get_ptr(Array* arr, int i) {
	return arr->data + i * arr->data_size;
}

void array_set(Array* arr, int i, uint8_t* data) {
	if (i >= arr->length) {
		array_add(arr, data);
	} else {
		memcpy(arr->data + i * arr->data_size, data, arr->data_size);
	}
}

void array_remove(Array* arr, int i) {
	memmove(arr->data + i * arr->data_size, arr->data + (i+1) * arr->data_size, arr->data_size);
}

void array_clear(Array* arr) {
	arr->length = 0;
}
