
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (strlen(pathname) == 1) {return ROOT_INUMBER;}

    // copy const pathname into buffer, make a pointer to that, and set our intial inumber
    char buf[strlen(pathname) + 1];
    strcpy(buf, pathname);
    char* pathPointer = buf + 1;
    struct direntv6 dirEnt;
    int inodeNum = ROOT_INUMBER;    

    // loop until we reach end of the path name
    while (pathPointer != NULL) {
        char* token = strsep(&pathPointer, "/");
        // printf("token: %s", token);
        if (token != NULL) {  // if this is not the name, our string is in token
            int checker = directory_findname(fs, token, inodeNum, &dirEnt);
            if (checker == -1) {return -1;}
            inodeNum = dirEnt.d_inumber;
        } else {  // if this is the last one, it is contained in pathPointer
            int checker = directory_findname(fs, pathPointer, inodeNum, &dirEnt);
            if (checker == -1) {return -1;}
            inodeNum = dirEnt.d_inumber;
            break;
        }
    }
    return inodeNum;
}
