#include "snaptree.h"
#include "sizedstring.h"
#include "storage.h"
#include <string.h>
#include <stdio.h>

// calculate bytes required to store a snaptree
size_t snaptree_size(SnapTree *tree) {
	size_t size =  sizeof(size_t) + // size field
            sizeof(size_t); // children_count
    for (int i = 0; i < tree->children_length; i++) {
        size += snapshot_size(tree->children[i]);
    }
    return size;
}

size_t snapshot_size(Snapshot *snapshot) {
	return  sizeof(size_t) + // size field
            sized_string_size(snapshot->path) + // Size of the path string
			sizeof(mode_t) + // mode
            sizeof(SnapshotType) + // type 
            sizeof(reference_t); // reference
}

void serialize_snapshot(unsigned char** buffer, Snapshot * shot) {
    serialize_size(buffer, snapshot_size(shot));
    serialize_snapshot_type(buffer, shot->type);
    serialize_sized_string(buffer, shot->path);
    serialize_mode(buffer, shot->mode);
	serialize_reference(buffer, shot->reference);
}

void deserialize_snapshot(unsigned char** buffer, Snapshot * shot) {
    *buffer += sizeof(size_t); // skip size
    SnapshotType type;
    deserialize_snapshot_type(buffer, &type);
    shot->type = type;
    
    SizedString * path = (SizedString *) malloc(sizeof(SizedString));
    deserialize_sized_string(buffer, path);
    shot->path = path;

    deserialize_mode(buffer, &(shot->mode));

    reference_t *reference = malloc(sizeof(reference_t));
    memcpy(reference, *buffer, sizeof(reference_t));
	*buffer += sizeof(reference_t);
    shot->reference = reference;
}

void serialize_snaptree(unsigned char** buffer, SnapTree * tree) {
    serialize_size(buffer, snaptree_size(tree));
    serialize_size(buffer, tree->children_length);
    for(int i = 0; i < tree->children_length; i++) {
        serialize_snapshot(buffer, tree->children[i]);
    }
}

void deserialize_snaptree(unsigned char** buffer, SnapTree * tree) {
    *buffer += sizeof(size_t); // skip size
    deserialize_size(buffer, &tree->children_length);
    tree->children = malloc(tree->children_length * sizeof(Snapshot *));

    for (int i = 0; i < tree->children_length; i++) {
        tree->children[i] = malloc(sizeof(Snapshot));
        deserialize_snapshot(buffer, (tree->children[i]));
    }
}

// free a snapshot
void free_snapshot(Snapshot *shot) {
	// TODO: check if this is making any assumptions about memory layout
	free_sized_string(shot->path);
    free_reference(shot->reference);
    //free(shot);
}

// Get mode of a snapshot
mode_t snap_mode(Snapshot * shot) {
    return shot->mode;
}

// free a snaptree
void free_snaptree(SnapTree *tree) {
    for(int i = 0; i < tree->children_length; i++) {
        free_snapshot(tree->children[i]);
    }
    free(tree);
}

void serialize_snapshot_type(unsigned char** buffer, SnapshotType type) {
	memcpy(*buffer, &type, sizeof(SnapshotType));
	*buffer += sizeof(SnapshotType);
}

void deserialize_snapshot_type(unsigned char** buffer, SnapshotType *type) {
	memcpy(type, *buffer, sizeof(SnapshotType));
	*buffer += sizeof(SnapshotType);
}

void serialize_mode(unsigned char** buffer, mode_t mode) {
	memcpy(*buffer, &mode, sizeof(mode_t));
	*buffer += sizeof(mode_t);
}

void deserialize_mode(unsigned char** buffer, mode_t * mode) {
	memcpy(mode, *buffer, sizeof(mode_t));
	*buffer += sizeof(mode_t);
}

void serialize_family_size(unsigned char ** buffer, int i) {
    memcpy(&i, buffer, sizeof(int));
    *buffer += sizeof(int);
}

void deserialize_family_size(unsigned char ** buffer, int *i) {
    memcpy(i, buffer, sizeof(int));
    *buffer += sizeof(int);
}