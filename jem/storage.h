#pragma once
#include "reference.h"
#include "sizedstring.h"

void serialize_reference(unsigned char** buffer, reference_t *reference);

void deserialize_reference(unsigned char** buffer, reference_t *reference);

void serialize_size(unsigned char** buffer, size_t size);

void deserialize_size(unsigned char** buffer, size_t *size);

void copy_file_to_jem(char * src_filepath, reference_t *file_ref);

// save the buffer to disk
reference_t *write_buffer_to_disk(unsigned char** buffer, size_t size);

// read from reference filename into buffer
void read_ref_from_disk(unsigned char** buffer, reference_t *reference);