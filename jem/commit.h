#pragma once
#include <sys/stat.h>
#include "reference.h"
#include "storage.h"

typedef struct {
    // 1 commit unless a merge, in which case it will be 2
    size_t parents_count;
    reference_t *parents[2]; // Normally this should be the state of HEAD at the type of committing
    SizedString *author; // Person who commits
    SizedString *message; // Commit MSG
    reference_t *tree; // Snapshot containing actual information to update the current HEAD + make changes
} Commit; // commit object

// create a commit from the current working tree
//Commit * create_commit(char * message);

// calculate bytes required to store a commit
size_t commit_size(Commit *commit);

// serialize a commit
void serialize_commit(unsigned char** buffer, Commit *commit);

// deserialize a commit
void deserialize_commit(unsigned char ** buffer, Commit *commit);

void print_commit(Commit *commit);