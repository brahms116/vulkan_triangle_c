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

#endif // SWAPCHAIN_H
