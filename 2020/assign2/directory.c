#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DISKIMG_SECTOR_SIZE_DIR_ENTRIES (DISKIMG_SECTOR_SIZE / sizeof(struct direntv6))

static inline int is_directory(struct inode *inp) {
  return (inp->i_mode & IALLOC) && ((inp->i_mode & IFMT) == IFDIR);
}

int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
  struct inode inp;
  if (inode_iget(fs, dirinumber, &inp) < 0) {
    return -1;
  }
  if (!is_directory(&inp)) {
    return -1;
  }

  int file_size = inode_getsize(&inp);
  int entry_count = file_size / sizeof(struct direntv6);
  int count = 0;
  struct direntv6 entry_buf[DISKIMG_SECTOR_SIZE_DIR_ENTRIES];

  for (int offset = 0; offset < file_size; offset += DISKIMG_SECTOR_SIZE) {
    int bno = offset / DISKIMG_SECTOR_SIZE;
    if (file_getblock(fs, dirinumber, bno, entry_buf) < 0) {
      return -1;
    }
    for (int i = 0; i < DISKIMG_SECTOR_SIZE_DIR_ENTRIES && count < entry_count; i++, count++) {
      assert(entry_buf[i].d_inumber != 0);
      if (strcmp(entry_buf[i].d_name, name) == 0) {
        *dirEnt = entry_buf[i];
        return 1;
      }
    }
  }
  return -1;
}
