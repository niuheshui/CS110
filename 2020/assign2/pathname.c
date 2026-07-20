
#include "pathname.h"
#include "directory.h"
#include "direntv6.h"
#include "file.h"
#include "ino.h"
#include "inode.h"
#include "diskimg.h"
#include "unixfilesystem.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DISKIMG_SECTOR_SIZE_DIR_ENTRIES (DISKIMG_SECTOR_SIZE / sizeof(struct direntv6))

int is_directory(struct inode *inp) {
  return (inp->i_mode & IALLOC) && ((inp->i_mode & IFMT) == IFDIR);
}

int directory_countentries(struct unixfilesystem* fs, struct inode* inp) {
  return inode_getsize(inp) / sizeof(struct direntv6);
}

int find_directory_entry(struct unixfilesystem* fs, int inumber, const char* name) {
  struct inode inp;
  if (inode_iget(fs, inumber, &inp) < 0) {
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
    if (file_getblock(fs, inumber, bno, entry_buf) < 0) {
      return -1;
    }
    for (int i = 0; i < DISKIMG_SECTOR_SIZE_DIR_ENTRIES && count < entry_count; i++, count++) {
      assert(entry_buf[i].d_inumber != 0);
      if (strcmp(entry_buf[i].d_name, name) == 0) {
        return entry_buf[i].d_inumber;
      }
    }
  }
  return -1;
}

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
  if (strcmp(pathname, "/") == 0) {
    return 1;
  }
  struct inode root;
  if (inode_iget(fs, 1, &root) < 0) {
    return -1;
  }
  char name_buf[14];
  int i = 1;
  int j = 0;
  int len = strlen(pathname);
  int inumber = 1;
  while (i < len) {

    while (1) {
      if (pathname[i] == '/' || pathname[i] == '\0') {
        i++;
        break;
      }
      name_buf[j++] = pathname[i++];

    }
    name_buf[j++] = '\0';

    if ((inumber = find_directory_entry(fs, inumber, name_buf)) <= 0) {
      return -1;
    }

    j = 0;
  }

  return inumber;
}
