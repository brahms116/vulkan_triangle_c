#include "vulkan/vulkan_core.h"
#include <framebuffer.h>
#include <stdio.h>
#include <stdlib.h>
#include <swapchain.h>

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

FramebufferAndImages
createFramebuffersAndImages(FramebufferAndImagesArgs *pArgs) {
  uint32_t swapchainImageCount = 0;

  FramebufferAndImages failure = {.success = 0};

  vkGetSwapchainImagesKHR(*pArgs->pDevice, *pArgs->pSwapchain,
                          &swapchainImageCount, NULL);


  VkImage *pSwapchainImages = malloc(sizeof(VkImage) * swapchainImageCount);

  vkGetSwapchainImagesKHR(*pArgs->pDevice, *pArgs->pSwapchain,
                          &swapchainImageCount, pSwapchainImages);

  VkImageView *pImageViews = malloc(sizeof(VkImageView) * swapchainImageCount);

  ImageViewArgs imageViewArgs = {.pDevice = pArgs->pDevice,
                                 .pSwapchainImages = pSwapchainImages,
                                 .swapchainImageViewsCount =
                                     swapchainImageCount,
                                 .pImageFormat = pArgs->pSwapchainFormat};

  if (createSwapchainImageViews(&imageViewArgs, pImageViews)) {
    fprintf(stderr, "Failed to create image views\n");
    return failure;
  }

  VkFramebuffer *pFramebuffers =
      malloc(sizeof(VkFramebuffer) * swapchainImageCount);

  FramebufferArgs framebufferArgs = {
    .pDevice = pArgs->pDevice,
    .pRenderPass = pArgs->pRenderPass,
    .pImageViews = pImageViews,
    .imageViewsCount = swapchainImageCount,
    .pExtent = pArgs->pSwapchainExtent
  };

  if (createFramebuffers(&framebufferArgs, pFramebuffers)) {
    fprintf(stderr, "Failed to create framebuffers\n");
    return failure;
  }

  FramebufferAndImages success = {
    .success = 1,
    .pFramebuffers = pFramebuffers,
    .pImageViews = pImageViews,
    .swapchainImageCount = swapchainImageCount,
    .pImages = pSwapchainImages
  };

  return success;
}

void cleanupFramebuffersAndImages(CleanupFramebufferAndImagesArgs *pArgs) {
  for (int i = 0; i < pArgs->swapchainImageCount; i++) {
    vkDestroyImageView(*pArgs->pDevice, pArgs->pImageViews[i], NULL);
    vkDestroyFramebuffer(*pArgs->pDevice, pArgs->pFramebuffers[i], NULL);
  }


  free(pArgs->pImages);
  free(pArgs->pImageViews);
  free(pArgs->pFramebuffers);
}
