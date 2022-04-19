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

int main(int argc, char * argv[]) {
    if (argc == 1) {
        puts("Not a valid use of ./jem!");
        return 0;  }
    
    char * command = argv[1];

    if (!strcmp(command, "add")) { // strcmp returns 0 when true, so use ! to get 1
        puts("add");
    }
    else if (!strcmp(command, "commit")) {
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