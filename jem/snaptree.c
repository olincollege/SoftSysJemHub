#include "snaptree.h"
#include "sizedstring.h"
#include <string.h>


void serialize_snaptree(unsigned char** buffer, SnapTree * tree) {
	// most of the time these will be together in memory, but but don't assume that
	size_t length = sized_string_length(tree);
	*(size_t *)(*buffer) = length; // copy size
	memcpy(*buffer+sizeof(size_t), tree, length); // copy string
	*buffer += sizeof(size_t) + length;
}

// free a snapshot
void free_snapshot(Snapshot *shot) {
	// TODO: check if this is making any assumptions about memory layout
	free_sized_string(&(shot->path));
}

// Get mode of a snapshot
mode_t snap_mode(Snapshot * shot) {
    return shot->mode;
}

// free a snaptree
void free_snaptree(SnapTree *tree) {
    free_snapshot(&(tree->path));
    int i = 0;
    while (tree->children[i] != NULL) {
        free_snapshot(tree->children[i]);
        i++;
    }
}
