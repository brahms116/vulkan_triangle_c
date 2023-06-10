#include "vulkan/vulkan_core.h"
#include <physicalDevice.h>
#include <stdio.h>
#include <swapchain.h>

// Look for VK_FORMAT_B8G8R8A8_SRGB and VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
VkSurfaceFormatKHR
chooseSwapchainSurfaceFormat(const VkSurfaceFormatKHR *pFormats,
                             uint32_t formatsLength) {
  for (int i = 0; i < formatsLength; i++) {
    const VkSurfaceFormatKHR format = pFormats[i];
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }
  return pFormats[0];
}

// Look for VK_PRESENT_MODE_MAILBOX_KHR or VK_PRESENT_MODE_FIFO_KHR
VkPresentModeKHR
chooseSwapchainPresentMode(const VkPresentModeKHR *pPresentModes,
                           uint32_t presentModesLength) {
  for (int i = 0; i < presentModesLength; i++) {
    if (pPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      return pPresentModes[i];
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
chooseSwapchainExtent(GLFWwindow *pWindow,
                      const VkSurfaceCapabilitiesKHR *pCapabilities) {
  // If the currentExtent is not 0xFFFFFF, then the extent is already defined
  // by the surface capabilities queried from the physical device and the
  // surface
  if (pCapabilities->currentExtent.width != 0xFFFFFF) {
    return pCapabilities->currentExtent;
  } else {
    // Otherwise, we need to define the extent ourselves

    // Get window size from glfw
    int width, height;
    glfwGetFramebufferSize(pWindow, &width, &height);

    // Setting them as defaults here
    VkExtent2D actualExtent = {
        .width = (uint32_t)width,
        .height = (uint32_t)height,
    };

    uint32_t actualWidth;
    uint32_t actualHeight;

    // Clamp width to the capabilities max and min
    if (actualExtent.width < pCapabilities->minImageExtent.width) {
      actualWidth = pCapabilities->minImageExtent.width;
    } else if (pCapabilities->maxImageExtent.width < actualExtent.width) {
      actualWidth = pCapabilities->maxImageExtent.width;
    } else {
      actualWidth = actualExtent.width;
    }

    actualExtent.width = actualWidth;

    // Clamp height to the capabilities max and min
    if (actualExtent.height < pCapabilities->minImageExtent.height) {
      actualHeight = pCapabilities->minImageExtent.height;
    } else if (pCapabilities->maxImageExtent.height < actualExtent.height) {
      actualHeight = pCapabilities->maxImageExtent.height;
    } else {
      actualHeight = actualExtent.height;
    }

    actualExtent.height = actualHeight;
    return actualExtent;
  }
}

int createSwapchain(SwapchainArgs *pArgs, VkSwapchainKHR *outSwapchain,
                    VkExtent2D *outExtent, VkFormat *outImageFormat) {

  // Get the swapchain support details
  SwapchainSupportDetails swapchainSupportDetails =
      querySwapchainSupport(pArgs->pPhysicalDevice, pArgs->pSurface);

  // Get the desired surface format, present mode, and extent
  VkSurfaceFormatKHR surfaceFormat = chooseSwapchainSurfaceFormat(
      swapchainSupportDetails.pFormats, swapchainSupportDetails.formatsLength);

  VkPresentModeKHR presentMode =
      chooseSwapchainPresentMode(swapchainSupportDetails.pPresentModes,
                                 swapchainSupportDetails.presentModesLength);

  VkExtent2D extent = chooseSwapchainExtent(
      pArgs->pWindow, &swapchainSupportDetails.capabilities);

  cleanupSwapchainSupportDetails(&swapchainSupportDetails);

  // Get the number of images to use in the swapchain
  uint32_t imageCount = swapchainSupportDetails.capabilities.minImageCount + 1;

  // 0 means that there is no maximum image count
  if (swapchainSupportDetails.capabilities.maxImageCount > 0 &&
      imageCount > swapchainSupportDetails.capabilities.maxImageCount) {
    imageCount = swapchainSupportDetails.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = *pArgs->pSurface,
      .minImageCount = imageCount,
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .preTransform = swapchainSupportDetails.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = presentMode,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
  };

  QueueFamilyIndices indices = findQueueFamilies(pArgs->pPhysicalDevice, pArgs->pSurface);

  uint32_t pQueueFamilyIndices[2] = {indices.graphicsFamily,
                                    indices.presentFamily};

  // If the graphics and present queues are different, then we need to use
  // concurrent mode to avoid ownership transfers for now.
  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = pQueueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
  }

  if (vkCreateSwapchainKHR(*pArgs->pDevice, &createInfo, NULL, outSwapchain) !=
      VK_SUCCESS) {
    fprintf(stderr, "Failed to create swapchain with VK_SUCCESS\n");
    return 0;
  }

  *outImageFormat = surfaceFormat.format;
  *outExtent = extent;

  return 0;
}

int createSwapchainImageViews(ImageViewArgs *pArgs, VkImageView *outSwapchainImageViews){
  VkComponentMapping components = {
    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
  };

  VkImageSubresourceRange subresourceRange = {
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .baseMipLevel = 0,
    .levelCount = 1,
    .baseArrayLayer = 0,
    .layerCount = 1,
  };


  for (int i = 0; i < pArgs->swapchainImageViewsCount; i++) {
    VkImageViewCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = pArgs->pSwapchainImages[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = *pArgs->pImageFormat,
        .components = components,
        .subresourceRange = subresourceRange,
    };

    if (vkCreateImageView(*pArgs->pDevice, &createInfo, NULL,
                          &outSwapchainImageViews[i]) != VK_SUCCESS) {
      fprintf(stderr, "Failed to create image view\n");
      return 1;
    }
  }
  return 0;
}
