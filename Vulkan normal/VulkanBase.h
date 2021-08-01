#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "VulkanSwapChain.h"


namespace vks {
	struct VulkanDevice;
}

class VulkanBase
{
	std::vector<std::string> supportedInstanceExtensions;
	std::vector<const char*> enabledInstanceExtensions;
	// Vulkan instance
	bool validate = false;
	VkInstance instance;
	VkResult CreateInstance(std::string name);
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	vks::VulkanDevice* vulkanDevice;
	VkDevice device;
	VkPhysicalDeviceFeatures enabledFeatures{};
	void* deviceCreatepNextChain = nullptr;
	std::vector<const char*> enabledDeviceExtensions;
	VulkanSwapChain swapChain;
	VkQueue queue;
	VkFormat depthFormat;
	VkSubmitInfo submitInfo;
	VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	struct {
		// Swap chain image presentation
		VkSemaphore presentComplete;
		// Command buffer submission and execution
		VkSemaphore renderComplete;
	} semaphores;

	
	bool InitVulkan(std::string name);


public:
	VulkanBase(std::string name);
};




