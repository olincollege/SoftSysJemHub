#pragma once
#include "reference.h"
#include "storage.h"
#include <sys/stat.h>

typedef enum {
    SST_DIR,
    SST_FILE
} SnapshotType;

typedef struct {
    SnapshotType type;
    SizedString *path;
    mode_t mode;
    // reference can be used to find stored copy of file
    reference_t *reference;
} Snapshot;

// difference is children is a list
typedef struct {
    size_t children_length;
    Snapshot **children; // list of references to snapshots or snap trees
} SnapTree;

// Calculate the size of a snaptree, in bytes 
size_t snaptree_size(SnapTree *tree);

// Calculate the size of a snapshot, in bytes 
size_t snapshot_size(Snapshot *snapshot);

// Serialize a snapshot
void serialize_snapshot_type(unsigned char** buffer, SnapshotType type);

// Deserialize a snapshot
void deserialize_snapshot_type(unsigned char** buffer, SnapshotType * type);

// Serialize a snapshot
void serialize_snapshot(unsigned char** buffer, Snapshot * shot);

// Deserialize a snapshot
void deserialize_snapshot(unsigned char** buffer, Snapshot * shot);

// Serialize a snap tree
void serialize_snaptree(unsigned char** buffer, SnapTree * tree);

// Deserialize a snap tree
void deserialize_snaptree(unsigned char** buffer, SnapTree * tree);

// Free the allocated memory for a snapshot
void free_snapshot(Snapshot *shot);

// Free the allocated memory for a snap tree
void free_snaptree(SnapTree *tree);

// Serialize the mode, helper for serializing snapshots and trees
void serialize_mode(unsigned char** buffer, mode_t mode);

// Deserialize the mode, helper for deserializing snapshots and trees
void deserialize_mode(unsigned char** buffer, mode_t * mode);

// Serialize a counter for the number of children in a snap tree, used before serializing children
void serialize_family_size(unsigned char ** buffer, int i);

// Deserialize a counter for the number of children in a snap tree, used before deserializing children
void deserialize_family_size(unsigned char ** buffer, int *i);