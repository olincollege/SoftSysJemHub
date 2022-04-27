#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
	unsigned char* position = buffer;
	serialize_size(&position, commit_size(commit));
	serialize_size(&position, commit->parents_count);
	for (int i = 0; i < commit->parents_count; i++) {
		serialize_reference(&position, commit->parents[i]);
	}
	serialize_sized_string(&position, commit->author);
	serialize_sized_string(&position, commit->message);
	serialize_reference(&position, commit->tree);
}

// example:
// size_t size = commit_size(commit);
// unsigned char* buffer = malloc();
// serialize_commit(&buffer, commit);
// make_reference(buffer, size)'
// // open file and write buffer to disk