#pragma once
#include "Mesh.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct MeshManager
{
	std::vector<Mesh> m_Meshes;
	uint32_t m_LineVAO{0};
	uint32_t m_LineVBO{0};
	
	MeshManager();
	void CreateSkyBoxCube();

	void ProcessNode(const aiNode* node, const aiScene* scene, const std::string& directory);

	static MeshManager& GetInstance();
	uint32_t GetLineVAO();
	void CreatePlanePrimitive();
	void CreateCubePrimitive();
	void CreateCubeWireframePrimitive();

	static std::vector<Mesh>& GetMeshes();
	static Mesh& GetMesh(int i);
	static std::optional<uint32_t> LoadFromFile(const std::string& path, aiPostProcessSteps loadFlags);
	static std::tuple<uint32_t, uint32_t> GetLineBuffer();;
	static void Init();
	//static Mesh* GetMesh(const Meshtype t);;
	
};

