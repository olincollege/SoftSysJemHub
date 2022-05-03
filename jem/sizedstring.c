#include "sizedstring.h"
#include <string.h>

SizedString *make_sized_string(char *string) {
	size_t size = strlen(string);
	SizedString *sstring = malloc(sizeof(SizedString));
	sstring->size = size;
	sstring->string = string;
	return sstring;
}

void serialize_sized_string(unsigned char** buffer, SizedString *string) {
	// most of the time these will be together in memory, but but don't assume that
	size_t length = sized_string_length(string);
	*(size_t *)(*buffer) = length; // copy size
	memcpy(*buffer+sizeof(size_t), string, length); // copy string
	*buffer += sizeof(size_t) + length;
}

void deserialize_sized_string(unsigned char** buffer, SizedString *string) {
	memcpy(&string->size, buffer, sizeof(size_t));
	*buffer += sizeof(size_t);
	memcpy(&string->string, buffer, string->size);
	*buffer += string->size;
}

// free a sized string
void free_sized_string(SizedString *string) {
	// TODO: check if this is making any assumptions about memory layout
	free(string->string);
	free(string);
}

// length of string
size_t sized_string_length(SizedString *string) {
	return string->size - sizeof(size_t) - 1; // exclude size and \0
}

// returns bytes required to store
size_t sized_string_size(SizedString *string) {
	return string->size;
}