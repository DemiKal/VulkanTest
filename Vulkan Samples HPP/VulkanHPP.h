#pragma once
#include "vulkanStructs.h"

struct GLFWwindow;

class VulkanHPP
{
public:
	void Prepare();

	void InitWindow();

	vk::SurfaceKHR CreateSurface(vk::Instance instance, vk::PhysicalDevice);

	void SelectPhysicalDeviceAndInstance(Context context);


private:
	void InitInstance(Context& context, const std::vector<const char*>& required_instance_extensions, const std::vector<const char*>& required_validation_layers);
	bool ValidateExtensions(const std::vector<const char*>& required, const std::vector<vk::ExtensionProperties>& available);



	Context m_Context;
	GLFWwindow* m_GLFWwindow;
};

bool ValidateLayers(const std::vector<const char*>& required, const std::vector<vk::LayerProperties>& available);
