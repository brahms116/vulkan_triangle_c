#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H
#include <ext.h>

typedef struct {
  const VkDevice *pDevice;
  const VkPhysicalDevice *pPhysicalDevice;
  const VkSurfaceKHR *pSurface;
  GLFWwindow *pWindow;
} SwapchainArgs;

int createSwapchain(SwapchainArgs *pArgs, VkSwapchainKHR *outSwapchain,
                    VkExtent2D *outExtent, VkFormat *outImageFormat);

typedef struct {
  int swapchainImageViewsCount;
  const VkDevice *pDevice;
  const VkSwapchainKHR *pSwapchain;
  const VkImage *pSwapchainImages;
  const VkFormat *pImageFormat;
} ImageViewArgs;

int createSwapchainImageViews(ImageViewArgs *pArgs,
                              VkImageView *outSwapchainImageViews);

typedef struct {
  VkFramebuffer *pFramebuffers;
  VkImageView *pSwapchainImageViews;
  VkSwapchainKHR *pSwapchain;
  VkDevice *pDevice;
  int swapchainImageCount;
} CleanUpSwapchainArgs;

int cleanupSwapchain(CleanUpSwapchainArgs *pArgs);

// Cleanup swapchain
// 1. The framebuffers
// 2. The image views
// 3. The swapchain

// Recreate the swapchain and the 3 resources cleaned up above
//
// To recreate the swapchain, we wait until the device is idle
//
// After we aquire the next image, we check if the swapchain is out of date
//
// After we vkQueuePresent, we check if the swapchain is out of date or suboptimal
//
// Make sure if we return from the draw frame function that we don't reset the fence if no
// was submitted.
//
// Use GLFW set framebuffer size callback to set a flag to recreate the swapchain
//
// Use GLFW wait events if the window is minimized; height or width is 0

#endif // SWAPCHAIN_H
