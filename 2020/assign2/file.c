#include <stdio.h>
#include <assert.h>

#include "file.h"
#include "ino.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
  struct inode inp;
  if (inode_iget(fs, inumber, &inp) < 0) {
    return -1;
  }

  int sector = inode_indexlookup(fs, &inp, blockNum);
  if (sector < 0) {
    return -1;
  }

  if(diskimg_readsector(fs->dfd, sector, buf) < 0) {
    return -1;
  }

  int file_size = inode_getsize(&inp);
  int is_last_block = ((file_size / DISKIMG_SECTOR_SIZE) == blockNum);

  return is_last_block ? file_size % DISKIMG_SECTOR_SIZE : DISKIMG_SECTOR_SIZE; 
}
