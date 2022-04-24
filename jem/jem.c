#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "structs.h"
#include <dirent.h>
#include <errno.h>
#include <glib.h>
#include <sys/types.h>
#include <unistd.h>
#include <openssl/sha.h>

#define MAX_FILE_SIZE 8192
#define SHA1_LENGTH 160
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
        ind->file_names[ind->file_count] = argv[i];
        puts(ind->file_names[ind->file_count]); // Print added filename
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

GraphNode * load_head() {
    // TODO: implement this (temporary head below)
    // Get the latest commit
    GraphNode * head = malloc(sizeof(GraphNode));
    return head;
}


////
//// TREENODE
////

TreeNode * create_tree_node() {
    TreeNode * tree_node = (TreeNode *)malloc(sizeof(TreeNode));
    return tree_node;
}

void free_tree_node(TreeNode * tree_node) {
    free(tree_node);
}

////
//// SNAPTREE
////

SnapTree * create_snap_tree() {
    SnapTree * snap_tree = (SnapTree *)malloc(sizeof(SnapTree));
    return snap_tree;
}

void free_snap_tree(SnapTree * snap_tree) {
    free(snap_tree->tree_head);
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
    SnapTree * snap_tree = create_snap_tree();
    snap_tree->tree_head = create_tree_node();
    int i = 0;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("./")) != NULL) { // Open current directory
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) { // Get all the files in the current directory that are not . or ..
                snap_tree->tree_head->snap[i] = ent->d_name;
                printf("file %i: %s\n", i, snap_tree->tree_head->snap[i]);
                i++;
            }
        }
        closedir (dir);
    } else {
        error("Could not open directory");
    }
    return snap_tree;
}

// void print_snap_tree(SnapTree tree) {
//     int snap_num = 0;
//     int i = 0;
//     while (tree.tree_head.snap != NULL) {
//         while (tree.tree_head.snap[i] != NULL) {
//             puts(tree.tree_head.snap[i]);
//             i++;
//         }
//         tree.tree_head = * tree.tree_head.descendants;
//         snap_num += 1;
//     }
// }

////
//// COMMIT
////

char * get_data_from_file(FILE * file) {
    char * txt = (char *) malloc(MAX_FILE_SIZE * sizeof(char));
    char word[64];
    int filled = 0;
    while (fscanf(file ,"%63s", word) == 1) {
        for (int i = 0; i < strlen(word); i++){
            txt[filled] = word[i];
            filled++;
        }
        puts(word);
    }
    return txt;
}

void free_data_from_file(char * data){
    free(data);
}

char * hash_file(char txt[]) {
    // The data to be hashed
    size_t length = strlen(txt);
    printf("strlen = %li\n", length);

    unsigned char * hash = (char*) malloc(SHA1_LENGTH * sizeof(char)); // len(hash) = SHA_DIGEST_LENGTH
    SHA1(txt, length, hash);
    printf("your hash is %s\n", hash);
    // hash now contains the 20-byte SHA-1 hash
    return hash;
}

Commit * create_commit(char * message) {
    Index * ind = load_index();
    Commit * commit = (Commit *)malloc(sizeof(Commit));
    commit->author = "test_author";
    commit->message = message;
    commit->parent = load_head();
    commit->snapshots = create_snap_tree_from_index(ind);
    return commit;
}

void save_commit(Commit * commit) {
    // TODO: Implement this
}

void free_commit(Commit * commit) {
    free(commit->snapshots);
    free(commit->parent);
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
        FILE* file = fopen("test/test1.txt","r");
        char* file_data = get_data_from_file(file);
        printf("File data: %s\n", file_data);
        char txt[MAX_FILE_SIZE]; int i = 0;
        while (*file_data != '\0') {
            txt[i] = *file_data;
            file_data++;
            i++; 
        }
        printf("Txt = %s", txt);
        file_data -= i; // Decrement pointer to free memory and make valgrind happy
        printf("\nGot data from file\n");

        free_data_from_file(file_data);
        printf("Freed datar\n");

        char * hp = hash_file(txt);
        char hash[SHA1_LENGTH]; i=0;
        while (*hp != '\0') {
            hash[i] = *hp;
            hp++; i++;
        }

        free_data_from_file(hp - i);

        printf("\n Freed hp \n");
        printf("%s\n", hash);
    }

    else if (!strcmp(command, "commit")) {
        char * message = "";
        if (argc > 2) {
            message = argv[2];
        }
        Commit * commit = create_commit(message);
        save_commit(commit);
        puts("commit");
        free_commit(commit);
    }

    else if (!strcmp(command, "init")) {
        // Check if the jem directory exists; if not, make it
        struct stat st = {0};
        if (stat("./.jem", &st) == -1) {
            mkdir("./.jem", 0777);
            puts("JEM Initialized");
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