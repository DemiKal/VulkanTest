#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VK_USE_PLATFORM_WIN32_KHR 1
#define VKB_VALIDATION_LAYERS
#define VKB_DEBUG 1
#define VMA_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
//#define GLFW_DLL
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "VulkanHPP.h"
#include <variant>
#include <tuple>
#include <fstream>

__pragma(warning(push, 0))
#include <GLFW/glfw3.h>
#include <iostream>
#include <shaderc/shaderc.hpp>
#include "vk_mem_alloc.h"
__pragma(warning(pop))

#include "Logger.hpp"
#include "MeshManager.h"
#include "LoadModel.h"
#include "Texture2D.h"
//#include <stb_image.h>

std::vector<const char*> get_optimal_validation_layers(const std::vector<vk::LayerProperties>& supported_instance_layers);




VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type, uint64_t object, size_t location, int32_t message_code, const char* layer_prefix, const char* message, void* user_data);
void WindowSizeCallback(GLFWwindow* window, int width, int height);
void MouseMoveHandler(GLFWwindow* window, double posx, double posy);
void MouseHandler(GLFWwindow* window, int button, int action, int mods);

struct UBO
{
	glm::mat4 proj;
	//glm::mat4 view = glm::lookAt(glm::vec3(-5.0f, 3.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	glm::mat4 view;// = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2.5f));
}ubo_vs;

void VulkanHPP::UpdateUniformBuffer(float dt)
{
	static size_t tick = 0;
	tick++;
	float aspect = (float)m_Width / (float)m_Height;
	//	if(aspect != m_Camera->m_AspectRatio)

		//if (aspect > 0)
		//ubo_vs.proj = glm::perspective(glm::radians(70.0f), aspect, 0.0f, 100.0f);

		//ubo_vs.view = glm::translate(ubo_vs.view, glm::vec3(0, 0, 0.1f) * dt);
	m_Camera->UpdateViewMatrix();
	ubo_vs.proj = (m_Camera->m_Projection);
	ubo_vs.view = (m_Camera->m_View);
	m_Uniform->convert_and_update(m_Allocator, ubo_vs);
}



#pragma region templatememes
//template<auto N>
//struct TestExtraction {
//
//};
//template<auto arg_N>
//struct val {
//	static constexpr auto N = arg_N;
//};
//
//template<template <auto> typename T, auto N>
//constexpr auto extract(const T<N>&)->val<N>;
//
//template<typename T>
//constexpr auto extract_N = decltype(extract(std::declval<T>()))::N;
//
//constexpr auto ffsd = extract_N<TestExtraction<5>>;
//
//template<typename...Ts>
//using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));
//
//template<typename...Ts>
//using remove_t = tuple_cat_t<
//	typename std::conditional<
//	//IsValidAttrib<Ts>(),
//	//!Ts::enabled(),
//	!extract_N<Ts>,
//	std::tuple<>,   //<-- if true
//	std::tuple<Ts>  //<-- if not
//	>::type...
//>;
//
//
//template <class T, template <class> class OtherType>
//class Nest
//{
//	OtherType<T> f;
//};


//template <typename... Types>
//struct VertexDataOld
//{
//	//using Converted... = AttributeToType(Types)... ;
//	//static const  Attribs<Types...> attribs;
//	static constexpr int count = sizeof...(Types);
//	static constexpr std::array<int, count> byteSizes{ sizeof(Types)... };
//	static constexpr std::array<int, count> componentLenghts{ Types::length()... };
//
//	//static const inline std::array<VertexAttrib, count> Attributes{ AttributeToType<Types>()... };
//	//static const inline std::array<int, count> typeIndices = { std::index_sequence<Types...>() };
//	// static constexpr std::array<int, sizeof...(Types)> typeIndices = std::array{Types...};
//
//	template <class T, class Tuple>
//	struct Index;
//
//	template <class T, class... Types>
//	struct Index<T, std::tuple<T, Types...>> {
//		static const std::size_t value = 0;
//	};
//
//	template <class T, class U, class... Types>
//	struct Index<T, std::tuple<U, Types...>> {
//		static const std::size_t value = 1 + Index<T, std::tuple<Types...>>::value;
//	};
//	//static const inline std::unordered_map<std::type_index, size_t> lookup{ {std::type_index{typeid(Types)}, Index<Types,std::tuple<Types...>>::value }  ... };
//	//static constexpr int stride = (sizeof(Types) + ...);
//	//static constexpr std::array< VertexAttrib, count> types = { AttributeToType<Types>() ... };
//	//static const size_t maxzzz = MAX(Types,...); //std::max( std::max(sizeof(Types)...));
//
//	static constexpr size_t validCount = (IsValid<Types>() + ...);
//	//static std::vector<int> as; //= { if constexpr (IsValid<Types>()) { IsValid<Types>() }   ... };
//
//	template<typename T>
//	static const T GetMeme(int i)
//	{
//		return __nth_element(i, Types...);
//	}
//
//	template<typename T>
//	static  constexpr int GetIndex()
//	{
//		return lookup.at(std::type_index{ typeid(T) });
//	}
//	operator std::string() const { return std::to_string(stride); }
//};
#pragma endregion

MeshManager  meshManager;
Image texture;

std::string LoadFile(const std::string& path)
{
	std::ifstream stream(path);
	return  std::string((std::istreambuf_iterator<char>(stream)), (std::istreambuf_iterator<char>()));
}

void VulkanHPP::Prepare()
{
	float aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	m_Camera = std::make_unique<Camera>(glm::vec3(0, 0, -2.5f), 70.0f, aspect, 0.01f, 100.0f);

	InitLogger();
	InitWindow();
	InitInstance(m_Context, { VK_KHR_SURFACE_EXTENSION_NAME }, {});
	SelectPhysicalDeviceAndInstance(m_Context);
	InitDevice(m_Context, { VK_KHR_MAINTENANCE1_EXTENSION_NAME, VK_KHR_SWAPCHAIN_EXTENSION_NAME });
	InitSwapchain(m_Context);
	InitAllocator(m_Context);
	LoadMeshes();
	SetupDepthStencil(m_Context);
	InitRenderPass(m_Context);
	SetupRenderPassBeginInfo(m_Context);
	InitUniformBuffer(m_Context);
	InitDescriptorPool(m_Context);
	InitDescriptorSetLayout(m_Context);
	SetupDescriptorSet(m_Context);
	InitPipeline(m_Context);
	InitFrameBuffers(m_Context);

	InitVertices(m_Context);
}
void VulkanHPP::LoadMeshes()
{
#pragma region
	//Buffer vb;
	//vb.AddAttribute(PositionAttribute{});
	//vb.AddAttribute(ColorAttribute{});
	//vb.Finalize();
	//PositionAttribute v0p = PositionAttribute{ 1.0f, 1.0f, 0.0f };
	//ColorAttribute v0c = ColorAttribute{ 1.0f, 0.0f, 0.0f };
	//PositionAttribute v1p = PositionAttribute{ -1.0f, 1.0f, 0.0f };
	//ColorAttribute v1c = ColorAttribute{ 0.0f, 1.0f, 0.0f };
	//PositionAttribute v2p = PositionAttribute{ 0.0f, -1.0f, 0.0f };
	//ColorAttribute v2c = ColorAttribute{ 0.0f, 0.0f, 1.0f };
	//vb.AddElement< PositionAttribute>(1.0f, 1.0f, 0.0f);
	//vb.AddElement< ColorAttribute>(1.0f, 0.0f, 0.0f);
	//vb.AddElement< PositionAttribute>(-1.0f, 1.0f, 0.0f);
	//vb.AddElement< ColorAttribute>(0.0f, 1.0f, 0.0f);
	//vb.AddElement< PositionAttribute>(0.0f, -1.0f, 0.0f);
	//vb.AddElement< ColorAttribute>(0.0f, 0.0f, 1.0f);
	//vb.AddElement< BoneWeightAttribute>(0.0f, 0.0f, 1.0f );
	//
	//auto a1 = vb.GetAttribute<PositionAttribute>(0);
	//auto b1 = vb.GetAttribute<ColorAttribute>(0);
	//auto a2 = vb.GetAttribute<PositionAttribute>(1);
	//auto b2 = vb.GetAttribute<ColorAttribute>(1);
	//auto a3 = vb.GetAttribute<PositionAttribute>(2);
	//auto b3 = vb.GetAttribute<ColorAttribute>(2);
	//auto b4 = vb.GetAttribute<ColorAttribute>(3);
	//
	//IndexBuffer ib;
	//ib.AddElement< IndexAttribute>( 0 , 1 , 2 );
#pragma endregion

	//meshManager.LoadFromFile("../Assets/ColoredTriangle.glb", aiPostProcessSteps::aiProcess_Triangulate);
	LoadModel lm;
	//std::chrono start = std::chrono::now();
	//a/uto start = glfwGetTime();
	Mesh tri = lm.Load("../Assets/spyro.glb");
	//Texture2D tex;
	texture = LoadImageFromFile("../Assets/Spyro_texture.png");
	//auto end = glfwGetTime();
	//auto diff = (end - start) * 1000.0;
	//fmt::print("time to load: {} ms", diff);
	//Mesh& m = meshManager.GetMesh(0);
	meshManager.AddMesh(tri);
}

void VulkanHPP::SetupDescriptorSet(Context& context)
{
	// Allocate a new descriptor set from the global descriptor pool
	vk::DescriptorSetAllocateInfo allocInfo;
	allocInfo.descriptorPool = context.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &context.descriptorSetLayout;
	

	context.descriptorSet = context.device.allocateDescriptorSets(allocInfo)[0];

	// Update the descriptor set determining the shader binding points
	// For every binding point used in a shader there needs to be one
	// descriptor set matching that binding point
	//vk::DescriptorBufferInfo descrBufferInfo;// = (*m_Uniform->handle, 0, VK_WHOLE_SIZE);
	//descrBufferInfo.buffer = m_Uniform->handle;
	//descrBufferInfo.offset = 0;
	//descrBufferInfo.range = VK_WHOLE_SIZE;

	vk::WriteDescriptorSet writeDescriptorSetUBO;
	// Binding 0 : Uniform buffer
	writeDescriptorSetUBO.dstSet = context.descriptorSet;
	writeDescriptorSetUBO.descriptorCount = 1;
	writeDescriptorSetUBO.descriptorType = vk::DescriptorType::eUniformBuffer;
	writeDescriptorSetUBO.pBufferInfo = &m_Uniform->m_DescrBufferInfo;
	writeDescriptorSetUBO.dstBinding = 0;

	vk::DescriptorImageInfo texDescriptor{ texture.sampler, texture.view, texture.descriptor.imageLayout };
	vk::WriteDescriptorSet samplerDescrSet{ context.descriptorSet, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &texDescriptor };

	vk::WriteDescriptorSet descriptorsets[2] = { writeDescriptorSetUBO, samplerDescrSet };



	//writeDescriptorSet.pBufferInfo = &descrBufferInfo;
	//writeDescriptorSet.setBufferInfo()
	context.device.updateDescriptorSets({ writeDescriptorSetUBO, samplerDescrSet }, nullptr);
}

void VulkanHPP::InitDescriptorPool(Context& context)
{
	// We need to tell the API the number of max. requested descriptors per type
	std::vector<vk::DescriptorPoolSize> poolSizes =
	{
		vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer, 1 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler, 1 },
	};
	// For additional types you need to add new entries in the type count list
	// E.g. for two combined image samplers :
	// typeCounts[1].type = vk::DescriptorType::eCombinedImageSampler;
	// typeCounts[1].descriptorCount = 2;

	// Create the global descriptor pool
	// All descriptors used in this example are allocated from this pool
	vk::DescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolInfo.pPoolSizes = poolSizes.data();
	descriptorPoolInfo.maxSets = 2;
	// Set the max. number of sets that can be requested
	// Requesting descriptors beyond maxSets will result in an error
	context.descriptorPool = context.device.createDescriptorPool(descriptorPoolInfo);
}

void VulkanHPP::InitDescriptorSetLayout(Context& context)
{
	// vk::DescriptorSetLayoutBinding layoutBindings
	//layoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	//layoutBinding.descriptorCount = 1;
	//layoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
	//layoutBinding.pImmutableSamplers = nullptr;
	//layoutBinding.binding = 0;
	
	std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings{
		// Binding 0 : Vertex shader uniform buffer
		vk::DescriptorSetLayoutBinding{ 0,  vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex },
		// Binding 1 : Fragment shader image sampler
		vk::DescriptorSetLayoutBinding{ 1,  vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment },
	};

	
	vk::DescriptorSetLayoutCreateInfo descriptorLayout;
	descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
	descriptorLayout.pBindings = setLayoutBindings.data();

	context.descriptorSetLayout = context.device.createDescriptorSetLayout(descriptorLayout, nullptr);

	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &context.descriptorSetLayout;

	context.pipelineLayout = context.device.createPipelineLayout(pipelineLayoutCreateInfo);
}



void VulkanHPP::InitAllocator(Context& context)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = context.gpu;
	allocatorInfo.device = context.device;
	allocatorInfo.instance = context.instance;
	vmaCreateAllocator(&allocatorInfo, &m_Allocator);
}


//template <typename T>
void VulkanHPP::StageBuffer(Context& context, Buffer& allocBuffer, vk::BufferUsageFlagBits usageFlags, VmaMemoryUsage memoryUsage)
{
	const size_t bufferSize = allocBuffer.GetBufferSize();

	vk::BufferCreateInfo stagingBufferInfo({}, bufferSize, vk::BufferUsageFlagBits::eTransferSrc);
	const VkBufferCreateInfo C_stagingBufferInfo = stagingBufferInfo;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	AllocatedBuffer stagingBuffer;

	VK_CHECK(vmaCreateBuffer(m_Allocator, reinterpret_cast<VkBufferCreateInfo*>(&stagingBufferInfo), &vmaallocInfo, &stagingBuffer._buffer, &stagingBuffer._allocation, nullptr));

	void* data;
	VK_CHECK(vmaMapMemory(m_Allocator, stagingBuffer._allocation, &data));
	memcpy(data, allocBuffer.buffer.data(), bufferSize);

	vmaUnmapMemory(m_Allocator, stagingBuffer._allocation);
	////////////////////////
	////  COPY 1 DONE!  ////
	////////////////////////

	//C++ style
	auto usage = usageFlags | vk::BufferUsageFlagBits::eTransferDst;
	auto vkBuffer_CI = vk::BufferCreateInfo{ {}, bufferSize, usage };
	VkBufferCreateInfo vertexBufferInfo = vkBuffer_CI; //convert!
	//let the VMA library know that this data should be gpu native	

	vmaallocInfo.usage = memoryUsage;
	VkBuffer vkBuff = allocBuffer.vkBuffer;

	VmaAllocationInfo allocationInfo{};
	VK_CHECK(vmaCreateBuffer(m_Allocator, &vertexBufferInfo, &vmaallocInfo, reinterpret_cast<VkBuffer*>(&allocBuffer.vkBuffer), &allocBuffer.vmaAllocation, &allocationInfo));
	allocBuffer.DeviceMemory = static_cast<vk::DeviceMemory>(allocationInfo.deviceMemory);

	allocBuffer.mapped_data = static_cast<uint8_t*>(allocationInfo.pMappedData);

	auto cmdBuffer_AI = vk::CommandBufferAllocateInfo{ context.m_UploadContext._commandPool, vk::CommandBufferLevel::ePrimary, 1 };
	auto allocatedCmdBuffers = context.device.allocateCommandBuffers(cmdBuffer_AI);
	vk::CommandBuffer cmdBuffer = allocatedCmdBuffers.front();

	auto cmdBuffer_BI = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	cmdBuffer.begin(cmdBuffer_BI);

	////////////////////////				lambda
	VkBufferCopy copy;
	copy.dstOffset = 0;
	copy.srcOffset = 0;
	copy.size = bufferSize;

	vkCmdCopyBuffer(cmdBuffer, stagingBuffer._buffer, allocBuffer.vkBuffer, 1, &copy);
	////////////////////////				lambda
	cmdBuffer.end();

	auto submitInfo = vk::SubmitInfo{ {}, {}, {1, &cmdBuffer} };

	context.queue.submit(submitInfo, context.m_UploadContext._uploadFence);
	context.device.waitForFences(context.m_UploadContext._uploadFence, VK_TRUE, 9999999999);
	context.device.resetFences(context.m_UploadContext._uploadFence);
	context.device.resetCommandPool(context.m_UploadContext._commandPool);

	vmaDestroyBuffer(m_Allocator, stagingBuffer._buffer, stagingBuffer._allocation);
}

void VulkanHPP::InitUniformBuffer(Context& context)
{
	//constexpr size_t size = sizeof(Camera);
	//
	//UboBuffer.buffer.resize(size);
	//Camera* camPtr = reinterpret_cast<Camera*>(UboBuffer.buffer.data());
	//memcpy(camPtr, &camera, size);
	//
	//StageBuffer(context, UboBuffer, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
	//context.device.bindBufferMemory(UboBuffer.vkBuffer, UboBuffer.DeviceMemory, 0);
	//auto OBJECT_INSTANCES = 1;
	//size_t min_ubo_alignment = context.gpu.getProperties().limits.minUniformBufferOffsetAlignment;// get_device().get_gpu().get_properties().limits.minUniformBufferOffsetAlignment;
	//size_t dynamicAlignment = sizeof(glm::mat4);
	//if (min_ubo_alignment > 0)
	//{
	//	dynamicAlignment = (dynamicAlignment + min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
	//}
	//
	//size_t buffer_size = OBJECT_INSTANCES * dynamicAlignment;
	//ubo_data_dynamic.model = (glm::mat4*)_aligned_malloc(buffer_size, dynamicAlignment);
	//assert(ubo_data_dynamic.model);

	struct v { glm::mat4 a; glm::mat4 b; }aaa;
	VkDeviceSize sz = sizeof(aaa);
	m_Uniform = new UniformBuffer(context.device, sz, m_Allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	//m_Uniform->convert_and_update(m_Allocator, ubo_vs);


	//
   //
   //
   //
   //auto offset = 0;

   //auto mapped = context.device.mapMemory(memory, offset, size, vk::MemoryMapFlags());
}

void VulkanHPP::InitVertices(Context& context)
{
	auto& triMesh = meshManager.GetMesh(0);
	StageBuffer(context, triMesh.m_VertexBuffer, vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_GPU_ONLY);
	StageBuffer(context, triMesh.m_IndexBuffer, vk::BufferUsageFlagBits::eIndexBuffer, VMA_MEMORY_USAGE_GPU_ONLY);
}

void VulkanHPP::InitWindow()
{
	vk::ApplicationInfo appInfo("Hello Triangle",
		VK_MAKE_VERSION(1, 0, 0),
		"No Engine",
		VK_MAKE_API_VERSION(0, 1, 0, 0),
		VK_API_VERSION_1_2);


	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_GLFWwindow = glfwCreateWindow(m_Width, m_Height, "Vulkan 101", nullptr, nullptr);
	glfwSetWindowUserPointer(m_GLFWwindow, this);
	glfwSetWindowSizeCallback(m_GLFWwindow, WindowSizeCallback);
	glfwSetCursorPosCallback(m_GLFWwindow, MouseMoveHandler);
	glfwSetMouseButtonCallback(m_GLFWwindow, MouseHandler);

}


void VulkanHPP::InitInstance(Context& context, const std::vector<const char*>& required_instance_extensions, const std::vector<const char*>& required_validation_layers)
{
	LOGI("Initializing Vulkan");


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
		//fmt::print("Enabled Validation Layers:");
		for (const auto& layer : requested_validation_layers)
		{
			LOGI(layer);
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

void VulkanHPP::SelectPhysicalDeviceAndInstance(Context& context)
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

void VulkanHPP::InitDevice(Context& context, const std::vector<const char*>& required_device_extensions)
{
	LOGI("Initializing vulkan device.");
	std::vector<vk::ExtensionProperties> device_extensions = context.gpu.enumerateDeviceExtensionProperties();

	if (!ValidateExtensions(required_device_extensions, device_extensions))
	{
		throw std::runtime_error("Required device extensions are missing, will try without.");
	}

	float queue_priority = 1.0f;

	// Create one queue
	vk::DeviceQueueCreateInfo queue_info({}, context.graphics_queue_index, 1, &queue_priority);

	vk::DeviceCreateInfo device_info({}, queue_info, {}, required_device_extensions);

	context.device = context.gpu.createDevice(device_info);
	// initialize function pointers for device
	VULKAN_HPP_DEFAULT_DISPATCHER.init(context.device);

	context.queue = context.device.getQueue(context.graphics_queue_index, 0);

}

void VulkanHPP::InitSwapchain(Context& context)
{
	vk::SurfaceCapabilitiesKHR surface_properties = context.gpu.getSurfaceCapabilitiesKHR(context.surface);

	std::vector<vk::SurfaceFormatKHR> formats = context.gpu.getSurfaceFormatsKHR(context.surface);

	vk::SurfaceFormatKHR format;
	if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
	{
		// There is no preferred format, so pick a default one
		format = formats[0];
		format.format = vk::Format::eB8G8R8A8Unorm;
	}
	else
	{
		if (formats.empty())
		{
			throw std::runtime_error("Surface has no formats.");
		}

		format.format = vk::Format::eUndefined;
		for (auto& candidate : formats)
		{
			switch (candidate.format)
			{
			case vk::Format::eR8G8B8A8Unorm:
			case vk::Format::eB8G8R8A8Unorm:
			case vk::Format::eA8B8G8R8UnormPack32:
				format = candidate;
				break;

			default:
				break;
			}

			if (format.format != vk::Format::eUndefined)
			{
				break;
			}
		}

		if (format.format == vk::Format::eUndefined)
		{
			format = formats[0];
		}
	}

	vk::Extent2D swapchain_size;
	if (surface_properties.currentExtent.width == 0xFFFFFFFF)
	{
		swapchain_size.width = context.swapchain_dimensions.width;
		swapchain_size.height = context.swapchain_dimensions.height;
	}
	else
	{
		swapchain_size = surface_properties.currentExtent;
	}

	// FIFO must be supported by all implementations.
	vk::PresentModeKHR swapchain_present_mode = vk::PresentModeKHR::eFifo;

	// Determine the number of vk::Image's to use in the swapchain.
	// Ideally, we desire to own 1 image at a time, the rest of the images can
	// either be rendered to and/or being queued up for display.
	uint32_t desired_swapchain_images = surface_properties.minImageCount + 1;
	if ((surface_properties.maxImageCount > 0) && (desired_swapchain_images > surface_properties.maxImageCount))
	{
		// Application must settle for fewer images than desired.
		desired_swapchain_images = surface_properties.maxImageCount;
	}

	// Figure out a suitable surface transform.
	vk::SurfaceTransformFlagBitsKHR pre_transform =
		(surface_properties.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) ? vk::SurfaceTransformFlagBitsKHR::eIdentity : surface_properties.currentTransform;

	vk::SwapchainKHR old_swapchain = context.swapchain;

	// Find a supported composite type.
	vk::CompositeAlphaFlagBitsKHR composite = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	if (surface_properties.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eOpaque)
	{
		composite = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	}
	else if (surface_properties.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)
	{
		composite = vk::CompositeAlphaFlagBitsKHR::eInherit;
	}
	else if (surface_properties.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
	{
		composite = vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;
	}
	else if (surface_properties.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
	{
		composite = vk::CompositeAlphaFlagBitsKHR::ePostMultiplied;
	}

	//swapchain_size.width = swapchain_size.width == 0 ? 1 : swapchain_size.width ;
	//swapchain_size.height = swapchain_size.height == 0 ? 1 : swapchain_size.height ;

	vk::SwapchainCreateInfoKHR info;
	info.surface = context.surface;
	info.minImageCount = desired_swapchain_images;
	info.imageFormat = format.format;
	info.imageColorSpace = format.colorSpace;
	info.imageExtent.width = swapchain_size.width;
	info.imageExtent.height = swapchain_size.height;
	info.imageArrayLayers = 1;
	info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	info.imageSharingMode = vk::SharingMode::eExclusive;
	info.preTransform = pre_transform;
	info.compositeAlpha = composite;
	info.presentMode = swapchain_present_mode;
	info.clipped = true;
	info.oldSwapchain = old_swapchain;

	context.swapchain = context.device.createSwapchainKHR(info);

	if (old_swapchain)
	{
		for (vk::ImageView image_view : context.swapchain_image_views)
		{
			context.device.destroyImageView(image_view);
		}

		size_t image_count = context.device.getSwapchainImagesKHR(old_swapchain).size();

		for (size_t i = 0; i < image_count; i++)
		{
			TeardownPerFrame(context, context.per_frame[i]);
		}

		context.swapchain_image_views.clear();

		context.device.destroySwapchainKHR(old_swapchain);
	}

	context.swapchain_dimensions = { swapchain_size.width, swapchain_size.height, format.format };

	/// The swapchain images.
	std::vector<vk::Image> swapchain_images = context.device.getSwapchainImagesKHR(context.swapchain);
	size_t                 image_count = swapchain_images.size();

	// Initialize per-frame resources.
	// Every swapchain image has its own command pool and fence manager.
	// This makes it very easy to keep track of when we can reset command buffers and such.
	context.per_frame.clear();
	context.per_frame.resize(image_count);

	for (size_t i = 0; i < image_count; i++)
	{
		InitPerFrame(context, context.per_frame[i]);
	}

	vk::ImageViewCreateInfo view_info;
	view_info.viewType = vk::ImageViewType::e2D;
	view_info.format = context.swapchain_dimensions.format;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.layerCount = 1;
	view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	view_info.components.r = vk::ComponentSwizzle::eR;
	view_info.components.g = vk::ComponentSwizzle::eG;
	view_info.components.b = vk::ComponentSwizzle::eB;
	view_info.components.a = vk::ComponentSwizzle::eA;
	for (size_t i = 0; i < image_count; i++)
	{
		// Create an image view which we can render into.
		view_info.image = swapchain_images[i];

		context.swapchain_image_views.push_back(context.device.createImageView(view_info));
	}

	//upload context
	vk::CommandPoolCreateInfo cmdPoolInfoUpload(vk::CommandPoolCreateFlagBits::eTransient, context.graphics_queue_index);
	context.m_UploadContext._commandPool = context.device.createCommandPool(cmdPoolInfoUpload);

	vk::FenceCreateInfo uploadFenceCreateInfo;
	context.m_UploadContext._uploadFence = context.device.createFence(uploadFenceCreateInfo);


}

void VulkanHPP::InitRenderPass(Context& context)
{
	std::vector<vk::AttachmentDescription> attachments;
	attachments.resize(2);

	// Backbuffer format.
	attachments[0].format = context.swapchain_dimensions.format;
	// Not multisampled.
	attachments[0].samples = vk::SampleCountFlagBits::e1;
	// When starting the frame, we want tiles to be cleared.
	attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
	// When ending the frame, we want tiles to be written out.
	attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
	// Don't care about stencil since we're not using it.
	attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

	// The image layout will be undefined when the render pass begins.
	attachments[0].initialLayout = vk::ImageLayout::eUndefined;
	// After the render pass is complete, we will transition to ePresentSrcKHR layout.
	attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

	//depth attachment
	attachments[1].format = vk::Format::eD32SfloatS8Uint; //TODO: set this dynamically!
	attachments[1].loadOp = vk::AttachmentLoadOp::eClear;
	attachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
	attachments[1].stencilLoadOp = vk::AttachmentLoadOp::eClear;
	attachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	attachments[1].initialLayout = vk::ImageLayout::eUndefined;
	attachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;




	// We have one subpass. This subpass has one color attachment.
	// While executing this subpass, the attachment will be in attachment optimal layout.
	//vk::AttachmentReference color_ref(0, vk::ImageLayout::eColorAttachmentOptimal);
	std::vector<vk::AttachmentReference> colorAttachmentReferences;
	{
		vk::AttachmentReference colorReference;
		colorReference.attachment = 0;
		colorReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
		colorAttachmentReferences.push_back(colorReference);
	}

	vk::AttachmentReference depthReference;
	depthReference.attachment = 1;
	depthReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	// We will end up with two transitions.
	// The first one happens right before we start subpass #0, where
	// eUndefined is transitioned into eColorAttachmentOptimal.
	// The final layout in the render pass attachment states ePresentSrcKHR, so we
	// will get a final transition from eColorAttachmentOptimal to ePresetSrcKHR.
	//vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, {}, color_ref,nullptr, &depthReference);
	std::vector<vk::SubpassDescription> subpasses{
	  {
		  {}, vk::PipelineBindPoint::eGraphics,
		  // Input attachment references
		  0, nullptr,
		  // Color / resolve attachment references
		  (uint32_t)colorAttachmentReferences.size(), colorAttachmentReferences.data(), nullptr,
		  // Depth stecil attachment reference,
		  &depthReference,
		  // Preserve attachments
		  0, nullptr
	  },
	};
	using vPSFB = vk::PipelineStageFlagBits;
	using vAFB = vk::AccessFlagBits;
	std::vector<vk::SubpassDependency> subpassDependencies{
		{
			0, VK_SUBPASS_EXTERNAL,
			vPSFB::eColorAttachmentOutput, vPSFB::eBottomOfPipe,
			vAFB::eColorAttachmentRead | vAFB::eColorAttachmentWrite, vAFB::eMemoryRead,
			vk::DependencyFlagBits::eByRegion
		},
		{
			VK_SUBPASS_EXTERNAL, 0,
			vPSFB::eBottomOfPipe, vPSFB::eColorAttachmentOutput,
			vAFB::eMemoryRead, vAFB::eColorAttachmentRead | vAFB::eColorAttachmentWrite,
			vk::DependencyFlagBits::eByRegion
		},
	};
	// Create a dependency to external events.
	// We need to wait for the WSI semaphore to signal.
	// Only pipeline stages which depend on eColorAttachmentOutput will
	// actually wait for the semaphore, so we must also wait for that pipeline stage.
	//old
	//vk::SubpassDependency dependency;
	//dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	//dependency.dstSubpass = 0;
	//dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	//dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	//
	//// Since we changed the image layout, we need to make the memory visible to
	//// color attachment to modify.
	//dependency.srcAccessMask = {};
	//dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

	// Finally, create the renderpass.
	//vk::RenderPassCreateInfo rp_info({}, attachments, subpass, dependency);
	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachmentCount = (uint32_t)attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = (uint32_t)subpasses.size();
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = (uint32_t)subpassDependencies.size();
	renderPassInfo.pDependencies = subpassDependencies.data();

	context.render_pass = context.device.createRenderPass(renderPassInfo);
}


void VulkanHPP::InitPipeline(Context& context)
{
	// Create a blank pipeline layout.
	// We are not binding any resources to the pipeline in this first sample.
	context.pipelineLayout;// = context.device.createPipelineLayout({});


	vk::PipelineVertexInputStateCreateInfo vertexInput;
	auto& triMesh = meshManager.GetMesh(0);
	auto bindingDescriptions = triMesh.m_VertexBuffer.GetVertexInputBindingDescriptions();
	vertexInput.setVertexBindingDescriptions({ (uint32_t)bindingDescriptions.size(), bindingDescriptions.data() });

	auto attributeDescriptions = triMesh.m_VertexBuffer.GetVertexInputAttributeDescriptions();
	vertexInput.setVertexAttributeDescriptions({ (uint32_t)attributeDescriptions.size(), attributeDescriptions.data() });

	// Specify we will use triangle lists to draw geometry.
	vk::PipelineInputAssemblyStateCreateInfo input_assembly({}, vk::PrimitiveTopology::eTriangleList);

	// Specify rasterization state.
	vk::PipelineRasterizationStateCreateInfo raster;
	raster.cullMode = vk::CullModeFlagBits::eNone;
	raster.frontFace = vk::FrontFace::eCounterClockwise;
	raster.lineWidth = 1.0f;

	// Our attachment will write to all color channels, but no blending is enabled.
	vk::PipelineColorBlendAttachmentState blend_attachment;
	blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

	vk::PipelineColorBlendStateCreateInfo blend({}, {}, {}, blend_attachment);

	// We will have one viewport and scissor box.
	vk::PipelineViewportStateCreateInfo viewport;
	viewport.viewportCount = 1;
	viewport.scissorCount = 1;

	// Disable all depth testing.
	vk::PipelineDepthStencilStateCreateInfo depth_stencil;
	depth_stencil.depthTestEnable = VK_TRUE;
	depth_stencil.depthWriteEnable = VK_TRUE;
	depth_stencil.depthCompareOp = vk::CompareOp::eLess;
	depth_stencil.depthBoundsTestEnable = VK_FALSE;
	depth_stencil.stencilTestEnable = VK_FALSE;

	// No multisampling.
	vk::PipelineMultisampleStateCreateInfo multisample({}, vk::SampleCountFlagBits::e1);

	// Specify that these states will be dynamic, i.e. not part of pipeline state object.
	std::array<vk::DynamicState, 2> dynamics{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamic({}, dynamics);


	std::string vertexSource = LoadFile("../Assets/shaders/texture.vert");
	std::string fragmentSource = LoadFile("../Assets/shaders/texture.frag");

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetOptimizationLevel(shaderc_optimization_level_performance);

	shaderc::SpvCompilationResult vertShaderResult =
		compiler.CompileGlslToSpv(vertexSource, shaderc_glsl_vertex_shader, "vertex shader", options);

	auto vertStatus = vertShaderResult.GetCompilationStatus();
	if (vertStatus != shaderc_compilation_status_success) { LOGE(vertShaderResult.GetErrorMessage()); }

	auto vertShaderCode = std::vector<uint32_t>{ vertShaderResult.cbegin(), vertShaderResult.cend() };
	auto vertSize = std::distance(vertShaderCode.begin(), vertShaderCode.end());
	auto vertShaderCreateInfo = vk::ShaderModuleCreateInfo{ {}, vertSize * sizeof(uint32_t), vertShaderCode.data() };
	auto vertexShaderModule = context.device.createShaderModule(vertShaderCreateInfo);

	shaderc::SpvCompilationResult fragShaderModule = compiler.CompileGlslToSpv(fragmentSource, shaderc_glsl_fragment_shader, "fragment shader", options);
	if (fragShaderModule.GetCompilationStatus() != shaderc_compilation_status_success) 	  fmt::print(fragShaderModule.GetErrorMessage());

	auto fragShaderCode = std::vector<uint32_t>{ fragShaderModule.cbegin(), fragShaderModule.cend() };
	auto fragSize = std::distance(fragShaderCode.begin(), fragShaderCode.end());
	auto fragShaderCreateInfo = vk::ShaderModuleCreateInfo{ {}, fragSize * sizeof(uint32_t), fragShaderCode.data() };
	auto fragmentShaderModule = context.device.createShaderModule(fragShaderCreateInfo);


	std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stages{
	vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main"),
	vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragmentShaderModule, "main") };


	vk::GraphicsPipelineCreateInfo pipe({}, shader_stages);
	pipe.pVertexInputState = &vertexInput;
	pipe.pInputAssemblyState = &input_assembly;
	pipe.pRasterizationState = &raster;
	pipe.pColorBlendState = &blend;
	pipe.pMultisampleState = &multisample;
	pipe.pViewportState = &viewport;
	pipe.pDepthStencilState = &depth_stencil;
	pipe.pDynamicState = &dynamic;

	// We need to specify the pipeline layout and the render pass description up front as well.
	pipe.renderPass = context.render_pass;
	pipe.setLayout(context.pipelineLayout);

	context.pipeline = context.device.createGraphicsPipeline(nullptr, pipe).value;
	VkPipeline a1 = static_cast<VkPipeline>(context.pipeline);
	VkPipelineLayout a2 = static_cast<VkPipelineLayout>(context.pipelineLayout);
	//VkPipeline a1 = static_cast<VkPipeline>(context.pipeline);
	// Pipeline is baked, we can delete the shader modules now.
	context.device.destroyShaderModule(shader_stages[0].module);
	context.device.destroyShaderModule(shader_stages[1].module);
}

void VulkanHPP::InitFrameBuffers(Context& context)
{
	vk::Device device = context.device;

	// Depth/Stencil attachment is the same for all frame buffers
	// Create framebuffer for each swapchain image view
	for (auto& image_view : context.swapchain_image_views)
	{
		// Build the framebuffer.
		//old: vk::FramebufferCreateInfo fb_info({}, context.render_pass, image_view, context.swapchain_dimensions.width, context.swapchain_dimensions.height, 1);
		vk::ImageView attachments[2];
		attachments[0] = image_view;
		attachments[1] = m_DepthStencil.view;

		vk::FramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.renderPass = context.render_pass;
		framebufferCreateInfo.attachmentCount = 2;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = context.swapchain_dimensions.width;
		framebufferCreateInfo.height = context.swapchain_dimensions.height;
		framebufferCreateInfo.layers = 1;

		context.swapchain_framebuffers.push_back(device.createFramebuffer(framebufferCreateInfo));
	}
}


void VulkanHPP::RunLoop()
{
	//constexpr float deltaTime = 1.f / 60.f;
	float prevFrameTime = static_cast<float>(glfwGetTime());
	float currentFrameTime = prevFrameTime;

	while (!glfwWindowShouldClose(m_GLFWwindow))
	{
		glfwPollEvents();
		prevFrameTime = currentFrameTime;
		currentFrameTime = static_cast<float>(glfwGetTime());
		float dt = (currentFrameTime - prevFrameTime);
		HandleInput(dt);
		//if (dt < 0.01) dt = 1.0f / 60.0f;
		float ms = 1000.0f * dt;
		float fps = 1000.0f / ms;
		glfwSetWindowTitle(m_GLFWwindow, fmt::format("ms: {:.1f}, fps: {:.1f}", ms, fps).c_str());
		auto& pos = m_Camera->m_Position;
		//glfwSetWindowTitle(m_GLFWwindow, fmt::format("pos: ({:.1f}, {:.1f}, {:.1f})", pos.x, pos.y, pos.z).c_str());

		Update(dt);

		//state reset
		//m_MouseLeft = m_MouseRight = false;

	}
}




void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
	if (auto glfw_window = reinterpret_cast<VulkanHPP*>(glfwGetWindowUserPointer(window)))

	{
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}

		//auto& platform = glfw_window->get_platform();
		glfw_window->Resize(width, height);
		glfw_window->m_Width = width;
		glfw_window->m_Height = height;
		//glfw_window->resize(width, height);
	}
}

void VulkanHPP::Update(float dt)
{
	uint32_t index;

	auto res = AcquireNextImage(m_Context, &index);

	// Handle outdated error in acquire.
	if (res == vk::Result::eSuboptimalKHR || res == vk::Result::eErrorOutOfDateKHR)
	{
		Resize(m_Context.swapchain_dimensions.width, m_Context.swapchain_dimensions.height);
		res = AcquireNextImage(m_Context, &index);
	}

	if (res != vk::Result::eSuccess)
	{
		m_Context.queue.waitIdle();
		return;
	}

	RenderTriangle(m_Context, index);
	res = PresentImage(m_Context, index);

	// Handle Outdated error in present.
	if (res == vk::Result::eSuboptimalKHR || res == vk::Result::eErrorOutOfDateKHR)
	{
		Resize(m_Context.swapchain_dimensions.width, m_Context.swapchain_dimensions.height);
	}
	else if (res != vk::Result::eSuccess)
	{
		LOGE("Failed to present swapchain image.");
	}

	UpdateUniformBuffer(dt);
}

vk::Result VulkanHPP::PresentImage(Context& context, uint32_t index)
{
	vk::PresentInfoKHR present(context.per_frame[index].swapchain_release_semaphore, context.swapchain, index);
	// Present swapchain image
	return context.queue.presentKHR(present);
}

void VulkanHPP::RenderTriangle(Context& context, uint32_t swapchain_index)
{
	// Render to this framebuffer.
	vk::Framebuffer framebuffer = context.swapchain_framebuffers[swapchain_index];

	// Allocate or re-use a primary command buffer.
	vk::CommandBuffer cmd = context.per_frame[swapchain_index].primary_command_buffer;

	// We will only submit this once before it's recycled.
	vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	// Begin command recording
	cmd.begin(begin_info);

	// Set clear color values.
	//vk::ClearValue clear_value;
	//clear_value.color = vk::ClearColorValue(std::array<float, 4>({ {0.1f, 0.1f, 0.2f, 1.0f} }));
	//std::vector< vk::ClearValue> clearValues =
	//{
	//	vk::ClearValue(vk::ClearColorValue(std::array<float, 4>({ {0.1f, 0.1f, 0.2f, 1.0f} }))),
	//	vk::ClearDepthStencilValue{ 1.0f, 0 }
	//};

	// Begin the render pass.
	//vk::RenderPassBeginInfo rp_begin;
	//(
	//	context.render_pass,
	//	framebuffer, 
	//	{ {0, 0},
	//	{context.swapchain_dimensions.width,
	//	context.swapchain_dimensions.height} },
	// 	clear_value);
	//rp_begin.renderPass = context.render_pass;
	//rp_begin.renderArea.extent = vk::Extent2D{ context.swapchain_dimensions.width,	context.swapchain_dimensions.height };
	//rp_begin.clearValueCount = clearValues.size();
	//rp_begin.pClearValues =  clearValues.data();
	//vk::RenderPassBeginInfo

	//rp_begin.framebuffer = framebuffer; 
	// We will add draw commands in the same command buffer.
	m_RenderPassBeginInfo.framebuffer = framebuffer;
	m_RenderPassBeginInfo.renderArea = vk::Rect2D{ {0u,0u},{(uint32_t)context.swapchain_dimensions.width, (uint32_t)context.swapchain_dimensions.height} };
	cmd.beginRenderPass(m_RenderPassBeginInfo, vk::SubpassContents::eInline);

	cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, context.pipelineLayout, 0, context.descriptorSet, nullptr);

	// Bind the graphics pipeline.
	auto& triMesh = meshManager.GetMesh(0);
	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, context.pipeline);
	vk::Buffer buffer{ triMesh.m_VertexBuffer.vkBuffer };
	cmd.bindVertexBuffers(0, buffer, { 0 });
	auto indexType = triMesh.m_IndexBuffer.GetIndexType();

	cmd.bindIndexBuffer(triMesh.m_IndexBuffer.vkBuffer, 0, indexType);

	auto width = static_cast<float>(context.swapchain_dimensions.width);
	auto height = static_cast<float>(context.swapchain_dimensions.height);
	vk::Viewport vp(0, height, width, -height, 0.0f, 1.0f);


	cmd.setViewport(0, vp);

	vk::Rect2D scissor({ 0, 0 }, { context.swapchain_dimensions.width, context.swapchain_dimensions.height });
	// Set scissor dynamically
	cmd.setScissor(0, scissor);

	// Draw three vertices with one instance.
	//cmd.draw(3, 1, 0, 0);
	size_t vertexCount = triMesh.m_VertexBuffer.GetVertexCount();
	size_t indexCount = triMesh.m_IndexBuffer.GetVertexCount();

	cmd.drawIndexed(static_cast<uint32_t>(indexCount), 1, 0, 0, 0);

	// Complete render pass.
	cmd.endRenderPass();

	// Complete the command buffer.
	cmd.end();

	// Submit it to the queue with a release semaphore.
	if (!context.per_frame[swapchain_index].swapchain_release_semaphore)
	{
		context.per_frame[swapchain_index].swapchain_release_semaphore = context.device.createSemaphore({});
	}

	vk::PipelineStageFlags wait_stage{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	vk::SubmitInfo info(context.per_frame[swapchain_index].swapchain_acquire_semaphore, wait_stage, cmd,
		context.per_frame[swapchain_index].swapchain_release_semaphore);
	// Submit command buffer to graphics queue
	context.queue.submit(info, context.per_frame[swapchain_index].queue_submit_fence);
}
vk::Result VulkanHPP::AcquireNextImage(Context& context, uint32_t* image)
{
	vk::Semaphore acquire_semaphore;
	if (context.recycled_semaphores.empty())
	{
		acquire_semaphore = context.device.createSemaphore({});
	}
	else
	{
		acquire_semaphore = context.recycled_semaphores.back();
		context.recycled_semaphores.pop_back();
	}

	vk::Result res;
	std::tie(res, *image) = context.device.acquireNextImageKHR(context.swapchain, UINT64_MAX, acquire_semaphore);

	if (res != vk::Result::eSuccess)
	{
		context.recycled_semaphores.push_back(acquire_semaphore);
		return res;
	}

	// If we have outstanding fences for this swapchain image, wait for them to complete first.
	// After begin frame returns, it is safe to reuse or delete resources which
	// were used previously.
	//
	// We wait for fences which completes N frames earlier, so we do not stall,
	// waiting for all GPU work to complete before this returns.
	// Normally, this doesn't really block at all,
	// since we're waiting for old frames to have been completed, but just in case.
	if (context.per_frame[*image].queue_submit_fence)
	{
		context.device.waitForFences(context.per_frame[*image].queue_submit_fence, true, UINT64_MAX);
		context.device.resetFences(context.per_frame[*image].queue_submit_fence);
	}

	if (context.per_frame[*image].primary_command_pool)
	{
		context.device.resetCommandPool(context.per_frame[*image].primary_command_pool);
	}

	// Recycle the old semaphore back into the semaphore manager.
	vk::Semaphore old_semaphore = context.per_frame[*image].swapchain_acquire_semaphore;

	if (old_semaphore)
	{
		context.recycled_semaphores.push_back(old_semaphore);
	}

	context.per_frame[*image].swapchain_acquire_semaphore = acquire_semaphore;

	return vk::Result::eSuccess;
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
		LOGW("Validation Layer: Warning: {}: {}", layer_prefix, message);
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




bool VulkanHPP::ValidateExtensions(const std::vector<const char*>& required, const std::vector<vk::ExtensionProperties>& available)
{
	// inner find_if gives true if the extension was not found
	// outer find_if gives true if none of the extensions were not found, that is if all extensions were found
	auto requiredButNotFoundIt = std::find_if(required.begin(),
		required.end(),
		[&available](auto extension) {
			return std::find_if(available.begin(),
				available.end(),
				[&extension](auto const& ep) {
					return strcmp(ep.extensionName, extension) == 0;
				}) == available.end();
		});

	if (requiredButNotFoundIt != required.end())
	{
		LOGE("Validation Layer {} not found", *requiredButNotFoundIt);
	}
	return  requiredButNotFoundIt == required.end();
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


void VulkanHPP::TeardownPerFrame(Context& context, PerFrame& per_frame)
{
	if (per_frame.queue_submit_fence)
	{
		context.device.destroyFence(per_frame.queue_submit_fence);

		per_frame.queue_submit_fence = nullptr;
	}

	if (per_frame.primary_command_buffer)
	{
		context.device.freeCommandBuffers(per_frame.primary_command_pool, per_frame.primary_command_buffer);

		per_frame.primary_command_buffer = nullptr;
	}

	if (per_frame.primary_command_pool)
	{
		context.device.destroyCommandPool(per_frame.primary_command_pool);

		per_frame.primary_command_pool = nullptr;
	}

	if (per_frame.swapchain_acquire_semaphore)
	{
		context.device.destroySemaphore(per_frame.swapchain_acquire_semaphore);

		per_frame.swapchain_acquire_semaphore = nullptr;
	}

	if (per_frame.swapchain_release_semaphore)
	{
		context.device.destroySemaphore(per_frame.swapchain_release_semaphore);

		per_frame.swapchain_release_semaphore = nullptr;
	}

	per_frame.device = nullptr;
	per_frame.queue_index = -1;
}



void VulkanHPP::InitPerFrame(Context& context, PerFrame& per_frame)
{
	per_frame.queue_submit_fence = context.device.createFence({ vk::FenceCreateFlagBits::eSignaled });

	vk::CommandPoolCreateInfo cmd_pool_info(vk::CommandPoolCreateFlagBits::eTransient, context.graphics_queue_index);
	per_frame.primary_command_pool = context.device.createCommandPool(cmd_pool_info);

	vk::CommandBufferAllocateInfo cmd_buf_info(per_frame.primary_command_pool, vk::CommandBufferLevel::ePrimary, 1);
	per_frame.primary_command_buffer = context.device.allocateCommandBuffers(cmd_buf_info).front();

	per_frame.device = context.device;
	per_frame.queue_index = context.graphics_queue_index;
}




VkShaderStageFlagBits  find_shader_stage(const std::string& ext)
{
	if (ext == "vert")
	{
		return VK_SHADER_STAGE_VERTEX_BIT;
	}
	else if (ext == "frag")
	{
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	else if (ext == "comp")
	{
		return VK_SHADER_STAGE_COMPUTE_BIT;
	}
	else if (ext == "geom")
	{
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	else if (ext == "tesc")
	{
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	}
	else if (ext == "tese")
	{
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}

	throw std::runtime_error("No Vulkan shader stage found for the file extension name.");
};

void VulkanHPP::Resize(const uint32_t, const uint32_t)
{
	if (!m_Context.device)
	{
		return;
	}

	vk::SurfaceCapabilitiesKHR surface_properties = m_Context.gpu.getSurfaceCapabilitiesKHR(m_Context.surface);

	// Only rebuild the swapchain if the dimensions have changed
	if (surface_properties.currentExtent.width == m_Context.swapchain_dimensions.width &&
		surface_properties.currentExtent.height == m_Context.swapchain_dimensions.height)
	{
		return;
	}

	m_Context.device.waitIdle();
	TearDownFramebuffers(m_Context);

	InitSwapchain(m_Context);
	InitFrameBuffers(m_Context);
	SetupDepthStencil(m_Context);
}

void VulkanHPP::TearDownFramebuffers(Context& context)
{
	// Wait until device is idle before teardown.
	context.queue.waitIdle();

	for (auto& framebuffer : context.swapchain_framebuffers)
	{
		context.device.destroyFramebuffer(framebuffer);
	}

	context.swapchain_framebuffers.clear();
}

void VulkanHPP::InitLogger()
{

	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
#ifdef VKB_DEBUG
	logger->set_level(spdlog::level::debug);
#else
	logger->set_level(spdlog::level::info);
#endif

	logger->set_pattern(LOGGER_FORMAT);
	spdlog::set_default_logger(logger);
	LOGI("Logger initialized");

}

bool validate_layers(const std::vector<const char*>& required,
	const std::vector<vk::LayerProperties>& available)
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
		LOGE("Validation Layer {} not found", *requiredButNotFoundIt);
	}
	return (requiredButNotFoundIt == required.end());
}

std::vector<const char*> get_optimal_validation_layers(const std::vector<vk::LayerProperties>& supported_instance_layers)
{
	std::vector<std::vector<const char*>> validation_layer_priority_list =
	{
		// The preferred validation layer is "VK_LAYER_KHRONOS_validation"
		{"VK_LAYER_KHRONOS_validation"},

		// Otherwise we fallback to using the LunarG meta layer
		{"VK_LAYER_LUNARG_standard_validation"},

		// Otherwise we attempt to enable the individual layers that compose the LunarG meta layer since it doesn't exist
		{
			"VK_LAYER_GOOGLE_threading",
			"VK_LAYER_LUNARG_parameter_validation",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_core_validation",
			"VK_LAYER_GOOGLE_unique_objects",
		},

		// Otherwise as a last resort we fallback to attempting to enable the LunarG core layer
		{"VK_LAYER_LUNARG_core_validation"} };

	for (auto& validation_layers : validation_layer_priority_list)
	{
		if (validate_layers(validation_layers, supported_instance_layers))
		{
			return validation_layers;
		}

		LOGW("Couldn't enable validation layers (see log for error) - falling back");
	}

	// Else return nothing
	return {};
}

void MouseHandler(GLFWwindow* window, int button, int action, int mods)
{
	if (VulkanHPP* vulkanHPP = reinterpret_cast<VulkanHPP*>(glfwGetWindowUserPointer(window)))
	{
		if (action == GLFW_PRESS) vulkanHPP->MousePress(button, mods);
		if (action == GLFW_RELEASE) vulkanHPP->MouseRelease(button, mods);
	}
}

void MouseMoveHandler(GLFWwindow* window, double posx, double posy) {
	if (VulkanHPP* vulkanHPP = reinterpret_cast<VulkanHPP*>(glfwGetWindowUserPointer(window)))
	{
		float newX = static_cast<float>(posx);
		float newY = static_cast<float>(posy);
		vulkanHPP->MouseMoved(newX, newY);
	}
}

Image VulkanHPP::LoadImageFromFile(const std::string& path)
{
	int texWidth; 
	int  texHeight;
	int texChannels; 

	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	//std::byte* pixels = nullptr;
	if (!pixels) {
		LOGE("Failed to load texture file {}", path);
	}

	void* pixel_ptr = pixels;
	VkDeviceSize imageSize = texWidth * texHeight * 4ll;

	AllocatedBuffer stagingBuffer = CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	VK_CHECK(vmaMapMemory(m_Allocator, stagingBuffer._allocation, &data));

	memcpy(data, pixel_ptr, static_cast<size_t>(imageSize));

	vmaUnmapMemory(m_Allocator, stagingBuffer._allocation);

	//stbi_image_free(pixels);

	vk::Extent3D imageExtent;
	imageExtent.width = static_cast<uint32_t>(texWidth);
	imageExtent.height = static_cast<uint32_t>(texHeight);
	imageExtent.depth = 1;

	vk::Format image_format = vk::Format::eR8G8B8A8Srgb;	
	vk::ImageCreateInfo dimg_info;// = vkinit::image_create_info(image_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);
	dimg_info.format = image_format;	
	dimg_info.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
	dimg_info.extent = imageExtent;
	dimg_info.imageType = vk::ImageType::e2D;
	dimg_info.mipLevels = 1;	
	dimg_info.arrayLayers = 1;	
	dimg_info.samples = vk::SampleCountFlagBits::e1;	
	dimg_info.tiling = vk::ImageTiling::eOptimal;

	Image newImage;
	newImage.extent = imageExtent;
	
	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	//allocate and create the image
	VK_CHECK(vmaCreateImage(m_Allocator, reinterpret_cast<VkImageCreateInfo*>(&dimg_info),
		&dimg_allocinfo, reinterpret_cast<VkImage*>(&newImage.image), &newImage.allocation, nullptr));

	WithPrimaryCommandBuffer([&](auto& cmd)
		{
			VkImageSubresourceRange range;
			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			range.baseMipLevel = 0;
			range.levelCount = 1;
			range.baseArrayLayer = 0;
			range.layerCount = 1;

			VkImageMemoryBarrier imageBarrier_toTransfer = {};
			imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

			imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toTransfer.image = newImage.image;
			imageBarrier_toTransfer.subresourceRange = range;

			imageBarrier_toTransfer.srcAccessMask = 0;
			imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			//barrier the image into the transfer-receive layout
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;

			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageExtent = imageExtent;

			//copy the buffer into the image
			vkCmdCopyBufferToImage(cmd, stagingBuffer._buffer, newImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

			VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

			imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			//barrier the image into the shader readable layout
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
		});
	 
	vmaDestroyBuffer(m_Allocator, stagingBuffer._buffer, stagingBuffer._allocation);
	
	vk::ImageViewCreateInfo imgViewInfo;
	imgViewInfo.format = image_format;
	imgViewInfo.image = newImage.image;
	imgViewInfo.viewType = vk::ImageViewType::e2D;
	imgViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;// VK_IMAGE_ASPECT_COLOR_BIT;
	imgViewInfo.subresourceRange.baseMipLevel = 0;
	imgViewInfo.subresourceRange.levelCount = 1;
	imgViewInfo.subresourceRange.baseArrayLayer = 0;
	imgViewInfo.subresourceRange.layerCount = 1;


	auto view = m_Context.device.createImageView(imgViewInfo);
	
	newImage.view = view;

	
	vk::SamplerCreateInfo samplerCreateInfo;
	samplerCreateInfo.magFilter = vk::Filter::eLinear;
	samplerCreateInfo.minFilter = vk::Filter::eLinear;
	samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	// Max level-of-detail should match mip level count
	//samplerCreateInfo.maxLod = static_cast<float>(1);
	// Only enable anisotropic filtering if enabled on the devicec
	samplerCreateInfo.maxAnisotropy = 0;// m_Context.deviceFeatures.samplerAnisotropy ? m_Context.deviceProperties.limits.maxSamplerAnisotropy : 1.0f;
	samplerCreateInfo.anisotropyEnable = VK_FALSE; //context.deviceFeatures.samplerAnisotropy;
	samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;
	
	auto sampler = m_Context.device.createSampler(samplerCreateInfo);
	newImage.sampler = sampler;

	newImage.descriptor.sampler = sampler;
	newImage.descriptor.imageView = view;
	newImage.descriptor.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;



	return newImage;
}