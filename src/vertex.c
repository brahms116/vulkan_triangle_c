#include "vulkan/vulkan_core.h"
#include <stdio.h>
#include <vertex.h>

int createVertexBuffer(VertexBufferArgs *pArgs, VkBuffer *outVertexBuffer,
                       VkDeviceMemory *outVertexBufferMemory) {

  VkBufferCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = *pArgs->pSize,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  };

  if (vkCreateBuffer(*pArgs->pDevice, &createInfo, NULL, outVertexBuffer) !=
      VK_SUCCESS) {

    fprintf(stderr, "Failed to create vertex buffer with VK_SUCCESS\n");
    return 1;
  }

  return 0;

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(*pArgs->pDevice, *outVertexBuffer,
                                &memRequirements);
}


void cleanupVertexBuffer(CleanupVertexBufferArgs *pArgs) {
  vkDestroyBuffer(*pArgs->pDevice, *pArgs->pVertexBuffer, NULL);
}
