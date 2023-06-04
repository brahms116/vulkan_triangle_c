#include <cleanup.h>

void cleanup(CleanupArgs *args){
  vkDestroySurfaceKHR(*args->pInstance, *args->pSurface, NULL);
  vkDestroyInstance(*args->pInstance, NULL);
}
