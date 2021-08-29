#pragma once
#include "vulkanStructs.h"

struct GLFWwindow;

class VulkanHPP
{
public:
	void RunLoop();
	void Update(float deltaTime);
	void RenderTriangle(Context& context, uint32_t swapchain_index);

	void Prepare();
	void InitLogger();
	void InitVertices(Context& m_Context);
	void InitWindow();
	void InitInstance(Context& context, const std::vector<const char*>& required_instance_extensions, const std::vector<const char*>& required_validation_layers);
	void SelectPhysicalDeviceAndInstance(Context& context);
	void InitDevice(Context& context, const std::vector<const char*>& required_device_extensions);
	void InitSwapchain(Context& context);
	void InitRenderPass(Context& context);
	void InitPipeline(Context& context);
	void InitFrameBuffers(Context& context);
	void InitAllocator(Context& context);

	vk::SurfaceKHR CreateSurface(vk::Instance instance, vk::PhysicalDevice);
	bool ValidateExtensions(const std::vector<const char*>& required, const std::vector<vk::ExtensionProperties>& available);
	void TeardownPerFrame(Context& context, PerFrame& per_frame);
	void InitPerFrame(Context& context, PerFrame& per_frame);
	void Resize(const uint32_t, const uint32_t);
	void TearDownFramebuffers(Context& context);
	vk::Result PresentImage(Context& context, uint32_t index);
	vk::Result AcquireNextImage(Context& context, uint32_t* image);

 
	Context m_Context;
	
	 

	GLFWwindow* m_GLFWwindow;
	//VmaAllocator m_Allocator;
	int m_Width = 1024;
	int m_Height = 768;
};

bool ValidateLayers(const std::vector<const char*>& required, const std::vector<vk::LayerProperties>& available);
