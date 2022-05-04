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

int get_dir_filecount(char * path){
    DIR *folder;
    int files = 0;
    int is_dir = 0;
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
        is_dir = is_directory(de);
        if (is_dir) {
            char filepath[300];
            strcpy(filepath, path);
            strcat(filepath, "/");
            strcat(filepath, de->d_name);
            printf("%s\n", filepath);
            // Recursively get all files
            files += get_dir_filecount(filepath);
        }
        files++;
    }
    closedir(folder);
    return files;
}


reference_t** make_reference_children(char* path)
{
    // Go through path and for each file
    // Create a snapshot, serialize and save it
    // Add the reference to children
    // Return all children
    DIR *folder;
    struct dirent *de;
    int is_dir;
    int files = get_dir_filecount(path);
    printf("File Count: %d\n", files);
    reference_t** children = calloc(files, sizeof(reference_t));
    folder = opendir(path);
    if(folder == NULL) {
        error("Unable to read directory");
    }
    int i = 0;
    while( (de=readdir(folder)) ) {
        if (is_parent_directory(de)) {
            continue;
        }
        printf("File %3d: %s\n", i, de->d_name);
        is_dir = is_directory(de);
        char filepath[300];
        strcpy(filepath, path);
        strcat(filepath, "/");
        strcat(filepath, de->d_name);
        if (is_dir) {
            // TODO: make directory reference here? Possibly not needed
            puts("File is directory");
            reference_t** sub_dir_children = make_reference_children(filepath);
            int i_d = 0;
            while (sub_dir_children[i_d] != NULL) {
                children[i] = sub_dir_children[i_d];
                i++;
                i_d++;
            }
        } else {
            // Each file goes here
            reference_t *file_ref = make_file_reference(filepath);
            copy_file_to_jem(filepath, file_ref);
            Snapshot * file_snap = malloc(sizeof(Snapshot));
            stat(filepath, file_snap->mode);
            file_snap->path = make_sized_string(filepath);
            file_snap->reference = file_ref;
            size_t size = snaptree_size(file_snap);
            unsigned char * buffer = malloc(size);
            serialize_snapshot(&buffer, file_snap);
            puts("Writing snapshot to disk");
            reference_t *snap_ref = write_buffer_to_disk(&buffer, size);
            children[i] = snap_ref;
            i++;
        }
    }
    closedir(folder);
    return children;
}


void update_head(reference_t * commit_ref) {
    // Create the head file if doesnt exist and write the reference to it
    FILE * fp;
    fp = fopen("./.jem/HEAD", "w");
    if(fp == NULL){
        fputs("Unable to create file.", stderr);
        exit(1);
    }
    fputs(commit_ref, fp);
    fclose(fp);
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

SnapTree * create_snap_tree_dir(char * path) {
    // Create a SnapTree of the current directory
    // TODO: i broke this, will fix ASAP -elvis
    SnapTree * snap_tree = create_snap_tree();
    snap_tree->path = make_sized_string(path);
    stat(path, snap_tree->mode);
    int i = 0;
    DIR *dir;
    struct dirent *de;
    if ((dir = opendir (path)) != NULL) {
        snap_tree->children = make_reference_children(path);
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
    Commit * commit = (Commit *)malloc(sizeof(Commit));
    commit->author = make_sized_string("author");
    commit->message = make_sized_string(message);
    commit->parents_count = 1;
    commit->parents[0] = load_head();
    SnapTree * initial_snap = create_snap_tree_dir("test");
    commit->tree = create_ref_from_snap_tree(initial_snap);
    return commit;
}

Commit * create_initial_commit() {
    Commit * commit = (Commit *)malloc(sizeof(Commit));
    commit->author = make_sized_string("author");
    commit->message = make_sized_string("Initial Commit");
    commit->parents_count = 0;
    // TODO: Can parent be an empty reference?
    commit->parents[0] = (reference_t *) malloc(sizeof(reference_t)); 
    SnapTree * initial_snap = create_snap_tree_dir("test");
    commit->tree = create_ref_from_snap_tree(initial_snap);
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
    if (!strcmp(command, "snap")) { // TESTING BLOCK FOR SNAPTREE SERIALIZATION
        
        char * filepath = "test";
        SnapTree * snaptree = create_snap_tree(); // allocs snaptree
        SizedString * path = make_sized_string(filepath); // allocs sizedstring
        snaptree->path = path;
        printf("Filepath: %s\n", snaptree->path->string);

        struct stat v; // Use sys/stat.h stat() command to store file info in a struct.
        stat(filepath, &v); 
        snaptree->mode = v.st_mode; // Save the mode to the snaptree.
        puts("Mode made");

        reference_t ** children = make_reference_children(filepath);
        snaptree->children = children;
        int i = 0;
        while(children[i] != NULL) {
            print_reference(children[i]);
            i++;
        }
        puts("Added references to children");

        size_t size = snaptree_size(snaptree);
        unsigned char * serialized_snaptree = malloc(size);
        serialize_snaptree(&serialized_snaptree, snaptree);
        puts("Serialized");
        reference_t * tree_ref = write_buffer_to_disk(&serialized_snaptree, size);
        
        // Below is for testing deserializing a snaptree
        unsigned char ** buff;
        read_ref_from_disk(buff, tree_ref);
        puts("read");
        SnapTree * new_tree = malloc(sizeof(Commit));
        deserialize_snaptree(buff, new_tree);
        // right now the frees are causing it to crash
        //free(buff);
        //free(serialized_commit);
        //free_commit(com);
        //free_commit(commit);
        //puts("loaded");
    }
    else if (!strcmp(command, "head")) {
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
        puts("checkout");
        if (argc != 3) {
            error("Please put a valid reference ID!\n Usage: ./jem checkout REF_ID\n");
        }
        unsigned char * REF_ID = argv[2];
        size_t size = 2*sizeof(reference_t);
        // reference_t* reference = malloc(size);
        // reference = char_to_reference(REF_ID);
        // printf("Reference given: %s : %hhu\n", REF_ID, reference);

        // // TESTING SEGMENT, TEMPORARY VARIABLE SETTING
        // reference_t* reference = make_file_reference("./test/test1.txt");
        // // print_reference(reference);
        // printf("Made file reference %hhu\n", reference);
        
        printf("strlen(REF_ID) + 5 = %i\n", strlen(REF_ID) + 5);
        unsigned char * buffer = (unsigned char * ) malloc(size);
        read_ref_from_disk_char(&buffer, REF_ID);
        Commit * commit = malloc(sizeof(Commit));
        deserialize_commit(&buffer, commit);
        print_commit(commit);
        // TODO this reference is wrong and we need to convert to unsigned char
        reference_t * new_ref = REF_ID;
        // TODO: Unpack and save into working directory
        print_reference(new_ref);
        update_head(new_ref);
        puts("Updated head");

    }
    
    return 0;
}