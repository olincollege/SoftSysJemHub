#pragma once
#include <stdlib.h>

// life is easier when we know the size of things
typedef struct {
  size_t size;
  char* string;
} SizedString;


SizedString *make_sized_string(char *string);

void serialize_sized_string(unsigned char** buffer, SizedString *string);

void deserialize_sized_string(unsigned char** buffer, SizedString *string);

// free a sized string
void free_sized_string(SizedString *string);

// length of string
size_t sized_string_length(SizedString *string);

// returns bytes required to store
size_t sized_string_size(SizedString *string);