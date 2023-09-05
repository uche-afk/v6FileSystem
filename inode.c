#include <stdio.h>
#include <assert.h>

#include "inode.h"
#include "diskimg.h"
#include <stdbool.h>

#define INODES_PER_BLOCK (DISKIMG_SECTOR_SIZE/sizeof(struct inode))
#define NUM_BLOCK_NUMS_PER_BLOCK (DISKIMG_SECTOR_SIZE/sizeof(uint16_t))
#define NUM_SINGLY_INDIRECT_BLOCKS 256 * 7  // NUM_BLOCK_NUMS_PER BLOCK times 7 
#define DOUBLY_INDIRECT_INDEX 7


int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    struct inode buffer[INODES_PER_BLOCK];

    int bytesRead = diskimg_readsector(fs->dfd, ((inumber - 1)/INODES_PER_BLOCK) + INODE_START_SECTOR, buffer); // read the correct sector
    if (bytesRead == -1) {return -1;}

    *inp = buffer[(inumber - 1) % INODES_PER_BLOCK];  // update buffer
    return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if ((inp->i_mode & ILARG) == 0) {
        return inp->i_addr[blockNum];
    } else {
        if (blockNum < NUM_SINGLY_INDIRECT_BLOCKS) {
            uint16_t buffer[NUM_BLOCK_NUMS_PER_BLOCK];
                
            int bytesRead = diskimg_readsector(fs->dfd, ((inp->i_addr[blockNum/NUM_BLOCK_NUMS_PER_BLOCK])), buffer);
            if (bytesRead == -1) {return -1;}

            return buffer[blockNum % NUM_BLOCK_NUMS_PER_BLOCK];
        } else {
            uint16_t bufferOne[NUM_BLOCK_NUMS_PER_BLOCK];
            uint16_t bufferTwo[NUM_BLOCK_NUMS_PER_BLOCK];
            
            int bytesRead = diskimg_readsector(fs->dfd, ((inp->i_addr[DOUBLY_INDIRECT_INDEX])), bufferOne);
            if (bytesRead == -1) {return -1;}

            int bytesReadTwo = diskimg_readsector(fs->dfd, ((bufferOne[((blockNum/NUM_BLOCK_NUMS_PER_BLOCK) - DOUBLY_INDIRECT_INDEX)])), bufferTwo);
            if (bytesReadTwo == -1) {return -1;}

            return bufferTwo[blockNum % NUM_BLOCK_NUMS_PER_BLOCK];
        }
    }
}

int inode_getsize(struct inode *inp) {
    return ((inp->i_size0 << 16) | inp->i_size1);
}
