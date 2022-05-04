#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "storage.h"

// helpers for serializing common data types
// not necessarily the most optimized but results in pretty clean code imo
void serialize_reference(unsigned char** buffer, reference_t *reference) {
	memcpy(*buffer, reference, sizeof(reference_t));
	*buffer += sizeof(reference_t);
}

void deserialize_reference(unsigned char** buffer, reference_t *reference) {
	memcpy(&reference, buffer, sizeof(reference_t));
	*buffer += sizeof(reference_t);
}

// returns the reference to the saved data
reference_t *write_buffer_to_disk(unsigned char** buffer, size_t size) {
	reference_t *reference = make_reference(*buffer, size);
	char * filename = reference_to_char(reference);
	// create a buffer big enough for filepath
	char path[45] = ".jem/";
	strncat(path, filename, 40);
	printf("%s\n", path);
	
	FILE *fw = fopen(path, "w");
	fwrite(*buffer, size, 1, fw);
	fclose(fw);
	return reference;
}

// can read size to allocate the buffer
void read_ref_from_disk(unsigned char** buffer, reference_t *reference) {
	char * filename = reference_to_char(reference);
	// create a buffer big enough for filepath
	char path[45] = ".jem/";
	strncat(path, filename, 40);
	FILE *fp = fopen(path, "r");
	if (fp != NULL) {
		size_t size;
		// read the size first
		fread(&size, sizeof(size_t), 1, fp);
		printf("size on disk: %lu\n", size);
		// allocate a buffer
		*buffer = malloc(size);
		// load all the data in
		fseek(fp, 0, SEEK_SET);
    fread(*buffer, sizeof(char), size, fp);
    if ( ferror( fp ) != 0 ) {
        fputs("Error reading file", stderr);
    }
    fclose(fp);
	} else {
		fprintf(stderr, "cannot open input file\n");
	}
}

void serialize_size(unsigned char** buffer, size_t size) {
	//*(size_t *)(*buffer) = size; // copy size
	memcpy(*buffer, &size, sizeof(size_t));
	*buffer += sizeof(size_t);
}

void deserialize_size(unsigned char** buffer, size_t *size) {
	memcpy(size, *buffer, sizeof(size_t));
	*buffer += sizeof(size_t);
}