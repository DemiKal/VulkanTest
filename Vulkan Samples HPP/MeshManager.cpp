#include "MeshManager.h"
#include <optional>
#include <assimp/version.h>

void MeshManager::ProcessNode(
	const aiNode* node,
	const aiScene* scene,
	const std::string& directory)
{

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		
		m_Meshes.emplace_back(mesh, scene, directory);
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, directory);
	}
}

std::optional<uint32_t> MeshManager::LoadFromFile(const std::string& path, const aiPostProcessSteps loadFlags)
{
	Assimp::Importer importer;
	
	const auto standardFlags = aiProcess_GenBoundingBoxes;
	const auto flagsComposed =  loadFlags;
	
	const aiScene* scene = importer.ReadFile(path, flagsComposed);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		fmt::print("ERROR::ASSIMP\n", importer.GetErrorString());
		return std::nullopt;
	}

	const std::string directory = path.substr(0, path.find_last_of('/'));

 
	const uint32_t idx =  m_Meshes.size();	//TODO: make indexing work with multiple meshes loaded
	 ProcessNode(scene->mRootNode, scene, directory);

	//TODO: delete scene?;
	return idx; //TODO: use std optional?
}

 

MeshManager::MeshManager()
{
	CreateCubePrimitive();
	CreateCubeWireframePrimitive();
	CreateSkyBoxCube();
}

void MeshManager::CreateSkyBoxCube()
{
	const std::vector<float> skyboxVertices = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	}; 
}

  
void MeshManager::CreateCubeWireframePrimitive()
{
	std::vector<float> cubeVertices = {
	 -0.5f, -0.5f, 0.5f,
	 0.5f, -0.5f, 0.5f,
	 0.5f, 0.5f, 0.5f,
	 -0.5f,0.5f, 0.5f,
	 -0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, 0.5f, -0.5f,
	 -0.5f,0.5f, -0.5f,
	};

	std::vector<unsigned int> cubeIndices = {
	   0,1, 1,2, 2,3, 3,0,
	   1,5, 5,6, 6,2,
	   5,4, 6,7,
	   4,7, 0,4, 3,7
	};

	//..VertexBufferLayout vbl;
	//..vbl.Push<float>(3, VertexType::POSITION);
	//
	//m_Meshes.emplace_back(cubeVertices, cubeIndices, vbl);
	//m_Meshes.back().SetElemDrawType(GL_LINES);
}

void MeshManager::CreateCubePrimitive()
{
	const std::vector<float> cubeVertices = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	//VertexBufferLayout vbl;
	//vbl.Push<float>(3, VertexType::POSITION);
	//vbl.Push<float>(2, VertexType::TEXCOORD);

	//m_Meshes.emplace_back(cubeVertices, vbl);
}

Mesh& MeshManager::GetMesh(const int i)
{
	return m_Meshes[i];
}

 

std::vector<Mesh>& MeshManager::GetMeshes()
{
	return  m_Meshes;
}
