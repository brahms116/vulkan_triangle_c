#include "vulkan/vulkan_core.h"
#include <physicalDevice.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int checkPhysicalDeviceExtensionSupport(const VkPhysicalDevice *pPhsyicalDevice,
                                        const char **ppPhsyicalDeviceExtensions,
                                        const uint32_t physicalExtensionCount) {

  // Query the number of extensions supported by the physical device
  uint32_t supportedExtensionCount = 0;
  vkEnumerateDeviceExtensionProperties(*pPhsyicalDevice, NULL,
                                       &supportedExtensionCount, NULL);

  // If there are no extensions supported by the physical device, return 0
  if (supportedExtensionCount == 0) {
    return 0;
  }

  VkExtensionProperties *pProperties = (VkExtensionProperties *)malloc(
      sizeof(VkExtensionProperties) * supportedExtensionCount);

  vkEnumerateDeviceExtensionProperties(*pPhsyicalDevice, NULL,
                                       &supportedExtensionCount, pProperties);

  // Loop through all the required extensions and check if they are supported
  for (int i = 0; i < physicalExtensionCount; i++) {
    const char *pDesiredExtension = ppPhsyicalDeviceExtensions[i];
    int extensionFound = 0;

    // For each desired extension, loop through the supported extensions and
    // check if it is supported
    for (int k = 0; k < supportedExtensionCount; k++) {
      // If it is supported, break the loop and set the flag
      if (strcmp(pDesiredExtension, pProperties[k].extensionName) == 0) {
        extensionFound = 1;
        break;
      }
    }

    // If all the supported extensions have been checked and the desired
    // extension is not found, return 0
    if (!extensionFound) {
      free(pProperties);
      return 0;
    }

    // Otherwise, continue to the next desired extension
  }

  free(pProperties);
  return 1;
}

int isPhysicalDeviceSuitable(const VkPhysicalDevice *pPhysicalDevice,
                             const char **ppPhsyicalDeviceExtensions,
                             const uint32_t physicalExtensionCount,
                             const VkSurfaceKHR *pSurface) {

  // Query for present and graphics queue support
  QueueFamilyIndices indices = findQueueFamilies(pPhysicalDevice, pSurface);

  int queuesPresent =
      indices.isGraphicsFamilyFound && indices.isPresentFamilyFound;

  if (!queuesPresent) {
    return 0;
  }

  // Check swapchainSupport
  SwapchainSupportDetails details =
      querySwapchainSupport(pPhysicalDevice, pSurface);

  int swapchainAdequate = details.formatsLength && details.presentModesLength;

  if (!swapchainAdequate) {
    return 0;
  }

  // Check for extensions support

  int extensionsSupported = checkPhysicalDeviceExtensionSupport(
      pPhysicalDevice, ppPhsyicalDeviceExtensions, physicalExtensionCount);

  if (!extensionsSupported) {
    return 0;
  }

  return 1;
}

void cleanUpSwapchainSupportDetails(SwapchainSupportDetails *pDetails) {
  if (pDetails == NULL) {
    return;
  }

  free(pDetails->pFormats);
  free(pDetails->pPresentModes);
}

SwapchainSupportDetails
querySwapchainSupport(const VkPhysicalDevice *pPhysicalDevice,
                      const VkSurfaceKHR *pSurface) {

  SwapchainSupportDetails details;

  // Query the surface capabilities from the physical device
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*pPhysicalDevice, *pSurface,
                                            &details.capabilities);

  // Query the surface formats from the physical device
  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(*pPhysicalDevice, *pSurface,
                                       &formatCount, NULL);

  details.formatsLength = formatCount;

  if (formatCount == 0) {
    details.pFormats = NULL;
  } else {
    details.pFormats = malloc(formatCount * sizeof(*details.pFormats));
    vkGetPhysicalDeviceSurfaceFormatsKHR(*pPhysicalDevice, *pSurface,
                                         &formatCount, details.pFormats);
  }

  // Query the surface present modes from the physical device
  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(*pPhysicalDevice, *pSurface,
                                            &presentModeCount, NULL);

  details.presentModesLength = presentModeCount;

  if (presentModeCount == 0) {
    details.pPresentModes = NULL;
  } else {
    details.pPresentModes =
        malloc(presentModeCount * sizeof(*details.pPresentModes));
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        *pPhysicalDevice, *pSurface, &presentModeCount, details.pPresentModes);
  }

  return details;
}

int pickPhysicalDevice(PhysicalDeviceArgs *pArgs, VkPhysicalDevice *outDevice) {

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(*pArgs->pInstance, &deviceCount, NULL);

  if (deviceCount == 0) {
    return 1;
  }

  VkPhysicalDevice *pDevices = malloc(deviceCount * sizeof(VkPhysicalDevice));

  vkEnumeratePhysicalDevices(*pArgs->pInstance, &deviceCount, pDevices);

  for (int i = 0; i < deviceCount; i++) {
    if (isPhysicalDeviceSuitable(&pDevices[i], pArgs->ppDeviceExtensions,
                                 pArgs->deviceExtensionCount,
                                 pArgs->pSurface)) {
      *outDevice = pDevices[i];
      free(pDevices);
      return 0;
    }
  }

  // No physical devices were suitable
  free(pDevices);
  fprintf(stderr, "No suitable physical devices found\n");
  return 1;
}

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice *pDevice,
                                     const VkSurfaceKHR *pSurface) {
  // Setup result struct
  QueueFamilyIndices indices;
  indices.isGraphicsFamilyFound = 0;
  indices.isPresentFamilyFound = 0;

  // Get queue family properties
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(*pDevice, &queueFamilyCount, NULL);

  VkQueueFamilyProperties *pQueueFamilies =
      malloc(queueFamilyCount * sizeof(*pQueueFamilies));
  vkGetPhysicalDeviceQueueFamilyProperties(*pDevice, &queueFamilyCount,
                                           pQueueFamilies);

  // Loop through queue families properties and check
  for (int i = 0; i < queueFamilyCount; i++) {
    // Check if it has graphics support
    if (pQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
      indices.isGraphicsFamilyFound = 1;
    }

    // Check if it has present support
    VkBool32 presentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(*pDevice, i, *pSurface,
                                         &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
      indices.isPresentFamilyFound = 1;
    }
  }

  free(pQueueFamilies);
  return indices;
}
