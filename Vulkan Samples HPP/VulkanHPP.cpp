#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VKB_DEBUG 1
#define VK_USE_PLATFORM_WIN32_KHR 1
#define GLFW_INCLUDE_VULKAN
#define GLFW_DLL

#include <GLFW/glfw3.h>

#include "VulkanHPP.h"
#include <fmt/core.h>
#include <shaderc/shaderc.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#define LOGE(...) fmt::print("Error!");
#define LOGI(...) fmt::print(__VA_ARGS__);

void VulkanHPP::Prepare()
{
	InitWindow();
	InitInstance(m_Context, { VK_KHR_SURFACE_EXTENSION_NAME }, {});
	SelectPhysicalDeviceAndInstance(m_Context);
}

void VulkanHPP::InitWindow()
{
	vk::ApplicationInfo appInfo("Hello Triangle", VK_MAKE_VERSION(1, 0, 0), "No Engine",
		VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);
	auto width = 1024;
	auto height = 768;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_GLFWwindow = glfwCreateWindow(width, height, "Vulkan 101", nullptr, nullptr);
}


vk::SurfaceKHR VulkanHPP::CreateSurface(vk::Instance instance, vk::PhysicalDevice)
{
	if (!instance || !m_GLFWwindow)
	{
		return nullptr;
	}

	VkSurfaceKHR surface;

	VkResult errCode = glfwCreateWindowSurface(instance, m_GLFWwindow, NULL, &surface);

	if (errCode != VK_SUCCESS)
	{
		return nullptr;
	}
	
	return surface;
}
void VulkanHPP::SelectPhysicalDeviceAndInstance(Context context)
{
	std::vector<vk::PhysicalDevice> gpus = context.instance.enumeratePhysicalDevices();

	for (size_t i = 0; i < gpus.size() && (context.graphics_queue_index < 0); i++)
	{
		context.gpu = gpus[i];

		std::vector<vk::QueueFamilyProperties> queue_family_properties = context.gpu.getQueueFamilyProperties();

		if (queue_family_properties.empty())
		{
			throw std::runtime_error("No queue family found.");
		}

		if (context.surface)
		{
			context.instance.destroySurfaceKHR(context.surface);
		}
		context.surface = CreateSurface(context.instance, context.gpu);

		for (uint32_t j = 0; j < static_cast<uint32_t>(queue_family_properties.size()); j++)
		{
			vk::Bool32 supports_present = context.gpu.getSurfaceSupportKHR(j, context.surface);

			// Find a queue family which supports graphics and presentation.
			if ((queue_family_properties[j].queueFlags & vk::QueueFlagBits::eGraphics) && supports_present)
			{
				context.graphics_queue_index = j;
				break;
			}
		}
	}

	if (context.graphics_queue_index < 0)
	{
		LOGE("Did not find suitable queue which supports graphics and presentation.");
	}


}
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
/// @brief A debug callback called from Vulkan validation layers.
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type,
	uint64_t object, size_t location, int32_t message_code,
	const char* layer_prefix, const char* message, void* user_data)
{
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		LOGE("Validation Layer: Error: {}: {}", layer_prefix, message);
	}
	else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		LOGE("Validation Layer: Warning: {}: {}", layer_prefix, message);
	}
	else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
	{
		LOGI("Validation Layer: Performance warning: {}: {}", layer_prefix, message);
	}
	else
	{
		LOGI("Validation Layer: Information: {}: {}", layer_prefix, message);
	}
	return VK_FALSE;
}
#endif



void VulkanHPP::InitInstance(Context& context, const std::vector<const char*>& required_instance_extensions, const std::vector<const char*>& required_validation_layers)
{
	fmt::print("Initializing Vulkan");

	//alternative
	// vk::ApplicationInfo appInfo("Hello Triangle", VK_MAKE_VERSION(1, 0, 0), "No Engine",
	// 	VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);
	// auto width = 1024;
	// auto height = 768;
	//
	// glfwInit();
	// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// auto window = glfwCreateWindow(width, height, "Vulkan 101", nullptr, nullptr);
	//
	// auto glfwExtensionCount = 0u;
	// auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	// std::vector<const char*> glfwExtensionsVector(glfwExtensions, glfwExtensions + glfwExtensionCount);
	// glfwExtensionsVector.push_back("VK_EXT_debug_utils");
	// auto layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };
	//
	//
	// auto instance = vk::createInstanceUnique(
	// 	vk::InstanceCreateInfo{ {}, &appInfo, static_cast<uint32_t>(layers.size()), layers.data(),
	// 		static_cast<uint32_t>(glfwExtensionsVector.size()), glfwExtensionsVector.data() });
	// auto dldi = vk::DispatchLoaderDynamic(*instance, vkGetInstanceProcAddr);

	static vk::DynamicLoader  dl;
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

	std::vector<vk::ExtensionProperties> instance_extensions = vk::enumerateInstanceExtensionProperties();
	std::vector<const char*> active_instance_extensions(required_instance_extensions);

#pragma region platform
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	active_instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	active_instance_extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
	active_instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
	active_instance_extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	active_instance_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	active_instance_extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	active_instance_extensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
	active_instance_extensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#else
#	pragma error Platform not supported
#endif
#pragma endregion 

	if (!ValidateExtensions(active_instance_extensions, instance_extensions))
	{
		throw std::runtime_error("Required instance extensions are missing.");
	}

	std::vector<vk::LayerProperties> supported_validation_layers = vk::enumerateInstanceLayerProperties();

	std::vector<const char*> requested_validation_layers(required_validation_layers);

#ifdef VKB_VALIDATION_LAYERS
	// Determine the optimal validation layers to enable that are necessary for useful debugging
	std::vector<const char*> optimal_validation_layers = get_optimal_validation_layers(supported_validation_layers);
	requested_validation_layers.insert(requested_validation_layers.end(), optimal_validation_layers.begin(), optimal_validation_layers.end());
#endif

	//todo use spdlog!
	if (ValidateLayers(requested_validation_layers, supported_validation_layers))
	{
		fmt::print("Enabled Validation Layers:");
		for (const auto& layer : requested_validation_layers)
		{
			fmt::print("	\t{}", layer);
		}
	}
	else
	{
		throw std::runtime_error("Required validation layers are missing.");
	}
	vk::ApplicationInfo app("HPP Vulkan 2", {}, "Ported from Vulkan Samples", VK_MAKE_VERSION(1, 0, 0));

	vk::InstanceCreateInfo instance_info({}, &app, requested_validation_layers, active_instance_extensions);

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	vk::DebugReportCallbackCreateInfoEXT debug_report_create_info(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning, debug_callback);

	instance_info.pNext = &debug_report_create_info;
#endif

	// Create the Vulkan instance
	context.instance = vk::createInstance(instance_info);

	// initialize function pointers for instance
	VULKAN_HPP_DEFAULT_DISPATCHER.init(context.instance);

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	context.debug_callback = context.instance.createDebugReportCallbackEXT(debug_report_create_info);
#endif
}


bool VulkanHPP::ValidateExtensions(const std::vector<const char*>& required, const std::vector<vk::ExtensionProperties>& available)
{
	// inner find_if gives true if the extension was not found
	// outer find_if gives true if none of the extensions were not found, that is if all extensions were found
	return std::find_if(required.begin(),
		required.end(),
		[&available](auto extension) {
			return std::find_if(available.begin(),
				available.end(),
				[&extension](auto const& ep) {
					return strcmp(ep.extensionName, extension) == 0;
				}) == available.end();
		}) == required.end();
}

bool ValidateLayers(const std::vector<const char*>& required, const std::vector<vk::LayerProperties>& available)
{
	// inner find_if returns true if the layer was not found
	// outer find_if returns iterator to the not found layer, if any
	auto requiredButNotFoundIt = std::find_if(required.begin(),
		required.end(),
		[&available](auto layer) {
			return std::find_if(available.begin(),
				available.end(),
				[&layer](auto const& lp) {
					return strcmp(lp.layerName, layer) == 0;
				}) == available.end();
		});
	if (requiredButNotFoundIt != required.end())
	{
		fmt::print("Validation Layer {} not found", *requiredButNotFoundIt);
	}
	return (requiredButNotFoundIt == required.end());
}
