#include <glib-2.0>

typedef struct {
    parents: GArray // This should be the state of HEAD at the type of committing
    author: char * // Person who commits
    message: char * // Commit MSG
    snapshot: GHashTable // Snapshot containing actual information to update the current HEAD + make changes
} branch // commit object