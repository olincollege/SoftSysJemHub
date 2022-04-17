#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

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
    }
    else if (!strcmp(command, "checkout")) {
        puts("checkout");
    }
    return 0;
}