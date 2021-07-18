#pragma once

//#define VK_USE_PLATFORM_WIN32_KHR


//#include <vulkan/vulkan.h>
#include <deque>
#include <functional>



#include <Include/vulkan/vulkan.h>
#include <vk_mem_alloc.h>

struct GLFWwindow;

class PipelineBuilder {
public:

	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineLayout _pipelineLayout;

	VkPipeline BuildPipeline(VkDevice device, VkRenderPass pass);
};

struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //call functors
		}

		deletors.clear();
	}
};
struct AllocatedImage {
	VkImage _image;
	VmaAllocation _allocation;
};
class VulkanEngine
{
public:
	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debug_messenger;
	VkPhysicalDevice _chosenGPU;
	VkDevice _device;
	DeletionQueue _mainDeletionQueue;
	uint32_t _graphicsQueueFamily;
	VkSurfaceKHR _surface;
	VkQueue _graphicsQueue;
	VkExtent2D _windowExtent{ 1024 , 768 };
	VkSwapchainKHR _swapchain;
	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
	VkFormat _swapchainImageFormat;
	VkFormat _depthFormat;
	VmaAllocator _allocator;
	VkRenderPass _renderPass;
	std::vector<VkFramebuffer> _framebuffers;
	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;
	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;
	VkPipelineLayout _trianglePipelineLayout;
	VkPipeline _trianglePipeline;
	VkPipeline _redTrianglePipeline;
	//depth resources
	VkImageView _depthImageView;
	AllocatedImage _depthImage;
	bool _isInitialized = false;
	GLFWwindow* _window;
	int _frameNumber;
	int _selectedShader;

	void Init()
	{
		InitVulkan();
		InitSwapchain();
		DefaultRenderpass();
		InitFramebuffers();
		InitCommands();
		InitSyncStructures();
		InitPipelines();
		_isInitialized = true;
	}

	void InitSwapchain();
	void InitVulkan();
	void DefaultRenderpass();
	void InitFramebuffers();
	void InitCommands();
	void InitSyncStructures();
	void InitPipelines();
	void Run();
	void Draw();

	bool LoadShaderModule(const char* filePath, VkShaderModule* outShaderModule);
	void draw();
};
