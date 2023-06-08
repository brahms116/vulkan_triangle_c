#ifndef DEVICE_H
#define DEVICE_H
#include <ext.h>

typedef struct {
  const VkPhysicalDevice *pPhysicalDevice;
  const char **ppPhysicalDeviceExtensions;
  uint32_t physicalDeviceExtensionCount;
  const VkSurfaceKHR *pSurface;
} DeviceArgs;

int createDevice(DeviceArgs *pArgs, VkDevice *outDevice,
                 VkQueue *outGraphicsQueue, VkQueue *outPresentQueue);

#endif // DEVICE_H
