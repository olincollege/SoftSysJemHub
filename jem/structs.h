#pragma once
#include <sys/stat.h>
#include "reference.h"

// life is easier when we know the size of things
typedef struct {
  size_t size;
  char* string;
} SizedString;

typedef struct {
    mode_t mode;
    long ctime;
    long mtime;
    reference_t *reference;
    SizedString *path;
} IndexEntry;

typedef struct {
    size_t file_count; 
    // rare case where we don't use references,
    // beause these are temporary they shouldn't be saved elsewhere
    IndexEntry *entries; // files added to index
} Index;

// make sure to mark which are trees and which are not
typedef struct {
    SizedString path;
    mode_t mode;
    // reference can be used to find stored copy of file
    reference_t *reference;
} Snapshot;

// difference is children is a list
typedef struct {
    SizedString path; // where the snapshot is (directory or file)
    mode_t mode;
    // TODO: this needs to be sorted for creating tree from index
    reference_t **children; // list of references to snapshots or snap trees
} SnapTree;

typedef struct {
    // 1 commit unless a merge, in which case it will be 2
    size_t parents_count;
    reference_t *parents; // Normally this should be the state of HEAD at the type of committing
    SizedString *author; // Person who commits
    SizedString *message; // Commit MSG
    reference_t *tree; // Snapshot containing actual information to update the current HEAD + make changes
} Commit; // commit object


