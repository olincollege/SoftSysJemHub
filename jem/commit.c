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
	serialize_size(&position, commit->parents_count);
	for (int i = 0; i < commit->parents_count; i++) {
		serialize_reference(&position, &commit->parents[i]);
	}
	serialize_sized_string(&position, commit->author);
	serialize_sized_string(&position, commit->message);
	serialize_reference(&position, commit->tree);
}

Commit *deserialize_commit(unsigned char ** buffer) {
	Commit * commit = malloc(sizeof(Commit));
	size_t commit_size;
	memcpy(&commit_size, buffer, sizeof(size_t));
	*buffer += sizeof(size_t);
	memcpy(commit->parents_count, buffer, sizeof(size_t));
	*buffer += sizeof(size_t);
	// TODO finish implementing
	return commit;
}

// example:
// size_t size = commit_size(commit);
// unsigned char* buffer = malloc();
// serialize_commit(&buffer, commit);
// make_reference(buffer, size)'
// // open file and write buffer to disk