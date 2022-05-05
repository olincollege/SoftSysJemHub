// compiler secret magic
#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE
#include <ftw.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <glib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "reference.h"
#include "storage.h"
#include "commit.h"
#include "index.h"
#include "snaptree.h"
#include <stdbool.h>

/*
jem init :
- Create .jem directory
- Create initial commit for working tree

jem commit :
- Create a snapshot of the working directory
- Create a commit with the snapshot
- Update HEAD to be the latest commit

jem checkout : 
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


// https://stackoverflow.com/a/4770992
bool prefix(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

// https://stackoverflow.com/a/5467788
int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (prefix(".jem/", fpath)) return 0; // don't delete .jem
    if (!strcmp(fpath, "./test")) return 0; // don't delete root
    int rv = remove(fpath);
    if (rv) perror(fpath);

    return rv;
}

int rmrf(char *path) {
    return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
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

int is_directory(struct dirent * entry){
    int is_dir = 0;
    // Check if entry is a directory
    // Source: https://stackoverflow.com/questions/39429803/how-to-list-first-level-directories-only-in-c/39430337#39430337
    if (entry->d_type != DT_UNKNOWN && entry->d_type != DT_LNK) {
        is_dir = (entry->d_type == DT_DIR);
    } else {
        struct stat stbuf;
        stat(entry->d_name, &stbuf);
        is_dir = S_ISDIR(stbuf.st_mode);
    }
    return is_dir;
}

int is_parent_directory(struct dirent *de) {
    // Check if parent directory . or ..
    return (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."));
}

int get_dir_filecount_shallow(char * path){
    DIR *folder;
    int files = 0;
    struct dirent *de;
    folder = opendir(path);
    if(folder == NULL)
    {
        error("Unable to read directory");
    }
    while( (de=readdir(folder)) )
    {
        if (is_parent_directory(de)) {
            continue;
        }
        files++;
    }
    closedir(folder);
    return files;
}

void update_head(reference_t * commit_ref) {
    // Create the head file if doesnt exist and write the reference to it
    int fd = open("./.jem/HEAD", O_CREAT);
    if(fd == -1){
        fputs("Unable to create file.", stderr);
        exit(1);
    }
    // need to write bytes because no null terminator
    write(fd, (const char*)commit_ref, sizeof(reference_t));
    close(fd);
}

reference_t* load_head() {
    FILE * fp;
    fp = fopen("./.jem/HEAD", "r");
    reference_t * ref = malloc(sizeof(reference_t));
    if(fp == NULL){
        fputs("Unable to open file.", stderr);
        exit(1);
    }
    fread(ref, 1, SHA_DIGEST_LENGTH, fp);
    if ( ferror( fp ) != 0 ) {
        fputs("Error reading file", stderr);
    }
    fclose(fp);
    return ref;
}


////
//// SNAPSHOT
////


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

SnapTree * create_snap_tree_from_index(Index * ind) {
    // TODO: Implement this (placeholder snaptree for now)
    // Create treenode from index
    // Create snaptree from treenode
    SnapTree * snap_tree = create_snap_tree();
    return snap_tree;
}

// load a snap tree
void load_snaptree(reference_t *tree_ref) {
    // deserialize the tree
    unsigned char *buffer, start;
    read_ref_from_disk(&buffer, tree_ref);
    start = buffer;
    SnapTree *tree = malloc(sizeof(SnapTree));
    deserialize_snaptree(&buffer, tree);
    // iterate through the children
    Snapshot *snap;
    for (size_t i = 0; i < tree->children_length; i++) {
        snap = tree->children[i];
        if (snap->type == SST_FILE) {
            copy_file_from_jem(snap->path->string, snap->reference);
        } else {
            // make directory
            mkdir(snap->path->string, snap->mode);
            load_snaptree(snap->reference);
        }
    }
}

// create a snap tree, save it and return it's reference
reference_t *create_snap_tree_dir(char * path) {
    // Create a SnapTree of the working directory
    SnapTree * snap_tree = create_snap_tree();
    // need to know how many children to allocate a big enough array
    snap_tree->children_length = get_dir_filecount_shallow(path);
    snap_tree->children = malloc(sizeof(Snapshot *) * snap_tree->children_length);
    
    // traverse file tree
    DIR *dir;
    size_t child = 0;
    struct dirent *de;
    int is_dir;
    SnapshotType type;
    char filepath[300];
    if ((dir = opendir (path)) != NULL) {
        // go through all the entries at this level
        while( (de=readdir(dir)) ) {
            is_dir = is_directory(de);
            strcpy(filepath, path);
            strcat(filepath, "/");
            strcat(filepath, de->d_name);
            reference_t *child_ref;
            struct stat fstats;

            if (is_dir & is_parent_directory(de)) {
                continue;
            } else if(is_dir) {
                // recursive call
                child_ref = create_snap_tree_dir(filepath);
                type = SST_DIR;
            } else {
                // copy the file into storage
                child_ref = make_file_reference(filepath);
                copy_file_to_jem(filepath, child_ref);
                type = SST_FILE;
            }
            Snapshot * child_snap = malloc(sizeof(Snapshot));
            // use stat to get mode
            // if the INDEX was in use, stat would be called during add and record last modified timestamp
            stat(filepath, &fstats);
            // this will get serialized right next to the other children and saved together
            child_snap->mode = fstats.st_mode;
            child_snap->type = type;
            child_snap->path = make_sized_string(filepath);
            child_snap->reference = child_ref;
            snap_tree->children[child] = child_snap;
            child++;
        }
        closedir (dir);
    } else {
        error("Could not open directory");
    }
    // serialize, save, and return reference
    size_t size = snaptree_size(snap_tree);
    unsigned char * buffer = malloc(size);
    unsigned char * start = buffer;
    serialize_snaptree(&buffer, snap_tree);
    return write_buffer_to_disk(&start, size);
}

reference_t * create_ref_from_snap_tree(SnapTree * snap) {
    return make_reference(snap, sizeof(SnapTree));
}

////
//// COMMIT
////

Commit * create_commit(char * message) {
    Commit * commit = (Commit *)malloc(sizeof(Commit));
    commit->author = make_sized_string(get_username());
    commit->message = make_sized_string(message);
    commit->parents_count = 1;
    commit->parents[0] = load_head();
    commit->tree = create_snap_tree_dir("test");
    return commit;
}

Commit * create_initial_commit() {
    Commit * commit = (Commit *)malloc(sizeof(Commit));
    commit->author = make_sized_string(get_username());
    commit->message = make_sized_string("Initial Commit");
    commit->parents_count = 0;
    // TODO: Can parent be an empty reference?
    commit->parents[0] = (reference_t *) malloc(sizeof(reference_t)); 
    commit->tree = create_snap_tree_dir("test");
    return commit;
}


void free_commit(Commit * commit) {
    free(commit->tree);
    free(commit->author);
    free(commit->message);
    // TODO: make sure this works with multiple
    free(commit->parents);
    free(commit);
}

int main(int argc, char * argv[]) {
    if (argc == 1) {
        puts("Not a valid use of ./jem!");
        return 0;  
    }
    char * command = argv[1];
    if (!strcmp(command, "head")) {
        // Load the head and print its reference
        reference_t *ref = load_head();
        print_reference(ref);
    }
    else if (!strcmp(command, "commit")) {
        char * message = "";
        if (argc > 2) {
            message = argv[2];
        }
        Commit * commit = create_commit(message);
        print_commit(commit);
        size_t size = commit_size(commit);
        unsigned char *serialized_commit = malloc(size);
        serialize_commit(&serialized_commit, commit);
        reference_t *commit_ref = write_buffer_to_disk(&serialized_commit, size);
        update_head(commit_ref);
        puts("Commit Created:");
        print_reference(commit_ref);
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
            update_head(commit_ref);
            puts("JEM Initialized. Initial Commit:");
            print_reference(commit_ref);
        } else {
            puts("This is a JEM project already");
        }
    }

    else if (!strcmp(command, "checkout")) {
        if (argc != 3) {
            error("Please put a valid reference ID!\n Usage: ./jem checkout REF_ID\n");
        }
        char * REF_ID = argv[2];

        reference_t * new_ref = char_to_reference(REF_ID);

        // load in the referenced commit
        unsigned char *buffer, *start;
        read_ref_from_disk(&buffer, new_ref);
        start = buffer;
        Commit * commit = malloc(sizeof(Commit));
        deserialize_commit(&buffer, commit);
        print_commit(commit);
        // delete everything except .jem
        rmrf("./test/"); // TODO: in an ideal world we would know which files changed and skip this
        //mkdir("./test", 0777); // recreate "root" directory

        load_snaptree(commit->tree);

        update_head(new_ref);
        //print_commit(commit);
        puts("Loaded Commit:");
        print_reference(commit->tree);

    }
    
    return 0;
}