#pragma once
#include "reference.h"
#include "storage.h"
#include <sys/stat.h>


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