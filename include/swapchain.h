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
  VkFormat *pImageFormat;
} ImageViewArgs;

int createSwapchainImageViews(ImageViewArgs *pArgs,
                              VkImageView *outSwapchainImageViews);

#endif // SWAPCHAIN_H
