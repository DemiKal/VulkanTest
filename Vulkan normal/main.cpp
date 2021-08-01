#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>
#include <iostream>

#include "Camera.h"


int main()
{
	std::cout << "Guten tag welt";
	Camera camera;
	camera.setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
	camera.setRotation(glm::vec3(0.0f));
	camera.setPerspective(60.0f, (float)1024 / (float)768, 1.0f, 256.0f);
	return 1;
}
