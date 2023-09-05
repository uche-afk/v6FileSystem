#include <stdio.h>
#include <assert.h>

#include "file.h"
#include "inode.h"
#include "diskimg.h"

#define SECTOR_SIZE 512

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode inp;
    int checker = inode_iget(fs, inumber, &inp);  // get the inode
    if (checker == -1) {return -1;}

    int sectorNumber = inode_indexlookup(fs, &inp, blockNum); // get the index
    if (sectorNumber == -1) {return -1;}

    int checkerTwo = diskimg_readsector(fs->dfd, sectorNumber, buf);  // read with buffer
    if (checkerTwo == -1) {return -1;}

    int fileSize = inode_getsize(&inp);
    
    if (fileSize == 0) {return 0;}

    // return the mod of sector size if this is the last block, 512 if not (because the block is full)
    if ((blockNum + 1) * SECTOR_SIZE > fileSize) {
        return fileSize % SECTOR_SIZE;
    } else {
        return SECTOR_SIZE;
    }
}
