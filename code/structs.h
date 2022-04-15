typedef struct graphnode {
    struct graphnode * descendants;
} GraphNode;

typedef struct {
    FILE ** snap; // this type might change, right now I was thinking an array of file pointers
    struct TreeNode * descendants;
} TreeNode;

typedef struct {
    TreeNode tree_head;
} SnapTree;

typedef struct {
    GraphNode * parents; // This should be the state of HEAD at the type of committing
    char * author; // Person who commits
    char * message; // Commit MSG
    SnapTree snapshots; // Snapshot containing actual information to update the current HEAD + make changes
} commit_t; // commit object


