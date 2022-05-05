#include "sizedstring.h"
#include <string.h>
#include <stdio.h>

SizedString *make_sized_string(char *string) {
	size_t len = strlen(string) + 1;
	SizedString *sstring = malloc(sizeof(SizedString));
	char *s = malloc(len);
	memcpy(s, string, len);
	sstring->size = len + sizeof(size_t);
	sstring->string = s;
	return sstring;
}

void serialize_sized_string(unsigned char** buffer, SizedString *string) {
	// most of the time these will be together in memory, but but don't assume that
	size_t length = sized_string_length(string);
	*(size_t *)(*buffer) = string->size; // copy size
	*buffer += sizeof(size_t);
	memcpy(*buffer, string->string, length); // copy string
	*buffer += length;
}

void deserialize_sized_string(unsigned char** buffer, SizedString *string) {
	memcpy(&string->size, *buffer, sizeof(size_t));
	*buffer += sizeof(size_t);
	size_t length = string->size - sizeof(size_t);
	// null terminator is just used when in memory but not saved to disk
	char *data = malloc(length + 1);
	data[length] = '\0';
	memcpy(data, *buffer, length);
	string->string = data;
	*buffer += length;
}

// free a sized string
void free_sized_string(SizedString *string) {
	// TODO: check if this is making any assumptions about memory layout
	free(string->string);
	free(string);
}

// length of string
size_t sized_string_length(SizedString *string) {
	return string->size - sizeof(size_t); // exclude size
}

// returns bytes required to store
size_t sized_string_size(SizedString *string) {
	return string->size;
}