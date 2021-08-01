#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define VK_USE_PLATFORM_WIN32_KHR


#include <iostream>

#include "VulkanBase.h"
#include "Camera.h"


int main()
{
	std::cout << "Guten tag welt";
	Camera camera;
	camera.setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
	camera.setRotation(glm::vec3(0.0f));
	camera.setPerspective(60.0f, (float)1024 / (float)768, 1.0f, 256.0f);

    VulkanBase vulkanBase = VulkanBase{ "myengine" };
	return 1;
}
