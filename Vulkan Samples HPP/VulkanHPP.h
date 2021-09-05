#pragma once
#include "vulkanStructs.h"
 #include "VertexBuffer.h"

struct GLFWwindow;



class VulkanHPP
{
public:
	void RunLoop();
	void Update(float deltaTime);
	void RenderTriangle(Context& context, uint32_t swapchain_index);

	void Prepare();
	void InitLogger();
	void InitWindow();
	void InitInstance(Context& context, const std::vector<const char*>& required_instance_extensions, const std::vector<const char*>& required_validation_layers);
	void SelectPhysicalDeviceAndInstance(Context& context);
	void InitDevice(Context& context, const std::vector<const char*>& required_device_extensions);
	void InitSwapchain(Context& context);
	void InitRenderPass(Context& context);
	void InitPipeline(Context& context);
	void InitFrameBuffers(Context& context);
	void InitAllocator(Context& context);
	void InitVertices(Context& context);
	void InitUniformBuffer(Context& context);
	void InitDescriptorSetLayout(Context& context);
	vk::SurfaceKHR CreateSurface(vk::Instance instance, vk::PhysicalDevice);
	bool ValidateExtensions(const std::vector<const char*>& required, const std::vector<vk::ExtensionProperties>& available);
	void TeardownPerFrame(Context& context, PerFrame& per_frame);
	void InitPerFrame(Context& context, PerFrame& per_frame);
	void Resize(const uint32_t, const uint32_t);
	void TearDownFramebuffers(Context& context);
	vk::Result PresentImage(Context& context, uint32_t index);
	vk::Result AcquireNextImage(Context& context, uint32_t* image);
	void SetupDescriptorSet(Context& context);
	void InitDescriptorPool(Context& context);
	//void StageBuffer(Context& context, Buffer& allocBuffer, vk::BufferUsageFlagBits usageFlags, VmaMemoryUsage memoryUsage);
	void StageBuffer(Context& context, Buffer& allocBuffer, vk::BufferUsageFlagBits usageFlags, VmaMemoryUsage memoryUsage);
	void  UpdateUniformBuffer(float dt);

	VmaAllocator m_Allocator;

	UniformBuffer* m_Uniform;//= UniformBuffer
	GLFWwindow* m_GLFWwindow;
	 
	int m_Width = 1024;
	int m_Height = 768;
	Context m_Context;
};

bool ValidateLayers(const std::vector<const char*>& required, const std::vector<vk::LayerProperties>& available);
