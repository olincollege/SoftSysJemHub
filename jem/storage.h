#pragma once
#include "reference.h"
#include "sizedstring.h"

void serialize_reference(unsigned char** buffer, reference_t *reference);

void deserialize_reference(unsigned char** buffer, reference_t *reference);

void serialize_size(unsigned char** buffer, size_t size);

void deserialize_size(unsigned char** buffer, size_t *size);

// returns bytes required to store
size_t sized_string_size(SizedString *string);

// length of string
size_t sized_string_length(SizedString *string);

// free a sized string
void free_sized_string(SizedString *string);

void serialize_sized_string(unsigned char** buffer, SizedString *string);

void deserialize_sized_string(unsigned char** buffer, SizedString *string);

// make a sized string from a c string
SizedString *make_sized_string(char *string);

// save the buffer to disk
reference_t *write_buffer_to_disk(unsigned char** buffer, size_t size);

// read from reference filename into buffer
void read_ref_from_disk(unsigned char** buffer, reference_t *reference);