#include "snaptree.h"
#include "sizedstring.h"
#include "storage.h"
#include <string.h>
#include <stdio.h>


// calculate bytes required to store a snaptree
size_t snaptree_size(SnapTree *tree) {
	return  sizeof(size_t) + // size field
            sized_string_size(tree->path) + // Size of the path string
			sizeof(tree->mode) + // Size of the mode of the tree, in bytes
            sizeof(size_t) + // children_count
			tree->children_length * sizeof(reference_t); // snapshot tree
}

size_t snapshot_size(Snapshot *snapshot) {
	return  sizeof(size_t) + // the tree itself counts i think?
            sized_string_size(snapshot->path) + // Size of the path string
			sizeof(snapshot->mode) + // Size of the mode of the tree, in bytes
			sizeof(reference_t); // snapshot tree
}

void serialize_snapshot(unsigned char** buffer, Snapshot * shot) {
    serialize_sized_string(buffer, shot->path);
    serialize_mode(buffer, &(shot->mode)); // at the bottom of this file; the same as serialize_reference with mode_t
	serialize_reference(buffer, shot->reference);
}

void deserialize_snapshot(unsigned char** buffer, Snapshot * shot) {

    SizedString * path = (SizedString *) malloc(sizeof(SizedString));
    deserialize_sized_string(buffer, path);
    shot->path = path;

    mode_t mode;
    deserialize_mode(buffer, &mode);
    shot-> mode = mode;

    reference_t * reference = (reference_t *) malloc(sizeof(reference_t));
    deserialize_reference(buffer, reference);
    shot->reference = reference;
}

void serialize_snaptree(unsigned char** buffer, SnapTree * tree) {
    serialize_sized_string(buffer, tree->path);
    serialize_mode(buffer, &(tree->mode)); // at the bottom of this file; the same as serialize_reference with mode_t
	int i = 0;
    while (tree->children[i] != NULL) {
        i++; // Get number of children in the tree
    }
    serialize_family_size(buffer, i);
    for (int j=0; j < i; j++) {
        serialize_snapshot(buffer, tree->children[j]);
    }
}

void deserialize_snaptree(unsigned char** buffer, SnapTree * tree) {
    SizedString * path = (SizedString *) malloc(sizeof(SizedString));
    deserialize_sized_string(buffer, path);
    printf("%zu\n", path->size);
	printf("%s\n", path->string);
    tree->path = path;

    mode_t mode;
    deserialize_mode(buffer, &mode);
    printf("mode: %i\n" , mode);
    tree-> mode = mode;

    int i;
    deserialize_family_size(buffer, i); // Get the number of children the tree has
    printf("Tree has %i children\n" , i);

    Snapshot ** children; // Array of [pointers to] Snapshots
    for (int j = 0; j < i; j++) {
        Snapshot * child = (Snapshot *) malloc(sizeof(Snapshot));
        deserialize_snapshot(buffer, child);
        printf("Child deserialized\n");
        children[j] = child;
    }
}

// free a snapshot
void free_snapshot(Snapshot *shot) {
	// TODO: check if this is making any assumptions about memory layout
	free_sized_string(shot->path);
    free_reference(shot->reference);
}

// Get mode of a snapshot
mode_t snap_mode(Snapshot * shot) {
    return shot->mode;
}

// free a snaptree
void free_snaptree(SnapTree *tree) {
    free_sized_string(tree->path);
    int i = 0;
    while (tree->children[i] != NULL) {
        free_snapshot(tree->children[i]);
        i++;
    }
}

void serialize_mode(unsigned char** buffer, mode_t * mode) {
	memcpy(*buffer, mode, sizeof(mode_t));
	*buffer += sizeof(mode_t);
}

void deserialize_mode(unsigned char** buffer, mode_t * mode) {
	memcpy(mode, buffer, sizeof(mode_t));
	*buffer += sizeof(mode_t);
}

void serialize_family_size(unsigned char ** buffer, int i) {
    memcpy(&i, buffer, sizeof(int));
    *buffer += sizeof(int);
}

void deserialize_family_size(unsigned char ** buffer, int i) {
    memcpy(&i, buffer, sizeof(int));
    *buffer += sizeof(int);
}