#pragma once
#include <openssl/sha.h>

// number of bytes to read at a time
#define REFERENCE_READ_BLOCK_SIZE 1<<16

// sha1 hash
typedef struct {
    unsigned char reference[SHA_DIGEST_LENGTH];
    char type[20];
} reference_t; 

// print a hash
void print_reference(reference_t *reference);

// make a reference to some bytes
reference_t *make_reference(void *bytes, size_t size);

// make a reference to a file
reference_t *make_file_reference(char* filepath);

// free a reference
void free_reference(reference_t *reference);

// convert reference to hex string
char * reference_to_char(reference_t *reference);