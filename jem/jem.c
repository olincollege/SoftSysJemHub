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
            // TODO: make directory reference here
            puts("File is directory");
            reference_t** sub_dir_children = make_reference_children(filepath);
            int i_d = 0;
            while (sub_dir_children[i_d] != NULL) {
                children[i] = sub_dir_children[i_d];
                i++;
                i_d++;
            }
        } else {
            children[i] = make_file_reference(filepath);
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
    commit->message = make_sized_string(message);
    commit->parents_count = 1;
    commit->parents[0] = load_head();
    // TODO: change tree
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

    // else if (!strcmp(command, "hash")) { // TESTING BLOCK FOR THE SHA1 HASHING FUNC
    //     reference_t *hash = make_file_reference("test/test1.txt");
    //     print_reference(hash);
    //     // after done using the reference
    //     free_reference(hash);
    // }

    else if (!strcmp(command, "snap")) { // TESTING BLOCK FOR SNAPTREE SERIALIZATION
        
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
        puts("Commit Created");

        // Below is for testing deserializing a commit
        unsigned char **buff;
        read_ref_from_disk(&buff, commit_ref);
        Commit * com = malloc(sizeof(Commit));
        deserialize_commit(&buff, com);
        print_commit(commit);
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
        // reference_t reference;
        // strcpy(reference, argv[2]); // Copy from something assumed to be a reference_t type to a reference_t type
        // printf("%s", reference);

        // TESTING SEGMENT, TEMPORARY VARIABLE SETTING
        reference_t* reference = make_file_reference("./.jem/testcheck.txt");
        printf("Made file reference %hhu\n", reference);
        size_t size = sizeof(reference_t);
        unsigned char * buffer = (unsigned char * ) malloc(size);
        serialize_reference(&buffer, reference);
        printf("Serialized, %u\n", *buffer);
        write_buffer_to_disk(&buffer, sizeof(buffer));
        printf("written to disk %u\n", *buffer);
        print_reference(reference);
        reference_t * new_ref = malloc(sizeof(reference_t));
        puts("mallocd new ref\n");
        read_ref_from_disk(&buffer, reference);
        puts("Read from disk\n");
        deserialize_reference(&buffer, new_ref);
        puts("Deserialized\n");

        update_head(new_ref);
        puts("Updated head");

        printf("Final output: %hhu", new_ref);
    }
    

    
    return 0;
}