#pragma once
#include "VertexBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>

struct Mesh
{
	IndexBuffer indexBuffer;
	Buffer vertexBuffer;

	Mesh(const aiMesh* mesh, const aiScene* scene, const std::string& directory);
	Mesh() = default;
};

