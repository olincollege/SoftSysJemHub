#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commit.h"
#include <dirent.h>
#include <sys/types.h>
#include "cuserid.h"

// helper function: get the username of the system operator.
char * get_username()
{
  char username[MAX_USERID_LENGTH];
  cuserid(username);
  //printf("%s\n", username);
  char * user = malloc(MAX_USERID_LENGTH);
  strcpy(user, username);
  return user;
}

// calculate bytes required to store a commit
size_t commit_size(Commit *commit) {
	return  sizeof(size_t) + // size
			sizeof(size_t) + // parent counts
			(size_t)(commit->parents_count * sizeof(reference_t)) + // parents
			sized_string_size(commit->author) + // author string
			sized_string_size(commit->message) +// message string
			sizeof(reference_t); // snapshot tree
}

void print_commit(Commit *commit) {
	printf("Author: %s\n", commit->author->string);
	printf("Message: %s\n", commit->message->string);
	printf("Parent Count: %lu\n", commit->parents_count);
	printf("Parents: ");
	for (size_t i = 0; i < commit->parents_count; i++) {
		print_reference(commit->parents[i]);
	}
	printf("Tree: ");
	print_reference(commit->tree);
}

// serialize a commit
void serialize_commit(unsigned char** buffer, Commit *commit) {
	unsigned char* position = *buffer;
	serialize_size(&position, commit_size(commit));
	serialize_size(&position, commit->parents_count);
	for (size_t i = 0; i < commit->parents_count; i++) {
		serialize_reference(&position, commit->parents[i]);
	}
	serialize_sized_string(&position, commit->author);
	serialize_sized_string(&position, commit->message);
	serialize_reference(&position, commit->tree);
}

void deserialize_commit(unsigned char ** buffer, Commit * commit) {
	*buffer += sizeof(size_t); // skip size
	deserialize_size(buffer, &(commit->parents_count));
	for (size_t i = 0; i < commit->parents_count; i++) {
		// TODO: this could use deserialize reference but I couldn't get it to work
		memcpy(&commit->parents[i], buffer, sizeof(reference_t));
		*buffer+=sizeof(reference_t);
	}
	SizedString *author = malloc(sizeof(SizedString));
	deserialize_sized_string(buffer, author);
	commit->author = author;
	SizedString *message = malloc(sizeof(SizedString));
	deserialize_sized_string(buffer, message);
	commit->message = message;
	reference_t *ref = malloc(sizeof(reference_t));
	//deserialize_reference(buffer, ref);
	memcpy(&ref, buffer, sizeof(reference_t));
	*buffer += sizeof(reference_t);
	commit->tree = ref;
}