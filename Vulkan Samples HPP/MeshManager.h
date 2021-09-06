#pragma once
#include "Mesh.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct MeshManager
{
	std::vector<Mesh> m_Meshes;

	MeshManager();
	void CreateSkyBoxCube();
	void ProcessNode(const aiNode* node, const aiScene* scene, const std::string& directory);
	void CreateCubePrimitive();
	void CreateCubeWireframePrimitive();

	std::vector<Mesh>& GetMeshes();
	Mesh& GetMesh(int i);
	std::optional<uint32_t> LoadFromFile(const std::string& path, aiPostProcessSteps loadFlags);
	void AddMesh(IndexBuffer&  indexBuffer, Buffer&  vertexBuffer)
	{
		m_Meshes.emplace_back (indexBuffer,  vertexBuffer);
	}
};

