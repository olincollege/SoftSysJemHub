#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <glib.h>
#include <sys/types.h>
#include <unistd.h>
#include "reference.h"
#include "storage.h"
#include "commit.h"
#include "index.h"
#include "snaptree.h"

// TODO:
/*
git add :
- Add file from working tree to index/staging area
- The index is essentially a list of files/paths

git commit :
- Create a branch object to be committed from the index to the repository
- ONLY objects listed in the index can be committed
- Update HEAD to be the latest commit
- 

git checkout : 
- create identifiers for every commit and store them on disk.
- traverse the repository to find the snapshot corresponding to the identifier the user wants checked out
- set this to the current HEAD (detached HEAD)
- reset file to this status
*/

void error(char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

int check_dir(){
    DIR* dir = opendir("~/.jem");
    if (dir) {
        /* Directory exists. */
        closedir(dir);
        return 1;
    } else if (ENOENT == errno) {
        printf("Directory does not exist");
        /* Directory does not exist. */
    } else {
        error("opendir failed");
        /* opendir() failed for some other reason. */
    }
    return 0;
}


char ** get_test_files() {
    // Returns list of strings with test filenames
    // Index 0 and 1 reserved for filename and command
    char** test_files = malloc(4 * sizeof(char*));
    test_files[2] = "./test/test1.txt";
    test_files[3] = "./test/test2.txt";
    return test_files;
}

void free_test_files(char ** test_files) {
    free(test_files);
}

////
//// INDEX
////

Index* make_index() {
    Index * ind = (Index * )malloc(sizeof(Index));
    ind->file_count=0;
    return ind;
}

void free_index(Index * ind) {
    free(ind);
}

void add_files_to_index(int argc, char * argv[], Index *ind){
    // Add all arguments after the first to the index
    for (int i=2; i < argc; i++){
        // TODO: broke, will fix - elvis
        //ind->file_names[ind->file_count] = argv[i];
        //puts(ind->file_names[ind->file_count]); // Print added filename
        ind->file_count +=1;
    }
}

Index* load_index() {
    // TODO: implement this (temporary example files below)
    Index * ind = make_index();
    char** test_files = get_test_files();
    add_files_to_index(4, test_files, ind);
    return ind;
}

void save_index(Index* ind) {
    // TODO: implement this
}

////
//// GRAPHNODE
////

reference_t * load_head() {
    // TODO: implement this (temporary head below)
    // Get the latest commit from head file
    reference_t * head = make_file_reference("test/test1.txt");
    return head;
}


////
//// SNAPSHOT
////

Snapshot * create_tree_node() {
    Snapshot * snapshot = (Snapshot *)malloc(sizeof(Snapshot));
    return snapshot;
}

void free_tree_node(Snapshot * snapshot) {
    free(snapshot);
}

////
//// SNAPTREE
////

SnapTree * create_snap_tree() {
    SnapTree * snap_tree = (SnapTree *)malloc(sizeof(SnapTree));
    return snap_tree;
}

void free_snap_tree(SnapTree * snap_tree) {
    free_sized_string(&snap_tree->path);
    // TODO: loop through children and each reference
    free(snap_tree);
}

SnapTree * create_snap_tree_from_index(Index * ind) {
    // TODO: Implement this (placeholder snaptree for now)
    // Create treenode from index
    // Create snaptree from treenode
    SnapTree * snap_tree = create_snap_tree();
    return snap_tree;
}

SnapTree * create_snap_tree_current_dir() {
    // Create a SnapTree of the current directory
    // TODO: i broke this, will fix ASAP -elvis
    SnapTree * snap_tree = create_snap_tree();
    //snap_tree->tree_head = create_tree_node();
    int i = 0;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("./")) != NULL) { // Open current directory
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) { // Get all the files in the current directory that are not . or ..
                //snap_tree->tree_head->snap[i] = ent->d_name;
                //printf("file %i: %s\n", i, snap_tree->tree_head->snap[i]);
                i++;
            }
        }
        closedir (dir);
    } else {
        error("Could not open directory");
    }
    return snap_tree;
}

reference_t * create_ref_from_snap_tree(SnapTree * snap) {
    return make_reference(snap, sizeof(SnapTree));
}

////
//// COMMIT
////

Commit * create_commit(char * message) {
    //Index * ind = load_index();
    Commit * commit = (Commit *)malloc(sizeof(Commit));
    commit->author = make_sized_string("author");
    printf("%s\n", commit->author->string);
    commit->message = make_sized_string(message);
    commit->parents_count = 1;
    commit->parents[0] = load_head();
    // TODO change tree
    commit->tree = make_file_reference("./test/test1.txt");
    // TODO: replace with a reference
    //commit->tree = create_snap_tree_from_index(ind);
    return commit;
}

Commit * create_initial_commit() {
    Commit * commit = (Commit *)malloc(sizeof(Commit));
    commit->author = make_sized_string("author");
    commit->message = make_sized_string("Initial Commit");
    commit->parents_count = 0;
    // TODO: Can parent be an empty reference?
    commit->parents[0] = (reference_t *) malloc(sizeof(reference_t)); 
    SnapTree * initial_snap = create_snap_tree_current_dir();
    commit->tree = create_ref_from_snap_tree(initial_snap);
    return commit;
}


void free_commit(Commit * commit) {
    free(commit->tree);
    free(commit->author);
    free(commit->message);
    // TODO make sure this works with multiple
    free(commit->parents);
    free(commit);
}

int main(int argc, char * argv[]) {
    if (argc == 1) {
        puts("Not a valid use of ./jem!");
        return 0;  }
    char * command = argv[1];

    if (!strcmp(command, "add")) {
        Index* ind = make_index();
        add_files_to_index(argc, argv, ind);
        save_index(ind);
        puts("Files Added");
        free_index(ind);
    }

    else if (!strcmp(command, "hash")) { // TESTING BLOCK FOR THE SHA1 HASHING FUNC
        reference_t *hash = make_file_reference("test/test1.txt");
        print_reference(hash);
        // after done using the reference
        free_reference(hash);
    }

    else if (!strcmp(command, "commit")) {
        char * message = "";
        if (argc > 2) {
            message = argv[2];
        }
        Commit * commit = create_commit(message);
        size_t size = commit_size(commit);
        printf("commit size: %lu bytes\n", size);
        unsigned char *serialized_commit = malloc(size);
        serialize_commit(&serialized_commit, commit);
        puts("create");
        reference_t *commit_ref = write_buffer_to_disk(&serialized_commit, size);
        puts("commit");

        // Below is for testing deserializing a commit
        unsigned char **buff;
        read_ref_from_disk(&buff, commit_ref);
        puts("read");
        Commit * com = malloc(sizeof(Commit));
        deserialize_commit(&buff, com);
        // right now the frees are causing it to crash
        //free(buff);
        //free(serialized_commit);
        //free_commit(com);
        //free_commit(commit);
        //puts("loaded");
    }

    else if (!strcmp(command, "init")) {
        // Check if the jem directory exists; if not, make it
        struct stat st = {0};
        if (stat("./.jem", &st) == -1) {
            mkdir("./.jem", 0777);
            Commit * commit = create_initial_commit();
            size_t size = commit_size(commit);
            unsigned char *serialized_commit = malloc(size);
            serialize_commit(&serialized_commit, commit);
            reference_t *commit_ref = write_buffer_to_disk(&serialized_commit, size);
            puts("JEM Initialized. Initial Commit:\n");
            print_reference(commit_ref);
        } else {
            puts("This is a JEM project already");
        }
        // TODO: add relevent initial files to .jem
    }

    else if (!strcmp(command, "checkout")) {
        puts("checkout");
    }
    return 0;
}