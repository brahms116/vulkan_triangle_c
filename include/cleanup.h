#ifndef CLEANUP_H
#define CLEANUP_H
#include <ext.h>


typedef struct {
  VkInstance *pInstance;
  VkSurfaceKHR *pSurface;
} CleanupArgs;

void cleanup(CleanupArgs* args);

#endif // CLEANUP_H
