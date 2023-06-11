#ifndef SHADER_H
#define SHADER_H
#include <ext.h>

typedef struct {
  long int contentLength;
  char *pContent;
} FileContents;

FileContents readFile(const char *pFilePath);

typedef struct {
  const VkDevice *pDevice;
  const char *pCode;
  size_t codeSize;
} ShaderModuleArgs;

int createShaderModule(ShaderModuleArgs *pArgs, VkShaderModule *outShaderModule);

#endif // SHADER_H
