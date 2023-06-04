#ifndef CLEANUP_H
#define CLEANUP_H
#include <ext.h>


typedef struct {
  VkInstance instance;
} CleanupArgs;

void cleanup(CleanupArgs* args);

#endif // CLEANUP_H
