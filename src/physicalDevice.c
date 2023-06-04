#include <physicalDevice.h>
#include <stdlib.h>

void cleanUpSwapchainSupportDetails(SwapchainSupportDetails *pDetails) {
  if (pDetails == NULL) {
    return;
  }

  free(pDetails->pFormats);
  free(pDetails->pPresentModes);
}

SwapchainSupportDetails
querySupportchainSupport(const VkPhysicalDevice *pDevice,
                         const VkSurfaceKHR *pSurface) {

}

int pickPhysicalDevice(PhysicalDeviceArgs *pArgs, VkPhysicalDevice *outDevice) {

  return 0;
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice *pDevice,
                                     VkSurfaceKHR *pSurface) {
  // Setup result struct
  QueueFamilyIndices indices;
  indices.isGraphicsFamilyFound = 0;
  indices.isPresentFamilyFound = 0;

  // Get queue family properties
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(*pDevice, &queueFamilyCount, NULL);

  VkQueueFamilyProperties *queueFamilies =
      malloc(queueFamilyCount * sizeof(*queueFamilies));
  vkGetPhysicalDeviceQueueFamilyProperties(*pDevice, &queueFamilyCount,
                                           queueFamilies);

  // Loop through queue families properties and check
  for (int i = 0; i < queueFamilyCount; i++) {
    // Check if it has graphics support
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
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

  free(queueFamilies);
  return indices;
}
