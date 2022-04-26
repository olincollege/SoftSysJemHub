#pragma once

void serialize_reference(unsigned char** buffer, reference_t *reference);

void serialize_size(unsigned char** buffer, size_t size);

// returns bytes required to store
size_t sized_string_size(SizedString *string);

// length of string
size_t sized_string_length(SizedString *string);

// free a sized string
void free_sized_string(SizedString *string);

void serialize_sized_string(unsigned char** buffer, SizedString *string);
// calculate bytes required to store a commit
size_t *commit_size(Commit *commit);

// serialize a commit
void serialize_commit(unsigned char** buffer, Commit *commit);

// make a sized string from a c string
SizedString *make_sized_string(char *string);