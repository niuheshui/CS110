
#include "pathname.h"
#include "directory.h"
#include "direntv6.h"
#include "ino.h"
#include "inode.h"
#include "unixfilesystem.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>


int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
  if (strcmp(pathname, "/") == 0) {
    return 1;
  }

  char name_buf[14];
  int pathname_index = 1;
  int pathname_len = strlen(pathname);
  int inumber = 1;
  struct direntv6 entry;

  while (pathname_index < pathname_len) {
    int buf_index = 0;
    while (1) {
      if (pathname[pathname_index] == '/' || pathname[pathname_index] == '\0') {
        pathname_index++;
        break;
      }
      name_buf[buf_index++] = pathname[pathname_index++];
    }
    name_buf[buf_index++] = '\0';

    if (directory_findname(fs, name_buf, inumber, &entry) < 0) {
      return -1;
    }
    inumber = entry.d_inumber;
  }

  return inumber;
}
