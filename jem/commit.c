#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commit.h"

// calculate bytes required to store a commit
size_t commit_size(Commit *commit) {
	return  sizeof(size_t) + // size
			(size_t)(commit->parents_count * sizeof(reference_t)) + // parents
			sized_string_size(commit->author) + sizeof(size_t) + // author string
			sized_string_size(commit->message) + sizeof(size_t) +// message string
			sizeof(reference_t); // snapshot tree
}

// serialize a commit
void serialize_commit(unsigned char** buffer, Commit *commit) {
	unsigned char* position = *buffer;
	serialize_size(&position, commit_size(commit));
	printf("%lu\n", commit_size(commit));
	serialize_size(&position, commit->parents_count);
	printf("%lu\n", commit->parents_count);
	for (size_t i = 0; i < commit->parents_count; i++) {
		serialize_reference(&position, commit->parents[i]);
		print_reference(commit->parents[i]);
	}
	serialize_sized_string(&position, commit->author);
	printf("%lu\n", commit->author->size);
	serialize_sized_string(&position, commit->message);
	printf("%lu\n", commit->message->size);
	serialize_reference(&position, commit->tree);
	print_reference(commit->tree);
}

void deserialize_commit(unsigned char ** buffer, Commit * commit) {
	unsigned char* position = *buffer;
	size_t commit_size;
	deserialize_size(&position, &commit_size);
	printf("%lu\n", commit_size);
	deserialize_size(&position, &commit->parents_count);
	printf("%lu\n", commit->parents_count);
	for (size_t i = 0; i < commit->parents_count; i++) {
		// TODO this could use deserialize reference but I couldn't get it to work
		memcpy(&commit->parents[i], &position, sizeof(reference_t));
		print_reference(commit->parents[i]);
		position+=sizeof(reference_t);
	}
	SizedString *author = malloc(sizeof(SizedString));
	deserialize_sized_string(&position, author);
	commit->author = author;
	printf("%lu\n", author->size);
	printf("%s\n", author->string);
	SizedString *message = malloc(sizeof(SizedString));
	deserialize_sized_string(&position, message);
	commit->message = message;
	printf("%lu\n", message->size);
	printf("%s\n", message->string);
	//deserialize_reference()
	memcpy(commit->tree, &position, sizeof(reference_t));
	print_reference(commit->tree);
}

// example:
// size_t size = commit_size(commit);
// unsigned char* buffer = malloc();
// serialize_commit(&buffer, commit);
// make_reference(buffer, size)'
// // open file and write buffer to disk