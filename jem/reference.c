#include "reference.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

reference_t * char_to_reference(char* input) {
    reference_t * reference = malloc(2*sizeof(reference_t));
    int len = strlen(input);
    for (int i = 0 ; i < len ; i ++) {
      *reference[i] = input[i];
      printf("input[i] : %c     ", input[i]);
      printf("reference[i] : %c\n", reference[i]);
    }
    return reference;
}

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

// https://stackoverflow.com/a/53966346
void btox(char *xp, unsigned char *bb, int n) {
    const char xx[]= "0123456789abcdef";
    while (--n >= 0) xp[n] = xx[(bb[n>>1] >> ((1 - (n&1)) << 2)) & 0xF];
}

char * reference_to_char(reference_t * reference) {
  char *string = malloc(40 + 1);
  btox(string, (unsigned char *)reference, 40);
  string[40] = '\0';
  return string;
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

