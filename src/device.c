#include <device.h>
#include <physicalDevice.h>
#include <stdio.h>

// Creates a logical device and retrieves the graphics and present queues handles
int createDevice(DeviceArgs *args, VkDevice *outDevice, VkQueue *outGraphicsQueue, VkQueue *outPresentQueue) {
  // Setup queue create info
  QueueFamilyIndices indices = findQueueFamilies(args->pPhysicalDevice, args->pSurface);
  float queuePriority = 1.0f;

  VkDeviceQueueCreateInfo graphicsQueuecreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .queueFamilyIndex = indices.graphicsFamily,
    .queueCount = 1,
    .pQueuePriorities = &queuePriority
  };

  VkDeviceQueueCreateInfo presentQueueCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .queueFamilyIndex = indices.presentFamily,
    .queueCount = 1,
    .pQueuePriorities = &queuePriority
  };

  VkDeviceQueueCreateInfo queueCreateInfos[] = {graphicsQueuecreateInfo, presentQueueCreateInfo};

  // The physical device features needed
  VkPhysicalDeviceFeatures physicalDeviceFeatures = {
    .sparseResidencyAliased = VK_FALSE
  };

 // Pass the info created above to the create info struct for the device 
  VkDeviceCreateInfo createInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .queueCreateInfoCount = 2,
    .pQueueCreateInfos = queueCreateInfos,
    .enabledExtensionCount = args->physicalDeviceExtensionCount,
    .ppEnabledExtensionNames = args->ppPhysicalDeviceExtensions,
    .pEnabledFeatures = &physicalDeviceFeatures
  };

  // Create the device
  if (vkCreateDevice(*args->pPhysicalDevice, &createInfo, NULL, outDevice) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create logical device with VK_SUCCESS");
    return 1;
  }

  // Retreive the queue handles from the newly formed device
  vkGetDeviceQueue(*outDevice, indices.graphicsFamily, 0, outGraphicsQueue);
  vkGetDeviceQueue(*outDevice, indices.presentFamily, 0, outPresentQueue);

  return 0;
}
