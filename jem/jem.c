#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "structs.h"
#include <dirent.h>
#include <errno.h>
#include <glib.h>

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

int checkdir(){
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

void add_files_to_index(int argc, char * argv[], Index *ind){
    // Add all arguments after the first to the index
    for (int i=2; i < argc; i++){
        ind->f_addrs[ind->file_count] = argv[i];
        ind->file_count +=1;
        puts(ind->f_addrs[ind->file_count-1]);
    }
    ind->file_count+=argc-2;
}

Index* make_index() {
    Index * ind = (Index * )malloc(sizeof(Index));
    ind->file_count=0;
    return ind;
}

void free_index(Index * ind) {
    free(ind);
}

char ** get_test_files() {
    // Index 0 and 1 reserved for filename and command
    char** test_files = malloc(4 * sizeof(char*));
    test_files[2] = "./test/test1.txt";
    test_files[3] = "./test/test2.txt";
    return test_files;
}

void free_test_files(char ** test_files) {
    free(test_files);
}

Index* load_index() {
    // TODO: implement this (temporary example files below)
    Index * ind = make_index();
    char** test_files = get_test_files();
    add_files_to_index(4, test_files, ind);
    return ind;
}

GraphNode * load_head() {
    // TODO: implement this (temporary head below)
    GraphNode * head = malloc(sizeof(GraphNode));
    return head;
}

SnapTree * create_snaptree(Index * ind) {
    // TODO: Implement this (placeholder snaptree for now)
    // Create treenode from index
    // Create snaptree from treenode
    SnapTree * snap_tree = (SnapTree *)malloc(sizeof(SnapTree));
    return snap_tree;
}

Commit * create_commit() {
    Index * ind = load_index();
    Commit * commit = (Commit *)malloc(sizeof(Commit));
    commit->author = "test_author";
    commit->message = "test commit message";
    commit->parent = load_head();
    commit->snapshots = *create_snaptree(ind);
    return commit;
}

int main(int argc, char * argv[]) {
    if (argc == 1) {
        puts("Not a valid use of ./jem!");
        return 0;  }
    char * command = argv[1];
    if (!strcmp(command, "add")) {
        Index* ind = make_index();
        add_files_to_index(argc, argv, ind);
        puts("Files Added");
        free_index(ind);
    }
    else if (!strcmp(command, "commit")) {
        create_commit();
        puts("commit");
    }
    else if (!strcmp(command, "init")) {
        puts("init");
        if (checkdir() == 0) {
            printf("Checked dir");
            const char* dir_name = ".jem";
            DIR *dir = opendir("~/");
            int dfd = dirfd(dir);
            int dir_status = mkdirat(dfd, dir_name, S_IRWXU);
            if (dir_status == -1) {
                error("Failed to create .jem directory");
            }
        }
    }
    else if (!strcmp(command, "checkout")) {
        puts("checkout");
    }
    return 0;
}