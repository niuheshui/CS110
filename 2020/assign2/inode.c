#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "inode.h"
#include "diskimg.h"
#include "ino.h"
#include "unixfilesystem.h"

#define INDIR_ADDR 7
#define INODE_SIZE sizeof(struct inode)
#define INODES_PER_SECTOR (DISKIMG_SECTOR_SIZE / INODE_SIZE)

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
  if (inumber < 1) {
    fprintf(stderr, "inode_iget: inumber %d is out of range\n", inumber);
    return -1;
  }

  if (INODES_PER_SECTOR * fs->superblock.s_isize < inumber) {
    fprintf(stderr, "inode_iget: inumber %d is out of range\n", inumber);
    return -1;
  }

  int inode_block = SUPERBLOCK_SECTOR + 1 + ((inumber - 1) / INODES_PER_SECTOR);
  struct inode INODE_BUFFER[INODES_PER_SECTOR];
  int result = diskimg_readsector(fs->dfd, inode_block, INODE_BUFFER);
  if (result < 0) {
    fprintf(stderr, "inode_iget: diskimg_readsector failed for inumber %d\n", inumber);
    return -1;
  }

  int inode_index = (inumber - 1) % INODES_PER_SECTOR;
  memcpy(inp, &INODE_BUFFER[inode_index], sizeof(struct inode));

  return 1;
}


#define INDIRECT_BLOCK_NUM (DISKIMG_SECTOR_SIZE / sizeof(uint16_t))

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
  int is_small_file = ((inp->i_mode & ILARG) == 0);
  if (is_small_file) {
    return inp->i_addr[blockNum];
  }
  // TODO: index range check

  uint16_t BLOCK_BUF[INDIRECT_BLOCK_NUM];
  if (blockNum < (INDIRECT_BLOCK_NUM * INDIR_ADDR)) {
    if (diskimg_readsector(fs->dfd, inp->i_addr[blockNum / INDIRECT_BLOCK_NUM], BLOCK_BUF) < 0) {
      return -1;
    }
    return BLOCK_BUF[blockNum % INDIRECT_BLOCK_NUM];
  } else {
    if (diskimg_readsector(fs->dfd, inp->i_addr[INDIR_ADDR], BLOCK_BUF) < 0) {
      return -1;
    }
    blockNum -= INDIRECT_BLOCK_NUM * INDIR_ADDR;
    int second_sector = BLOCK_BUF[blockNum / INDIRECT_BLOCK_NUM];
    if (diskimg_readsector(fs->dfd, second_sector, BLOCK_BUF) < 0) {
      return -1;
    }
    return BLOCK_BUF[blockNum % INDIRECT_BLOCK_NUM];
  }

}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
