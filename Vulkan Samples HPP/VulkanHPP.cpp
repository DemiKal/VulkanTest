#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VKB_DEBUG 1
#define VK_USE_PLATFORM_WIN32_KHR 1
#define VMA_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define VKB_VALIDATION_LAYERS
#define GLFW_DLL
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define GLFW_INCLUDE_VULKAN

#include "vk_mem_alloc.h"
#include "VulkanHPP.h"
#include "vertexbuffer.h"
#include <typeinfo>
#include <variant>
#include <tuple>

__pragma(warning(push, 0))
#include "tiny_gltf.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shaderc/shaderc.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
__pragma(warning(pop))


#define LOGGER_FORMAT "[%^%l%$] %v"
#define ROOT_PATH_SIZE 48
#define __FILENAME__ (static_cast<const char *>(__FILE__) + ROOT_PATH_SIZE)

#define LOGI(...) spdlog::info(__VA_ARGS__);
#define LOGW(...) spdlog::warn(__VA_ARGS__);
#define LOGE(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(__VA_ARGS__));
#define LOGD(...) spdlog::debug(__VA_ARGS__);

#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout << "Detected Vulkan error: " << err << std::endl; \
			 											\
			abort();                                                \
		}                                                           \
	} while (0)


std::vector<const char*> get_optimal_validation_layers(const std::vector<vk::LayerProperties>& supported_instance_layers);


struct AllocatedBuffer {
	VkBuffer _buffer;
	VmaAllocation _allocation;
};

struct Mesh
{
	IndexBuffer indexBuffer;
	Buffer vertexBuffer;
};

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type, uint64_t object, size_t location, int32_t message_code, const char* layer_prefix, const char* message, void* user_data);
void window_size_callback(GLFWwindow* window, int width, int height);


struct UBO
{
	glm::mat4 proj;
	glm::mat4 view = glm::lookAt(glm::vec3(-5.0f, 3.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}ubo_vs;

void VulkanHPP::UpdateUniformBuffer(float dt)
{
	static size_t tick = 0;
	tick++;
	float aspect = (float)m_Width / (float)m_Height;
	ubo_vs.proj = glm::perspective(glm::radians(70.0f), aspect, 0.0f, 100.0f);
	ubo_vs.view = glm::translate(ubo_vs.view, glm::vec3(0, 0.1f, 0) * dt);
	// Fixed ubo with projection and view matrices
	//ubo_vs.projection = camera.projection;
	//ubo_vs.view = camera.view;
	//
	m_Uniform->convert_and_update(m_Allocator, ubo_vs);
}

int LoadModel()
{
	std::string filename = "../Assets/DamagedHelmet.glb";
	//using namespace tinygltf;

	tinygltf::Model model;

	return 0;
}


static Mesh triMesh;


#pragma region templatememes
template<auto N>
struct TestExtraction {

};
template<auto arg_N>
struct val {
	static constexpr auto N = arg_N;
};

template<template <auto> typename T, auto N>
constexpr auto extract(const T<N>&)->val<N>;

template<typename T>
constexpr auto extract_N = decltype(extract(std::declval<T>()))::N;

constexpr auto ffsd = extract_N<TestExtraction<5>>;

template<typename...Ts>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

template<typename...Ts>
using remove_t = tuple_cat_t<
	typename std::conditional<
	//IsValidAttrib<Ts>(),
	//!Ts::enabled(),
	!extract_N<Ts>,
	std::tuple<>,   //<-- if true
	std::tuple<Ts>  //<-- if not
	>::type...
>;


template <class T, template <class> class OtherType>
class Nest
{
	OtherType<T> f;
};


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



void VulkanHPP::Prepare()
{
	//camera.model = glm::mat4(1.0f);
	//camera.view = glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f)));
	//auto aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	//camera.projection = glm::perspective(glm::radians(60.0f), aspect, 1.f, 256.f);

	//glm::persp
	constexpr AttributeVariant  attr2 = PositionAttribute(2, 1, 3);
	constexpr AttributeVariant  attr1 = ColorAttribute(2, 1, 3);
	constexpr AttributeVariant  attr3 = TexCoordAttribute(2, 1);
	Buffer vb;

	vb.AddAttribute(PositionAttribute{});
	vb.AddAttribute(ColorAttribute{});
	vb.Finalize();
	//	{ { 1.0f, 1.0f, 0.0f },  { 1.0f, 0.0f, 0.0f } },
	//	{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
	//	{ { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
	//PositionAttribute vv =		PositionAttribute{ -1,  1, 0 } *0.5f;


	PositionAttribute v0p = PositionAttribute{ 1.0f, 1.0f, 0.0f };
	ColorAttribute v0c = ColorAttribute{ 1.0f, 0.0f, 0.0f };
	PositionAttribute v1p = PositionAttribute{ -1.0f, 1.0f, 0.0f };
	ColorAttribute v1c = ColorAttribute{ 0.0f, 1.0f, 0.0f };
	PositionAttribute v2p = PositionAttribute{ 0.0f, -1.0f, 0.0f };
	ColorAttribute v2c = ColorAttribute{ 0.0f, 0.0f, 1.0f };
	//PositionAttribute v3p = PositionAttribute{ -1, -1, 0 };
	//ColorAttribute v3c = ColorAttribute{ 1, 1, 0 };

	vb.AddElement(v0p);
	vb.AddElement(v0c);
	vb.AddElement(v1p);
	vb.AddElement(v1c);
	vb.AddElement(v2p);
	vb.AddElement(v2c);
	//vb.AddElement(v3p);
	//vb.AddElement(v3c);





	vb.AddElement(BoneWeight{ 0.0f, 0.0f, 1.0f });
	//vb.AddElement(BoneIndex(1, 2, 3));
	//vb.AddElement(BoneIndex(4, 5, 6));
	//interleave!


	auto a1 = vb.GetAttribute<PositionAttribute>(0);
	auto b1 = vb.GetAttribute<ColorAttribute>(0);
	auto a2 = vb.GetAttribute<PositionAttribute>(1);
	auto b2 = vb.GetAttribute<ColorAttribute>(1);
	auto a3 = vb.GetAttribute<PositionAttribute>(2);
	auto b3 = vb.GetAttribute<ColorAttribute>(2);
	auto b4 = vb.GetAttribute<ColorAttribute>(3);

	IndexBuffer ib;
	ib.AddElement(Indices{ 0 , 1 , 2 });
	//ib.AddElement(Indices{2 , 3 , 0});

	triMesh.vertexBuffer = vb;
	triMesh.indexBuffer = ib;



	using T = BoneIndex;
	auto* ptr = vb.GetAttribute<T>(0);
	if (ptr)
	{
		ptr->x = 112.0f;
		ptr->y = 867.0f;
		ptr->z = 991.0f;


		auto* vvv = reinterpret_cast<T*>(ptr);

		*vvv = T(100, 128, 256);;
	}
	auto* ptr2 = vb.GetAttribute<BoneIndex>(0);


	//vb.emplace_back(attr1);
	//vb.emplace_back(attr2);
	//vb.emplace_back(attr3);
	auto strd = vb.TotalStride();
	auto& vb1 = vb.VertexAttributes[0];

	LoadModel();
	InitLogger();
	InitWindow();
	InitInstance(m_Context, { VK_KHR_SURFACE_EXTENSION_NAME }, {});
	SelectPhysicalDeviceAndInstance(m_Context);

	m_Context.swapchain_dimensions.width = m_Width;
	m_Context.swapchain_dimensions.height = m_Height;

	InitDevice(m_Context, { VK_KHR_SWAPCHAIN_EXTENSION_NAME });
	InitSwapchain(m_Context);
	InitRenderPass(m_Context);
	InitAllocator(m_Context);
	InitUniformBuffer(m_Context);
	InitDescriptorPool(m_Context);
	InitDescriptorSetLayout(m_Context);
	SetupDescriptorSet(m_Context);
	InitPipeline(m_Context);
	InitFrameBuffers(m_Context);

	InitVertices(m_Context);
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

	vk::WriteDescriptorSet writeDescriptorSet;
	// Binding 0 : Uniform buffer
	writeDescriptorSet.dstSet = context.descriptorSet;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = vk::DescriptorType::eUniformBuffer;
	writeDescriptorSet.pBufferInfo = &m_Uniform->m_DescrBufferInfo;
	// Binds this uniform buffer to binding point 0
	writeDescriptorSet.dstBinding = 0;


	//writeDescriptorSet.pBufferInfo = &descrBufferInfo;
	//writeDescriptorSet.setBufferInfo()
	context.device.updateDescriptorSets(writeDescriptorSet, nullptr);
}

void VulkanHPP::InitDescriptorPool(Context& context)
{
	// We need to tell the API the number of max. requested descriptors per type
	vk::DescriptorPoolSize typeCounts[1];
	// This example only uses one descriptor type (uniform buffer) and only
	// requests one descriptor of this type
	typeCounts[0].type = vk::DescriptorType::eUniformBuffer;
	typeCounts[0].descriptorCount = 1;
	// For additional types you need to add new entries in the type count list
	// E.g. for two combined image samplers :
	// typeCounts[1].type = vk::DescriptorType::eCombinedImageSampler;
	// typeCounts[1].descriptorCount = 2;

	// Create the global descriptor pool
	// All descriptors used in this example are allocated from this pool
	vk::DescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.poolSizeCount = 1;
	descriptorPoolInfo.pPoolSizes = typeCounts;
	// Set the max. number of sets that can be requested
	// Requesting descriptors beyond maxSets will result in an error
	descriptorPoolInfo.maxSets = 1;
	context.descriptorPool = context.device.createDescriptorPool(descriptorPoolInfo);
}

void VulkanHPP::InitDescriptorSetLayout(Context& context)
{
	static vk::DescriptorSetLayoutBinding layoutBinding;
	layoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
	layoutBinding.pImmutableSamplers = nullptr;
	layoutBinding.binding = 0;

	static vk::DescriptorSetLayoutCreateInfo descriptorLayout;
	descriptorLayout.bindingCount = 1;
	descriptorLayout.pBindings = &layoutBinding;

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

	auto OBJECT_INSTANCES = 1;
	size_t min_ubo_alignment = context.gpu.getProperties().limits.minUniformBufferOffsetAlignment;// get_device().get_gpu().get_properties().limits.minUniformBufferOffsetAlignment;
	size_t dynamicAlignment = sizeof(glm::mat4);
	if (min_ubo_alignment > 0)
	{
		dynamicAlignment = (dynamicAlignment + min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
	}

	size_t buffer_size = OBJECT_INSTANCES * dynamicAlignment;

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
	StageBuffer(context, triMesh.vertexBuffer, vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_GPU_ONLY);
	StageBuffer(context, triMesh.indexBuffer, vk::BufferUsageFlagBits::eIndexBuffer, VMA_MEMORY_USAGE_GPU_ONLY);
}

void VulkanHPP::InitWindow()
{
	vk::ApplicationInfo appInfo("Hello Triangle", VK_MAKE_VERSION(1, 0, 0), "No Engine",
		VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);


	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_GLFWwindow = glfwCreateWindow(m_Width, m_Height, "Vulkan 101", nullptr, nullptr);
	glfwSetWindowUserPointer(m_GLFWwindow, this);
	glfwSetWindowSizeCallback(m_GLFWwindow, window_size_callback);
}


void VulkanHPP::InitInstance(Context& context, const std::vector<const char*>& required_instance_extensions, const std::vector<const char*>& required_validation_layers)
{
	LOGI("Initializing Vulkan");

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
	vk::AttachmentDescription attachment;
	// Backbuffer format.
	attachment.format = context.swapchain_dimensions.format;
	// Not multisampled.
	attachment.samples = vk::SampleCountFlagBits::e1;
	// When starting the frame, we want tiles to be cleared.
	attachment.loadOp = vk::AttachmentLoadOp::eClear;
	// When ending the frame, we want tiles to be written out.
	attachment.storeOp = vk::AttachmentStoreOp::eStore;
	// Don't care about stencil since we're not using it.
	attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

	// The image layout will be undefined when the render pass begins.
	attachment.initialLayout = vk::ImageLayout::eUndefined;
	// After the render pass is complete, we will transition to ePresentSrcKHR layout.
	attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	// We have one subpass. This subpass has one color attachment.
	// While executing this subpass, the attachment will be in attachment optimal layout.
	vk::AttachmentReference color_ref(0, vk::ImageLayout::eColorAttachmentOptimal);

	// We will end up with two transitions.
	// The first one happens right before we start subpass #0, where
	// eUndefined is transitioned into eColorAttachmentOptimal.
	// The final layout in the render pass attachment states ePresentSrcKHR, so we
	// will get a final transition from eColorAttachmentOptimal to ePresetSrcKHR.
	vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, {}, color_ref);

	// Create a dependency to external events.
	// We need to wait for the WSI semaphore to signal.
	// Only pipeline stages which depend on eColorAttachmentOutput will
	// actually wait for the semaphore, so we must also wait for that pipeline stage.
	vk::SubpassDependency dependency;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	// Since we changed the image layout, we need to make the memory visible to
	// color attachment to modify.
	dependency.srcAccessMask = {};
	dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

	// Finally, create the renderpass.
	vk::RenderPassCreateInfo rp_info({}, attachment, subpass, dependency);

	context.render_pass = context.device.createRenderPass(rp_info);
}

void VulkanHPP::InitPipeline(Context& context)
{
	// Create a blank pipeline layout.
	// We are not binding any resources to the pipeline in this first sample.
	context.pipelineLayout;// = context.device.createPipelineLayout({});


	vk::PipelineVertexInputStateCreateInfo vertexInput;

	auto bindingDescriptions = triMesh.vertexBuffer.GetVertexInputBindingDescriptions();
	vertexInput.setVertexBindingDescriptions({ (uint32_t)bindingDescriptions.size(), bindingDescriptions.data() });

	auto attributeDescriptions = triMesh.vertexBuffer.GetVertexInputAttributeDescriptions();
	vertexInput.setVertexAttributeDescriptions({ (uint32_t)attributeDescriptions.size(), attributeDescriptions.data() });

	// Specify we will use triangle lists to draw geometry.
	vk::PipelineInputAssemblyStateCreateInfo input_assembly({}, vk::PrimitiveTopology::eTriangleList);

	// Specify rasterization state.
	vk::PipelineRasterizationStateCreateInfo raster;
	raster.cullMode = vk::CullModeFlagBits::eNone;
	raster.frontFace = vk::FrontFace::eClockwise;
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

	// No multisampling.
	vk::PipelineMultisampleStateCreateInfo multisample({}, vk::SampleCountFlagBits::e1);

	// Specify that these states will be dynamic, i.e. not part of pipeline state object.
	std::array<vk::DynamicState, 2> dynamics{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamic({}, dynamics);

	const std::string kShaderSource = R"vertexShader(
	#version 450

	layout(location = 0) in vec3 inPos;
	layout(location = 1) in vec3 inColor;

	layout(binding = 0) uniform UBO
	{
		mat4 projectionMatrix;
		mat4 viewMatrix;
	} ubo;

	layout(location = 0) out vec3 outColor;

	out gl_PerVertex
	{
		vec4 gl_Position;
	};


	void main()
	{
		outColor = inColor;
		gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vec4(inPos.xyz, 1.0);
	}
)vertexShader";

	const std::string fragmentShader = R"fragmentShader(
#version 450

layout(location = 0) in vec3 in_color;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(in_color, 1.0);
}

)fragmentShader";

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetOptimizationLevel(shaderc_optimization_level_performance);

	shaderc::SpvCompilationResult vertShaderResult =
		compiler.CompileGlslToSpv(kShaderSource, shaderc_glsl_vertex_shader, "vertex shader", options);

	auto vertStatus = vertShaderResult.GetCompilationStatus();
	if (vertStatus != shaderc_compilation_status_success) { fmt::print(vertShaderResult.GetErrorMessage()); }

	auto vertShaderCode = std::vector<uint32_t>{ vertShaderResult.cbegin(), vertShaderResult.cend() };
	auto vertSize = std::distance(vertShaderCode.begin(), vertShaderCode.end());
	auto vertShaderCreateInfo = vk::ShaderModuleCreateInfo{ {}, vertSize * sizeof(uint32_t), vertShaderCode.data() };
	auto vertexShaderModule = context.device.createShaderModule(vertShaderCreateInfo);

	shaderc::SpvCompilationResult fragShaderModule = compiler.CompileGlslToSpv(fragmentShader, shaderc_glsl_fragment_shader, "fragment shader", options);
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

	// Create framebuffer for each swapchain image view
	for (auto& image_view : context.swapchain_image_views)
	{
		// Build the framebuffer.
		vk::FramebufferCreateInfo fb_info({}, context.render_pass, image_view, context.swapchain_dimensions.width, context.swapchain_dimensions.height, 1);

		context.swapchain_framebuffers.push_back(device.createFramebuffer(fb_info));
	}
}


void VulkanHPP::RunLoop()
{
	constexpr float deltaTime = 1.f / 60.f;
	while (!glfwWindowShouldClose(m_GLFWwindow))
	{
		glfwPollEvents();
		Update(deltaTime);
	}
}



void window_size_callback(GLFWwindow* window, int width, int height)
{
	if (auto glfw_window = reinterpret_cast<VulkanHPP*>(glfwGetWindowUserPointer(window)))

	{
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
	vk::ClearValue clear_value;
	clear_value.color = vk::ClearColorValue(std::array<float, 4>({ {0.1f, 0.1f, 0.2f, 1.0f} }));

	// Begin the render pass.
	vk::RenderPassBeginInfo rp_begin(context.render_pass, framebuffer, { {0, 0}, {context.swapchain_dimensions.width, context.swapchain_dimensions.height} },
		clear_value);
	// We will add draw commands in the same command buffer.
	cmd.beginRenderPass(rp_begin, vk::SubpassContents::eInline);

	cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, context.pipelineLayout, 0, context.descriptorSet, nullptr);

	// Bind the graphics pipeline.
	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, context.pipeline);
	vk::Buffer buffer{ triMesh.vertexBuffer.vkBuffer };
	cmd.bindVertexBuffers(0, buffer, { 0 });

	cmd.bindIndexBuffer(triMesh.indexBuffer.vkBuffer, 0, vk::IndexType::eUint32);


	vk::Viewport vp(0.0f, 0.0f, static_cast<float>(context.swapchain_dimensions.width), static_cast<float>(context.swapchain_dimensions.height), 0.0f, 1.0f);
	// Set viewport dynamically
	cmd.setViewport(0, vp);

	vk::Rect2D scissor({ 0, 0 }, { context.swapchain_dimensions.width, context.swapchain_dimensions.height });
	// Set scissor dynamically
	cmd.setScissor(0, scissor);

	// Draw three vertices with one instance.
	//cmd.draw(3, 1, 0, 0);
	size_t vertexCount = triMesh.vertexBuffer.GetVertexCount();
	size_t indexCount = triMesh.indexBuffer.GetVertexCount();

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