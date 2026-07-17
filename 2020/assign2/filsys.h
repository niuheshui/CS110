#ifndef _FILSYS_H_
#define _FILSYS_H_

#include <stdint.h>

/**
 * This is the header file from Unix Version 6 that describes the superblock of
 * the file system.  It has been converted to use stdint.h to work on 32- and 64-
 * bit systems. See comment in inode.h. 
 */

/**
 * Definition of the unix super block.  The root super block is allocated and
 * read in iinit/alloc.c.  Subsequently a super block is allocated and read
 * with each mount (smount/sys3.c) and released with unmount (sumount/sys3.c).
 * A disk block is ripped off for storage.  See alloc.c for general alloc/free
 * routines for free list and I list.
 */

struct filsys {
  uint16_t	s_isize;	// inode 区大小 (以 block 为单位)
  uint16_t	s_fsize;	// 文件系统总 block 数
  uint16_t	s_nfree;	// 当前空闲 block 列表数量
  uint16_t	s_free[100];	// 空闲 block 栈
  uint16_t	s_ninode;	// 当前空闲的 inode 数量
  uint16_t	s_inode[100];	// 空闲 inode 列表
  uint8_t	  s_flock;	// lock during free list manipulation
  uint8_t   s_ilock;	// lock during I list manipulation
  uint8_t	  s_fmod;		// super block modified flag
  uint8_t	  s_ronly;	// mounted read-only flag
  uint16_t	s_time[2];	// current date of last update
  uint16_t	pad[48];        // aligns struct filesys to be 512 bytes in size (the block size!)
};

#endif 
