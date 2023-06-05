#ifndef PHYSICALDEVICE_H
#define PHYSICALDEVICE_H

#include <ext.h>

typedef struct {
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR *pFormats;
  uint32_t formatsLength;
  VkPresentModeKHR *pPresentModes;
  uint32_t presentModesLength;
} SwapchainSupportDetails;

SwapchainSupportDetails querySwapchainSupport(const VkPhysicalDevice *pDevice,
                                              const VkSurfaceKHR *pSurface);

void cleanupSwapchainSupportDetails(SwapchainSupportDetails *pDetails);

typedef struct {
  VkInstance *pInstance;
  VkSurfaceKHR *pSurface;
  char **ppDeviceExtensions;
  uint32_t deviceExtensionCount;
} PhysicalDeviceArgs;

int pickPhysicalDevice(PhysicalDeviceArgs *pArgs, VkPhysicalDevice *outDevice);

typedef struct {
  uint32_t graphicsFamily;
  uint32_t isGraphicsFamilyFound;
  uint32_t presentFamily;
  uint32_t isPresentFamilyFound;
} QueueFamilyIndices;

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice *pDevice,
                                     const VkSurfaceKHR *pSurface);

#endif // PHYSICALDEVICE_H
