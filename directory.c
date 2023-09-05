uochuba@myth59:~/cs111/cs111_p7$clear
                 // Collaborator: Prerit Choudhary

#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define NUM_DIRENTS_PER_BLOCK (DISKIMG_SECTOR_SIZE/sizeof(struct direntv6))
#define MAX_COMPONENT_LENGTH sizeof(dirEnt->d_name)

int directory_findname(struct unixfilesystem *fs, const char *name,
                       int dirinumber, struct direntv6 *dirEnt) {
    struct inode inp;
    inode_iget(fs, dirinumber, &inp);
    
    if ((inp.i_mode & IFMT) != IFDIR) {return -1;}  // verify this is a directory
    
    int blockCount = inode_getsize(&inp) / 512;

    // loop over the blocks in the directory
    for (int k = 0; k <= blockCount; k++) {
        struct direntv6 buffer[NUM_DIRENTS_PER_BLOCK];
        int checker = file_getblock(fs, dirinumber, k, buffer);
        if (checker == -1) {return -1;}

        int blockCountTwo = checker/sizeof(struct direntv6);
        for (int i = 0; i < blockCountTwo; i++) {
            if (strncmp(name, buffer[i].d_name, MAX_COMPONENT_LENGTH) == 0) {  // check if this is the dirent we want
                *dirEnt = buffer[i];
                return 0;
            }
        }
    }
    return -1;
}
