#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commit.h"

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
	printf("Parent Count: %d\n", commit->parents_count);
	printf("Parents: ");
	int i = 0;
	while (commit->parents[i] != NULL){
		print_reference(commit->parents[i]);
		i++;
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
		print_reference(commit->parents[i]);
	}
	serialize_sized_string(&position, commit->author);
	serialize_sized_string(&position, commit->message);
	serialize_reference(&position, commit->tree);
}

void deserialize_commit(unsigned char ** buffer, Commit * commit) {
	puts("deserialize");
	size_t commit_size;
	deserialize_size(buffer, &commit_size);
	size_t parents_count;
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
	//deserialize_reference()
	memcpy(&commit->tree, buffer, sizeof(reference_t));
}

// example:
// size_t size = commit_size(commit);
// unsigned char* buffer = malloc();
// serialize_commit(&buffer, commit);
// make_reference(buffer, size)'
// // open file and write buffer to disk