#include "reference.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void print_reference(reference_t *reference) {
  unsigned char* bytes = (unsigned char *)reference;
  int i = 0;
  for (i=0; i < sizeof(reference_t); i++) {
    printf("%02x", bytes[i] & 0xFF);
  }
  printf("\n");
}

// get a reference to a buffer of bytes
reference_t *make_reference(void *bytes, size_t size) {
  reference_t *reference = malloc(sizeof(reference_t));
  SHA1(bytes, size, *reference);
  return reference;
}

// get a reference to a file
reference_t *make_file_reference(char* filepath) {
  reference_t *reference = malloc(sizeof(reference_t));
  SHA_CTX context;
  SHA1_Init(&context);
  // open file for reading
  int fd = open(filepath, O_RDONLY);
  unsigned char buffer[REFERENCE_READ_BLOCK_SIZE];
  int bytes;
  // TODO: check for errors
  do {
    // read bytes into the buffer and update hash
    bytes = read(fd, buffer, REFERENCE_READ_BLOCK_SIZE);
    SHA1_Update(&context, buffer, bytes);
  } while(bytes > 0);

  // finalize hash
  SHA1_Final(*reference, &context);
  return reference;
}

// free a reference
void free_reference(reference_t *reference) {
  free(reference);
}

