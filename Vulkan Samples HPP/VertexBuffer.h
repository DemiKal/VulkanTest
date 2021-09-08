#pragma once
#include <variant>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <fmt/format.h>
#include "vk_mem_alloc.h"
//#include "VulkanHPP.h"
#include <glm/gtc/matrix_transform.hpp>
#include "logger.hpp"

struct Context;
struct UniformBuffer
{
	size_t dynamicAlignment;
	uint8_t* mapped_data;
	size_t size;
	bool persistent;
	vk::Buffer handle;
	VmaAllocation allocation;
	//VkDeviceMemory memory;
	vk::DeviceMemory memory;
	//VmaAllocator m_vmaAllocator_ref;
	vk::DescriptorBufferInfo m_DescrBufferInfo;

	UniformBuffer(
		vk::Device& device,
		VkDeviceSize size,
		VmaAllocator vmaAllocator,
		VkBufferUsageFlags buffer_usage,
		VmaMemoryUsage memory_usage,
		VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_MAPPED_BIT) :
		size{ size }
	{
		//struct ubo
		//{
		//	glm::mat4 proj;
		//	glm::mat4 view;
		//}ubo;
	   //
		persistent = (flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0;

		VkBufferCreateInfo buffer_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_info.usage = buffer_usage;
		buffer_info.size = size;

		VmaAllocationCreateInfo memory_info{};
		memory_info.flags = flags;
		memory_info.usage = memory_usage;

		VmaAllocationInfo allocation_info{};
		auto              result = vmaCreateBuffer(vmaAllocator,
			&buffer_info, &memory_info,
			reinterpret_cast<VkBuffer*>(&handle), &allocation,
			&allocation_info);

		if (result != VK_SUCCESS)
		{
			throw std::exception{ "Cannot create Buffer" };
		}

		memory = static_cast<vk::DeviceMemory>(allocation_info.deviceMemory);
		if (persistent)
		{
			mapped_data = static_cast<uint8_t*>(allocation_info.pMappedData);
		}
		//auto res = vmaMapMemory(vmaAllocator, allocation, (void**)&mapped_data);
		//device.bindBufferMemory(handle, memory, 0);
		m_DescrBufferInfo = vk::DescriptorBufferInfo(handle, 0, VK_WHOLE_SIZE);
		
		//glm::mat4x4 model = glm::mat4x4(1.0f);
		//ubo.view =
		//	glm::lookAt(glm::vec3(-5.0f, 3.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		//ubo.proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		// clang-format off
		//	glm::mat4x4 clip = glm::mat4x4(1.0f, 0.0f, 0.0f, 0.0f,
		//	0.0f, -1.0f, 0.0f, 0.0f,
		//	0.0f, 0.0f, 0.5f, 0.0f,
		//	0.0f, 0.0f, 0.5f, 1.0f);  // vulkan clip space has inverted y and half z !
		//glm::mat4x4 mvpc = projection * view * model;
		//vk::Buffer uniformDataBuffer = device.createBuffer(
		//	vk::BufferCreateInfo(vk::BufferCreateFlags(), sizeof(ubo), vk::BufferUsageFlagBits::eUniformBuffer));
		//handle = uniformDataBuffer;
		//vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements(uniformDataBuffer);
		//uint32_t typeIndex = 8;
		////uint32_t               typeIndex =
		////	vk::su::findMemoryType(physicalDevice.getMemoryProperties(),
		////		memoryRequirements.memoryTypeBits,
		////		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		//vk::DeviceMemory uniformDataMemory =
		//	device.allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size, typeIndex));
		//mapped_data = static_cast<uint8_t*>(device.mapMemory(uniformDataMemory, 0, memoryRequirements.size));
		//memcpy(mapped_data, &ubo, sizeof(ubo));
		////device.unmapMemory(uniformDataMemory);
		//
		//m_DescrBufferInfo.buffer = handle;
		//m_DescrBufferInfo.offset = 0;
		//m_DescrBufferInfo.range = VK_WHOLE_SIZE;
		//
		//device.bindBufferMemory(uniformDataBuffer, uniformDataMemory, 0);
	}

	template <class T>
	void convert_and_update(VmaAllocator vmaAlloc, const T& object, size_t offset = 0)
	{
		update(vmaAlloc, reinterpret_cast<const uint8_t*>(&object), sizeof(T), offset);
	}

	void update(VmaAllocator vmaAlloc, const uint8_t* data, const size_t size, const size_t offset)
	{

		if (persistent)
		{
			std::copy(data, data + size, mapped_data + offset);
			Flush(vmaAlloc);
		}
		else
		{
			//map();
			std::copy(data, data + size, mapped_data + offset);
			Flush(vmaAlloc);
			//unmap();
		}
	}

	void Flush(VmaAllocator vmaAlloc)
	{
		vmaFlushAllocation(vmaAlloc, allocation, 0, size);
	}

};

template<typename T  >
struct VertexAttributeNew : T  //todo: add normalization?
{
	using T::T; //inherit constructor
	//template<typename ...Args>
	//VertexAttributeNew(Args&& ...args) : T(std::forward<Args^>(args )...)



	//static constexpr size_t Idx = 1;
	static constexpr size_t Stride() { return T::length() * sizeof(T::value_type); }
	static constexpr size_t Length() { return T::length(); }
	static constexpr vk::Format GetFormat()
	{
		if constexpr (std::is_same_v<T, glm::vec1>) return  vk::Format::eR32Sfloat;
		if constexpr (std::is_same_v<T, glm::vec2>) return  vk::Format::eR32G32Sfloat;
		if constexpr (std::is_same_v<T, glm::vec3>) return  vk::Format::eR32G32B32Sfloat;
		if constexpr (std::is_same_v<T, glm::vec4>) return  vk::Format::eR32G32B32A32Sfloat;

		if constexpr (std::is_same_v<T, glm::ivec1>) return  vk::Format::eR32Sint;
		if constexpr (std::is_same_v<T, glm::ivec2>) return  vk::Format::eR32G32Sint;
		if constexpr (std::is_same_v<T, glm::ivec3>) return  vk::Format::eR32G32B32Sint;
		if constexpr (std::is_same_v<T, glm::ivec4>) return  vk::Format::eR32G32B32A32Sint;

		if constexpr (std::is_same_v<T, glm::uvec1>) return  vk::Format::eR32Uint;
		if constexpr (std::is_same_v<T, glm::uvec2>) return  vk::Format::eR32G32Uint;
		if constexpr (std::is_same_v<T, glm::uvec3>) return  vk::Format::eR32G32B32Uint;
		if constexpr (std::is_same_v<T, glm::uvec4>) return  vk::Format::eR32G32B32A32Uint;
	}
};

#define INHERIT_CONSTRUCTOR using VertexAttributeNew :: VertexAttributeNew;
//make bone weights static
static constexpr int BoneIndexCount = 3;
struct PositionAttribute : VertexAttributeNew<glm::vec3> { INHERIT_CONSTRUCTOR };
struct ColorAttribute : VertexAttributeNew<glm::vec3> { INHERIT_CONSTRUCTOR };
struct TexCoordAttribute : VertexAttributeNew<glm::vec2> { INHERIT_CONSTRUCTOR };
struct BitangentAttribute : VertexAttributeNew<glm::vec3> { INHERIT_CONSTRUCTOR };
struct TangentAttribute : VertexAttributeNew<glm::vec3> { INHERIT_CONSTRUCTOR };
struct NormalAttribute : VertexAttributeNew<glm::vec3> { INHERIT_CONSTRUCTOR };
struct BoneWeightAttribute : VertexAttributeNew<glm::vec<BoneIndexCount, float>> { INHERIT_CONSTRUCTOR };
struct BoneIndexAttribute : VertexAttributeNew<glm::vec<BoneIndexCount, uint32_t>> { INHERIT_CONSTRUCTOR };

 template< template <class...> typename T, int N  >
 struct Boneseee : VertexAttributeNew<glm::vec<N, T>> 
 { 
	 using VertexAttributeNew<glm::vec<N, T>>::VertexAttributeNew;
 };
 
//Boneseee<glm::vec<3, uint32_t>> fasdas;
//template <typename N, template<class...> class V>
//template <int N>
struct IndexAttribute : VertexAttributeNew <glm::uvec3>
{
	INHERIT_CONSTRUCTOR
};


//struct Indices4 : VertexAttributeNew<glm::uvec2> { INHERIT_CONSTRUCTOR };

//no padding allowed!
static_assert(sizeof(glm::vec1) == sizeof(VertexAttributeNew<glm::vec1>));
static_assert(sizeof(glm::vec2) == sizeof(VertexAttributeNew<glm::vec2>));
static_assert(sizeof(glm::vec3) == sizeof(VertexAttributeNew<glm::vec3>));
static_assert(sizeof(glm::vec4) == sizeof(VertexAttributeNew<glm::vec4>));

static_assert(sizeof(glm::ivec1) == sizeof(VertexAttributeNew<glm::ivec1>));
static_assert(sizeof(glm::ivec2) == sizeof(VertexAttributeNew<glm::ivec2>));
static_assert(sizeof(glm::ivec3) == sizeof(VertexAttributeNew<glm::ivec3>));
static_assert(sizeof(glm::ivec4) == sizeof(VertexAttributeNew<glm::ivec4>));

static_assert(sizeof(glm::dvec1) == sizeof(VertexAttributeNew<glm::dvec1>));
static_assert(sizeof(glm::dvec2) == sizeof(VertexAttributeNew<glm::dvec2>));
static_assert(sizeof(glm::dvec3) == sizeof(VertexAttributeNew<glm::dvec3>));
static_assert(sizeof(glm::dvec4) == sizeof(VertexAttributeNew<glm::dvec4>));

static_assert(sizeof(glm::vec<BoneIndexCount, float>) == sizeof(VertexAttributeNew < glm::vec<BoneIndexCount, float>>));
static_assert(sizeof(glm::vec<BoneIndexCount, int>) == sizeof(VertexAttributeNew < glm::vec<BoneIndexCount, int>>));

#define ATTRIBUTE_TYPES  PositionAttribute, ColorAttribute, TexCoordAttribute, IndexAttribute ,  BitangentAttribute, TangentAttribute, NormalAttribute, BoneIndexAttribute, BoneWeightAttribute 
using AttributeVariant = std::variant<ATTRIBUTE_TYPES>;


template<typename T>
constexpr auto GetIndexType(const AttributeVariant& v)
{
	//constexpr size_t i = static_cast<size_t>(nullptr) + static_cast<size_t>(nullptr);
	return std::get_if<T>(&v);
}

struct Buffer //: private std::vector<std::byte>
{
	std::vector<AttributeVariant> VertexAttributes;
	std::vector<std::byte> buffer;
	uint8_t* mapped_data{ nullptr };

	vk::Buffer vkBuffer;
	vk::DeviceMemory DeviceMemory;
	VmaAllocation vmaAllocation;
	vk::BufferUsageFlags usageFlags;
	vk::DescriptorBufferInfo descriptor;
	bool m_IsFinalized = false;

	void AddAttribute(const AttributeVariant& a)
	{
		if (m_IsFinalized) { LOGE("Buffer is finalized!"); return; }


		VertexAttributes.push_back(a);
	}

	void Finalize() { m_IsFinalized = true; }

	//size in bytes
	size_t GetBufferSize() { return buffer.size() * sizeof(std::byte); /*lol*/ }
	size_t GetVertexCount()
	{
		return  buffer.size() / TotalStride();
	}

	//template<typename T, typename ...Args>
	//Check

	template <typename T, typename ... Args>
	T create(Args&& ... args) {
		return T(std::forward<Args>(args)...);
	}


	template<typename T, typename ... Args>
	void AddElement(Args&& ... args)
	{
		//todo check size!
		bool hasType = false;
		for (auto& var : VertexAttributes) //todo: use bitset?
			hasType |= static_cast<bool>(std::get_if<T>(&var));

		if (!hasType)
		{
			LOGE("This vertex does not have this attribute declared!");
			return;
		}

		size_t bufferSize = buffer.size();
		size_t byteIndex = bufferSize;
		size_t byteSize = sizeof(T);

		//if (buffer.size() + byteSize > buffer.capacity())
		{
			buffer.resize(bufferSize + byteSize);
			bufferSize = buffer.size();
		}

		//buffer.reserve(byteSize);
		//std::array < std::byte, T::Stride()> arr;
		T* ptr = reinterpret_cast<T*>(buffer.data() + byteIndex);
		//*ptr = static_cast<T>(std::forward<T>(args)...);
		*ptr = T{ std::forward<Args>(args)... };

		//buffer.insert(buffer.end(), std::begin(arr), std::end(arr));

	}

	//always check for nullptr!
	template<typename T>
	T* GetAttribute(size_t idx)
	{
		auto vertexStride = TotalStride();
		auto capacity = buffer.size() / vertexStride;
		if (idx > capacity - 1) return nullptr;
		std::byte* ptr = nullptr;
		const auto totalStride = idx * vertexStride;

		size_t offset = 0; //build up offset as you traverse
		auto lambda = [&](auto& variant) { return variant.Stride(); };
		for (const auto& v : VertexAttributes)
		{
			const T* castPtr = std::get_if<T>(&v);

			if (castPtr)
			{
				//ptr = castPtr + castPtr.Stride();

				return reinterpret_cast<T*>(buffer.data() + totalStride + offset);
			}

			offset += std::visit(lambda, v);
		}

		return nullptr;
	}

	//Buffer(AttributeVariant&& attr) : 
	//	VertexAttributes{ std::move  (attr) },
	//	vmaAllocation{}, 
	//	vkBuffer{}, 
	//	buffer{}  
	//{}

	Buffer() = default;

	//todo: cache!
	size_t  TotalStride()
	{
		auto CallGenerate = [](auto& p) { return p.Stride(); };
		size_t stride = 0;
		for (auto& v : VertexAttributes)
		{
			stride += std::visit(CallGenerate, v);
		}
		return stride;
	}
	void OffsetOf() {};

	std::vector<vk::VertexInputBindingDescription> GetVertexInputBindingDescriptions(uint32_t binding = 0)
	{
		//std::vector<vk::VertexInputBindingDescription> bindingDescriptions = 
		//{{0, sizeof(Vertex), vk::VertexInputRate::eVertex}};

		return  { {binding, static_cast<uint32_t>(TotalStride()), vk::VertexInputRate::eVertex}, };



	}


	std::vector< vk::VertexInputAttributeDescription> GetVertexInputAttributeDescriptions(uint32_t binding = 0)
	{
		std::vector<vk::VertexInputAttributeDescription>  attributeDescriptions;
		uint32_t offset = 0;
		uint32_t location = 0;
		auto format_lambda = [&](auto& variant) { return variant.GetFormat(); };
		auto stride_lambda = [&](auto& variant) { return variant.Stride(); };

		for (auto& variant : VertexAttributes)
		{
			const vk::Format format = std::visit(format_lambda, variant);
			const auto stride = std::visit(stride_lambda, variant);

			attributeDescriptions.emplace_back(location, binding, format, offset);

			offset += stride;
			location++;
		}

		return attributeDescriptions;
	}


private:
	//std::byte& operator[](int index)
	//{
		//return this[index];
		//
		//auto o =std::byte{8};
		//return o;
	//}
};

class IndexBuffer : public Buffer
{
public:

	IndexBuffer()
	{
		//Buffer(Indices)
		AddAttribute(IndexAttribute{});
		Finalize();
	}

public:
	size_t GetVertexCount()
	{
		return  buffer.size() / TotalStride() * glm::uvec3::length();
	}
};

// : VertexAttributes{ std::move(attr) } {}
