#include <cleanup.h>

void cleanup(CleanupArgs *args){
  vkDestroyDevice(*args->pDevice, NULL);
  vkDestroySurfaceKHR(*args->pInstance, *args->pSurface, NULL);
  vkDestroyInstance(*args->pInstance, NULL);
}
