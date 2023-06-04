#include <cleanup.h>
#include <ext.h>
#include <instance.h>
#include <stdio.h>

int main() {
  VkInstance instance;
  if (createVulkanInstance(&instance) != VK_SUCCESS) {
    printf("Failed to create instance\n");
    return 1;
  }

  CleanupArgs args = {
      .instance = instance,
  };

  cleanup(&args);
  return 0;
}
