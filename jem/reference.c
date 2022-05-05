#include "reference.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

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
  puts("inside fn");
  reference_t *reference = malloc(sizeof(reference_t));
  puts("allocated reference");
  SHA1(bytes, size, *reference);
  return reference;
}

// https://stackoverflow.com/a/53966346
void btox(char *xp, unsigned char *bb, int n) {
  const char xx[]= "0123456789abcdef";
  while (--n >= 0) xp[n] = xx[(bb[n>>1] >> ((1 - (n&1)) << 2)) & 0xF];
}


void xtob(unsigned char*bb, char *xp, int n) {
  // n is number of characters
  int idx = 0;
  for (int i = 0; i < n; i++) {
    unsigned char nibble = xp[i] < 60 ? xp[i] - 48 : xp[i]-87;
    if (i % 2) {
      // set lower 4 bits
      bb[idx] = bb[idx] | nibble;
      idx ++;
    } else {
      // set upper 4 bits and wipe lower 4
      bb[idx] = nibble << 4;
    }
  }
}

reference_t * char_to_reference(char* input) {
  printf("%lu", sizeof(reference_t));
  reference_t * reference = malloc(sizeof(reference_t));
  xtob(reference, input, 40);
  return reference;
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

