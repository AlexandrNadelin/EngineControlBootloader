#ifndef FSDATA_H
#define FSDATA_H
#include "main.h"

struct fsdata_file {
  const struct fsdata_file *next;
  const unsigned char *name;
  const unsigned char *data;
  int len;
  uint8_t flags;
};


#endif

