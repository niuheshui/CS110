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
#define INDIRECT_BLOCK_NUM (DISKIMG_SECTOR_SIZE / sizeof(uint16_t))

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
  if (inumber < 1) {
    fprintf(stderr, "inode_iget: inumber %d is out of range\n", inumber);
    return -1;
  }

  if (INODES_PER_SECTOR * fs->superblock.s_isize < inumber) {
    fprintf(stderr, "inode_iget: inumber %d is out of range\n", inumber);
    return -1;
  }

  int sector = SUPERBLOCK_SECTOR + 1 + ((inumber - 1) / INODES_PER_SECTOR);
  struct inode buf[INODES_PER_SECTOR];
  if (diskimg_readsector(fs->dfd, sector, buf) < 0) {
    return -1;
  }
  *inp = buf[(inumber - 1) % INODES_PER_SECTOR];

  return 1;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
  int is_small_file = ((inp->i_mode & ILARG) == 0);
  if (is_small_file) {
    return inp->i_addr[blockNum];
  }

  int file_size = inode_getsize(inp);
  if (blockNum > (file_size / DISKIMG_SECTOR_SIZE)) {
    fprintf(stderr, "inode_indexlookup: blockNum %d is out of range\n", blockNum);
    return -1;
  }

  uint16_t buf[INDIRECT_BLOCK_NUM];
  if (blockNum < (INDIRECT_BLOCK_NUM * INDIR_ADDR)) {
    if (diskimg_readsector(fs->dfd, inp->i_addr[blockNum / INDIRECT_BLOCK_NUM], buf) < 0) {
      return -1;
    }
    return buf[blockNum % INDIRECT_BLOCK_NUM];
  } else {
    if (diskimg_readsector(fs->dfd, inp->i_addr[INDIR_ADDR], buf) < 0) {
      return -1;
    }
    blockNum -= INDIRECT_BLOCK_NUM * INDIR_ADDR;
    int second_sector = buf[blockNum / INDIRECT_BLOCK_NUM];
    if (diskimg_readsector(fs->dfd, second_sector, buf) < 0) {
      return -1;
    }
    return buf[blockNum % INDIRECT_BLOCK_NUM];
  }

}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
