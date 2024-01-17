#pragma once
#pragma once
#define VMA_VULKAN_VERSION 1002000 // Vulkan 1.2
#include <vma/vk_mem_alloc.h>
#include <GLFW/glfw3.h>

#define stopExecute(message) {\
std::cout << (message);   \
__debugbreak(); \
while (true);   \
}

#define ASSERT(x) if(!(x)) __debugbreak();
/*
* in "if" set in {GLCall()}
*/
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__)) 




