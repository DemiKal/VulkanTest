#pragma once
//#include <string>
#include "tiny_gltf.h"
//#include "fmt/format.h"
//#include "VertexBuffer.h"
#include <glm/gtc/type_ptr.hpp>
#include "mesh.h"

struct Node
{
	glm::mat4 matrix;
};

//template<typename T, typename U>
//T Convert(U val, int type, bool normalize)
//{
//	if (type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
//	{
//		unsigned short val_intr = reinterpret_cast<unsigned short>(val);
//		float f = static_cast<float>(val_intr);
//		if (normalize)
//			f / std::numeric_limits<unsigned short>::max();
//		return f;
//	}
//}

#define CONVERT_COMPONENT 


struct LoadModel
{
	void loadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, Node* parent, IndexBuffer& indexBuffer, Buffer& vertexBuffer)
	{
		Node node;
		node.matrix = glm::mat4(1.0f);
		if (inputNode.translation.size() == 3) {
			node.matrix = glm::translate(node.matrix, glm::vec3(glm::make_vec3(inputNode.translation.data())));
		}
		if (inputNode.rotation.size() == 4) {
			glm::quat q = glm::make_quat(inputNode.rotation.data());
			node.matrix *= glm::mat4(q);
		}
		if (inputNode.scale.size() == 3) {
			node.matrix = glm::scale(node.matrix, glm::vec3(glm::make_vec3(inputNode.scale.data())));
		}
		if (inputNode.matrix.size() == 16) {
			node.matrix = glm::make_mat4x4(inputNode.matrix.data());
		};
		if (inputNode.children.size() > 0) {
			for (size_t i = 0; i < inputNode.children.size(); i++) {
				loadNode(input.nodes[inputNode.children[i]], input, &node, indexBuffer, vertexBuffer);
			}
		}
		if (inputNode.mesh > -1)
		{
			const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
			// Iterate through all primitives of this node's mesh
			//for (size_t i = 0; i < mesh.primitives.size(); i++) 
			for (const tinygltf::Primitive& glTFPrimitive : mesh.primitives)
			{
				//const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
				uint32_t firstIndex = static_cast<uint32_t>(indexBuffer.buffer.size());
				uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.buffer.size());
				uint32_t indexCount = 0;

				bool hasPos = glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end();
				bool hasNormal = glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end();
				bool hasTexCoord0 = glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end();
				bool hasColor = glTFPrimitive.attributes.find("COLOR_0") != glTFPrimitive.attributes.end();

				//if (hasNormal) vertexBuffer.AddAttribute(PositionAttribute{});
				//if (hasTexCoord0) vertexBuffer.AddAttribute(PositionAttribute{});



				// Vertices
				const std::byte* positionBuffer = nullptr;
				const std::byte* normalsBuffer = nullptr;
				const std::byte* texCoordsBuffer = nullptr;
				const std::byte* colorBuffer = nullptr;
				size_t vertexCount = 0;

				// Get buffer data for vertex normals
				if (hasPos)
				{
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					positionBuffer = reinterpret_cast<const std::byte*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					vertexCount = accessor.count;
					vertexBuffer.AddAttribute(PositionAttribute{});

				}
				// Get buffer data for vertex normals
				if (hasNormal)
				{
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					normalsBuffer = reinterpret_cast<const std::byte*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					//vertexBuffer.AddAttribute(NormalAttribute{});
				}
				// Get buffer data for vertex texture coordinates
				// glTF supports multiple sets, we only load the first one
				if (hasTexCoord0)
				{
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					texCoordsBuffer = reinterpret_cast<const std::byte*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					//vertexBuffer.AddAttribute(TexCoordAttribute{});

				}
				if (hasColor)
				{
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("COLOR_0")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					colorBuffer = reinterpret_cast<const std::byte*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));

					vertexBuffer.AddAttribute(VertexColor_u16vec4{});
				}

				vertexBuffer.Finalize();

				// Append data to model's vertex buffer
				for (size_t v = 0; v < vertexCount; v++)
				{
					if (hasPos)
					{
						const float* f_ptr = reinterpret_cast<const float*>(positionBuffer);
						const glm::vec3* posConverted = reinterpret_cast<const glm::vec3*>(f_ptr) + v;
						glm::vec3 pos = glm::make_vec3(f_ptr + v * 3); //* sizeof(float)
						vertexBuffer.AddElement<PositionAttribute >(pos.x, pos.y, pos.z);


					}
					if (hasColor)
					{
						//vertexBuffer.AddElement<ColorAttribute>(ColorBuffer, v);
						//const uint16_t* r_ptr = reinterpret_cast<const uint16_t*>(colorBuffer) +  4 * v;
						//uint16_t r  = *r_ptr;
						//uint16_t g  = *(r_ptr + 1);
						//uint16_t b  = *(r_ptr + 2);
						//uint16_t a  = *(r_ptr + 3);
						const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("COLOR_0")->second];

						AddColors(vertexBuffer, colorBuffer, accessor.componentType, accessor.type, v);
						//const float r = static_cast<float>(ru16) / std::numeric_limits<uint16_t>::max();
						//const float g = static_cast<float>(gu16) / std::numeric_limits<uint16_t>::max();
						//const float b = static_cast<float>(bu16) / std::numeric_limits<uint16_t>::max();
						//const float a = static_cast<float>(au16) / std::numeric_limits<uint16_t>::max();
						//const float cr = convert
						//vertexBuffer.AddElement<ColorAttribute>(r, g, b, a);



					}

					//Vertex vert{};
					//auto pos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
					//auto normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
					//auto uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
					//auto color = glm::vec3(1.0f);
					//vertexBuffer.push_back(vert);
				}

				// Indices
				{
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
					const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

					indexCount += static_cast<uint32_t>(accessor.count);
					//indexBuffer.AddElement<IndexAttribute>(0, 1, 2);

					// glTF supports different component types of indices
					switch (accessor.componentType) {
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
						uint32_t* buf = new uint32_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
						for (size_t index = 0; index < accessor.count; index++) {
							auto ind = (buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
						uint16_t* buf = new uint16_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
						for (size_t index = 0; index < accessor.count; index++) {
							auto ind = (buf[index] + vertexStart);
						}
						//std::byte* ptr = reinterpret_cast<std::byte*>(buf);
						//size_t buffAllocBytes = accessor.count * sizeof(uint16_t);
						//for (size_t i = 0; i < buffAllocBytes; i+= 3 )
						indexBuffer.AddAttribute(IndexAttribute_u16vec3{});
						indexBuffer.Finalize();
						for (size_t i = 0; i < accessor.count; i += 3)
						{
							uint32_t i0 = static_cast<uint32_t>(buf[i]);
							uint32_t i1 = static_cast<uint32_t>(buf[i + 1]);
							uint32_t i2 = static_cast<uint32_t>(buf[i + 2]);
							indexBuffer.AddElement<IndexAttribute_u16vec3>(i0, i1, i2);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
						uint8_t* buf = new uint8_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
						for (size_t index = 0; index < accessor.count; index++) {
							auto ind = (buf[index] + vertexStart);
						}
						break;
					}
					default:
						fmt::print("Index component type {} not supported!", accessor.componentType);
						return;
					}
				}
				//Primitive primitive{};
				//primitive.firstIndex = firstIndex;
				//primitive.indexCount = indexCount;
				//primitive.materialIndex = glTFPrimitive.material;
				//node.mesh.primitives.push_back(primitive);
			}
		}

	}

	void AddColors(Buffer& buffer, const std::byte* srcBuffer, int componentType, int vecType, size_t index)
	{
		if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT && vecType == TINYGLTF_TYPE_VEC4)
			AddColors<VertexColor_u16vec4>(buffer, srcBuffer, index);
		if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT && vecType == TINYGLTF_TYPE_VEC3)
			AddColors<VertexColor_u16vec3>(buffer, srcBuffer, index);
	}

	template<typename T>
	void AddColors(Buffer& buffer, const std::byte* srcBuffer, size_t index)
	{
		const T* ptr = reinterpret_cast<const T*>(srcBuffer);
		//vertexBuffer.AddElement<ColorAttribute>(ColorBuffer, v);
		//const uint16_t* r_ptr = reinterpret_cast<const uint16_t*>(colorBuffer) +  4 * v;
		//uint16_t r  = *r_ptr;
		//uint16_t g  = *(r_ptr + 1);
		//uint16_t b  = *(r_ptr + 2);
		//uint16_t a  = *(r_ptr + 3);
		buffer.AddElement<T>((const void*)srcBuffer, index);

	}

	//tinygltf::Model Load(const std::string& filename)
	Mesh Load(const std::string& filename)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
		//bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)

		if (!warn.empty()) fmt::print("Warn: {}", warn.c_str());
		if (!err.empty()) printf("Err: %s\n", err.c_str());
		if (!ret) printf("Failed to parse glTF\n");

		for (auto& mesh : model.meshes)
		{
			fmt::print(mesh.name);
			for (auto& primitive : mesh.primitives)
			{
				for (auto [name, idx] : primitive.attributes)
				{
					fmt::print("name:{}, idx:{}", name, idx);
				}

			}
		}
		//for (size_t i = 0; i < model.accessors.size(); i++) {
		//	const tinygltf::Accessor& accessor = model.accessors[i];
		//std::vector<uint32_t> indexBuffer;
		//std::vector<VulkanglTFModel::Vertex> vertexBuffer;
		IndexBuffer indexBuffer;
		Buffer vertexBuffer;
		for (auto& accessor : model.accessors)
		{

		}
		const tinygltf::Scene& scene = model.scenes[0]; //TODO: loop?

		for (size_t i = 0; i < scene.nodes.size(); i++) {
			const tinygltf::Node node = model.nodes[scene.nodes[i]];
			loadNode(node, model, nullptr, indexBuffer, vertexBuffer);
		}

		//for (auto& accessor : model.accessors)
		//{
		//	accessor.
		//	auto stride = accessor.ByteStride();
		//}
		// Buffer
		{
			//std::map<std::string, tinygltf::BufferView>::const_iterator it(
			//	scene.bufferViews.begin());
			//std::map<std::string, tinygltf::BufferView>::const_iterator itEnd(
			//	scene.bufferViews.end());
			//
			//for (; it != itEnd; it++) {
			//	const tinygltf::BufferView& bufferView = it->second;
			//	if (bufferView.target == 0) {
			//		std::cout << "WARN: bufferView.target is zero" << std::endl;
			//		continue;  // Unsupported bufferView.
			//	}
			//
			//	const tinygltf::Buffer& buffer = scene.buffers[bufferView.buffer];
			//	GLBufferState state;
			//	glGenBuffers(1, &state.vb);
			//	glBindBuffer(bufferView.target, state.vb);
			//	std::cout << "buffer.size= " << buffer.data.size()
			//		<< ", byteOffset = " << bufferView.byteOffset << std::endl;
			//	glBufferData(bufferView.target, bufferView.byteLength,
			//		&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
			//	glBindBuffer(bufferView.target, 0);
			//
			//	gBufferState[it->first] = state;
			//}
		}
		//Mesh mesh;
		//mesh.m_VertexBuffer = vertex
		return Mesh(indexBuffer, vertexBuffer);
	}
};

