#include "storage.h"
#include "reference.h"
#include <sys/stat.h>


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