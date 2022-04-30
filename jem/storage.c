#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "storage.h"

// helpers for serializing common data types
// not necessarily the most optimized but results in pretty clean code imo
// TODO: maybe refactor out into small files for each type that have the definition
// and code to serialize/deserialize etc so that one doesn't get changed without the other
void serialize_reference(unsigned char** buffer, reference_t *reference) {
	memcpy(*buffer, reference, sizeof(reference_t));
	*buffer += sizeof(reference_t);
}

void serialize_size(unsigned char** buffer, size_t size) {
	*(size_t *)(*buffer) = size; // copy size
	*buffer += sizeof(size_t);
}

// returns bytes required to store
size_t sized_string_size(SizedString *string) {
	return string->size;
}

// length of string
size_t sized_string_length(SizedString *string) {
	return string->size - sizeof(size_t) - 1; // exclude size and \0
}

// free a sized string
void free_sized_string(SizedString *string) {
	// TODO: check if this is making any assumptions about memory layout
	free(string->string);
	free(string);
}

void serialize_sized_string(unsigned char** buffer, SizedString *string) {
	// most of the time these will be together in memory, but but don't assume that
	size_t length = sized_string_length(string);
	*(size_t *)(*buffer) = length; // copy size
	memcpy(*buffer+sizeof(size_t), string, length); // copy string
	*buffer += sizeof(size_t) + length;
}
// in some cases not using reference_t pointers would let us get away with a
// single memcopy, but it would not work for lists and overall be a headache

// all data structures are stored with their size to make creating buffers easy
// except when required, the size is discarded when they are loaded

SizedString *make_sized_string(char *string) {
	size_t size = strlen(string);
	SizedString *sstring = malloc(sizeof(SizedString));
	sstring->size = size;
	sstring->string = &string;
	return sstring;
}

// example:
// size_t size = commit_size(commit);
// unsigned char* buffer = malloc();
// serialize_commit(&buffer, commit);
// make_reference(buffer, size)'
// // open file and write buffer to disk