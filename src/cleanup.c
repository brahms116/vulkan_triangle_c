#include <cleanup.h>

void cleanup(CleanupArgs *args){
  vkDestroyInstance(args->instance, NULL);
}
