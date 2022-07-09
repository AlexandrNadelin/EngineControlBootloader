#ifndef __FileSystem_H
#define __FileSystem_H

#include "lwip/err.h"

struct fs_file {
  const char *data;
  int len;
  int index;
  void *pextension;
  u8_t flags;
};

err_t fs_open(struct fs_file *file, const char *name);

#endif



