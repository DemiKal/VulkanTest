#pragma once
#include "VertexBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>

struct Mesh
{
	IndexBuffer m_IndexBuffer;
	Buffer m_VertexBuffer;

	Mesh(const aiMesh* mesh, const aiScene* scene, const std::string& directory);
	Mesh() = default;
	Mesh(IndexBuffer& indexBuffer, Buffer vertexBuffer)
	{
		m_IndexBuffer = indexBuffer;
		m_VertexBuffer = vertexBuffer;
	}
};



