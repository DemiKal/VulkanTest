#pragma once
#include <variant>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <vector>

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
struct BoneWeight : VertexAttributeNew<glm::vec<BoneIndexCount, float>> { INHERIT_CONSTRUCTOR };
struct BoneIndex : VertexAttributeNew<glm::vec<BoneIndexCount, int>> { INHERIT_CONSTRUCTOR };
struct Indices : VertexAttributeNew<glm::uvec3> { INHERIT_CONSTRUCTOR };

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


#define ATTRIBUTE_TYPES  PositionAttribute, ColorAttribute, TexCoordAttribute, BitangentAttribute, TangentAttribute, NormalAttribute, BoneWeight, BoneIndex
using AttributeVariant = std::variant<ATTRIBUTE_TYPES>;

template<typename T>
constexpr auto GetIndexType(const AttributeVariant& v)
{
	//constexpr size_t i = static_cast<size_t>(nullptr) + static_cast<size_t>(nullptr);
	return std::get_if<T>(&v);
}

struct VertexBuffer //: private std::vector<std::byte>
{
	std::vector<AttributeVariant> VertexAttributes;
	std::vector<std::byte> data;
	vk::Buffer vkBuffer;

	void AddAttribute(const AttributeVariant& a)
	{
		VertexAttributes.push_back(a);
	}

	template<typename T>
	void AddElement(const T&& elem)
	{
		auto byteSize = sizeof(T);
		std::array < std::byte, T::Stride()> arr;
		T* ptr = reinterpret_cast<T*>(&arr);
		*ptr = std::move(elem);

		data.insert(data.end(), std::begin(arr), std::end(arr));

	}

	//always check for nullptr!
	template<typename T>
	T* GetAttribute(size_t idx)
	{
		auto vertexStride = TotalStride();
		auto capacity = data.size() / vertexStride;
		if (idx > capacity) return nullptr;
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

				return reinterpret_cast<T*>(data.data() + totalStride + offset);
			}

			offset += std::visit(lambda, v);
		}

		return nullptr;
	}

	size_t TotalStride()
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
private:
	//std::byte& operator[](int index)
	//{
		//return this[index];
		//
		//auto o =std::byte{8};
		//return o;
	//}
};

