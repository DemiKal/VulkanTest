#pragma once
#include "vulkanStructs.h"

struct GLFWwindow;

class VulkanHPP
{
public:
	void RunLoop();
	void Update(float deltaTime);
	vk::Result PresentImage(Context& context, uint32_t index);
	void RenderTriangle(Context& context, uint32_t swapchain_index);
	vk::Result AcquireNextImage(Context& context, uint32_t* image);
	void Prepare();

	void InitFrameBuffers(Context& context);

	void InitPipeline(Context& context);

	void InitRenderPass(Context& context);

	void InitSwapchain(Context& context);

	void InitDevice(Context& context, const std::vector<const char*>& required_device_extensions);

	void InitWindow();

	vk::SurfaceKHR CreateSurface(vk::Instance instance, vk::PhysicalDevice);

	void SelectPhysicalDeviceAndInstance(Context& context);


private:
	void InitInstance(Context& context, const std::vector<const char*>& required_instance_extensions, const std::vector<const char*>& required_validation_layers);
	bool ValidateExtensions(const std::vector<const char*>& required, const std::vector<vk::ExtensionProperties>& available);

	void TeardownPerFrame(Context& context, PerFrame& per_frame);

	void InitPerFrame(Context& context, PerFrame& per_frame);

 


	Context m_Context;
	GLFWwindow* m_GLFWwindow;
	int m_Width = 1024;
	int m_Height = 768;
};

bool ValidateLayers(const std::vector<const char*>& required, const std::vector<vk::LayerProperties>& available);
