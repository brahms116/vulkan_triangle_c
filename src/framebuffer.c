#include "vulkan/vulkan_core.h"
#include <framebuffer.h>
#include <stdio.h>

int createFramebuffers(FramebufferArgs *pArgs, VkFramebuffer *outFramebuffers) {
  for (int i = 0; i < pArgs->imageViewsCount; i++) {
    VkImageView pAttachments[] = {pArgs->pImageViews[i]};

    VkFramebufferCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = *pArgs->pRenderPass,
        .attachmentCount = 1,
        .pAttachments = pAttachments,
        .width = pArgs->pExtent->width,
        .height = pArgs->pExtent->height,
        .layers = 1};

    if (vkCreateFramebuffer(*pArgs->pDevice, &createInfo, NULL,
                            &outFramebuffers[i]) != VK_SUCCESS) {
      fprintf(stderr, "Failed to create framebuffer %i\n", i);
      return 1;
    }
  }

  return 0;
}
