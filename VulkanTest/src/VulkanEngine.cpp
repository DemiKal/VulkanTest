#define GLFW_INCLUDE_VULKAN
#define VMA_IMPLEMENTATION


#include "VulkanEngine.h"

//#include <vk_mem_alloc.h>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>


  void VulkanEngine::InitSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU,_device,_surface };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.use_default_format_selection()
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(_windowExtent.width, _windowExtent.height)
		.build()
		.value();

	//store swapchain and its related images
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();

	_swapchainImageFormat = vkbSwapchain.image_format;

	_mainDeletionQueue.push_function([=]() {
		vkDestroySwapchainKHR(_device, _swapchain, nullptr);
		});

	//depth image size will match the window
	VkExtent3D depthImageExtent = {
		_windowExtent.width,
		_windowExtent.height,
		1
	};

	//hardcoding the depth format to 32 bit float
	_depthFormat = VK_FORMAT_D32_SFLOAT;

	//the depth image will be a image with the format we selected and Depth Attachment usage flag
	VkImageCreateInfo dimg_info = ImageCreateInfo(_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

	//for the depth image, we want to allocate it from gpu local memory
	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//allocate and create the image
	vmaCreateImage(_allocator, &dimg_info, &dimg_allocinfo, &_depthImage._image, &_depthImage._allocation, nullptr);

	//build a image-view for the depth image to use for rendering
	VkImageViewCreateInfo dview_info = ImageviewCreateInfo(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);;



	VkResult err = vkCreateImageView(_device, &dview_info, nullptr, &_depthImageView);                                           \
		if (err)
		{
			std::cout << "Detected Vulkan error: " << err << std::endl;
			abort();
		}


	//add to deletion queues
	_mainDeletionQueue.push_function([=]() {
		vkDestroyImageView(_device, _depthImageView, nullptr);
		vmaDestroyImage(_allocator, _depthImage._image, _depthImage._allocation);
		});
}

    void VulkanEngine::InitVulkan()
  {
	  std::cout << "test\n";

	  if (!glfwInit())
	  {
		  throw std::exception("GLFW not initialized!");
	  }

	  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	  int SAMPLE_WIDTH = 1024;
	  int SAMPLE_HEIGHT = 768;
	  std::string PROJECT_NAME = "banana peeler";
	  GLFWwindow* window = glfwCreateWindow(SAMPLE_WIDTH, SAMPLE_HEIGHT, PROJECT_NAME.c_str(), nullptr, nullptr);
	  if (!glfwVulkanSupported())
	  {
		  printf("GLFW: Vulkan Not Supported\n");
		  throw std::exception("GLFW: Vulkan Not Supported");
	  }

	  assert(glfwVulkanSupported() == 1);

	  vkb::InstanceBuilder builder;
	  auto inst_ret = builder.set_app_name("Example Vulkan Application")
		  .request_validation_layers(true)
		  .use_default_debug_messenger()
		  .require_api_version(1, 1, 0)
		  .build();

	  vkb::Instance vkb_inst = inst_ret.value();

	  _instance = vkb_inst.instance;
	  _debug_messenger = vkb_inst.debug_messenger;

	  //glfw surface?
	  VkSurfaceKHR  surface;
	  VkResult err = glfwCreateWindowSurface(static_cast<VkInstance>(_instance), window, nullptr, &surface);

	  if (err != VK_SUCCESS)
		  throw std::runtime_error("Failed to create window!");
	  _surface = VkSurfaceKHR(surface);

	  vkb::PhysicalDeviceSelector selector{ vkb_inst };
	  vkb::PhysicalDevice physicalDevice = selector
		  .set_minimum_version(1, 1)
		  .set_surface(surface)
		  .select()
		  .value();

	  vkb::DeviceBuilder deviceBuilder{ physicalDevice };

	  vkb::Device vkbDevice = deviceBuilder.build().value();

	  // Get the VkDevice handle used in the rest of a vulkan application
	  _device = vkbDevice.device;
	  _chosenGPU = physicalDevice.physical_device;

	  // use vkbootstrap to get a Graphics queue
	  _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();

	  _graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	  //initialize the memory allocator
	  VmaAllocatorCreateInfo allocatorInfo = {};
	  allocatorInfo.physicalDevice = _chosenGPU;
	  allocatorInfo.device = _device;
	  allocatorInfo.instance = _instance;
	  vmaCreateAllocator(&allocatorInfo, &_allocator);



	  _mainDeletionQueue.push_function([&]() {
		  vmaDestroyAllocator(_allocator);
		  });

	  VkPhysicalDeviceProperties _gpuProperties;
	  vkGetPhysicalDeviceProperties(_chosenGPU, &_gpuProperties);

	  std::cout << "The gpu has a minimum buffer alignement of " << _gpuProperties.limits.minUniformBufferOffsetAlignment << std::endl;
  }

	  VkImageCreateInfo VulkanEngine::ImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent)
	{
		VkImageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.pNext = nullptr;

		info.imageType = VK_IMAGE_TYPE_2D;

		info.format = format;
		info.extent = extent;

		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = usageFlags;

		return info;
	}

	    VkImageViewCreateInfo VulkanEngine::ImageviewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
	  {
		  //build a image-view for the depth image to use for rendering
		  VkImageViewCreateInfo info = {};
		  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		  info.pNext = nullptr;

		  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		  info.image = image;
		  info.format = format;
		  info.subresourceRange.baseMipLevel = 0;
		  info.subresourceRange.levelCount = 1;
		  info.subresourceRange.baseArrayLayer = 0;
		  info.subresourceRange.layerCount = 1;
		  info.subresourceRange.aspectMask = aspectFlags;

		  return info;
	  }
