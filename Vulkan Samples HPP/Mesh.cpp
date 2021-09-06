#pragma once
#include "Mesh.h"
#include "Logger.hpp"

glm::mat4 AI2GLMMAT(aiMatrix4x4& ai_mat)
{
	return glm::transpose(glm::make_mat4(ai_mat[0]));
}
//class glm::vec3;



struct  Armature2
{
	std::string name;
	int id{};
	std::vector< Armature2*> children;
	Armature2* parent{};
	glm::mat4 mat{};
	std::vector<uint32_t> indices;
	~Armature2() noexcept
	{
		//LOGI("lmao cyaa\n");
		for (auto* child : children)
			delete child;
	}
};

Armature2* FindBone(Armature2* curr, const std::string& name)
{
	for (auto* child : curr->children)
		FindBone(child, name);

	if (std::strcmp(curr->name.c_str(), name.c_str()) == 0)
	{
		return curr;
	}
}

void CreateArmature(Armature2* parentArmature, aiNode* node, uint32_t  iter)
{
	Armature2* me = new Armature2;
	me->name = node->mName.C_Str();
	me->mat = glm::make_mat4(node->mTransformation[0]);

	if (iter > 0) me->parent = parentArmature;

	parentArmature->children.push_back(me);

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		CreateArmature(me, node->mChildren[i], ++iter);
	}
}


void CreateHierarchy(Armature2* parent, uint32_t& idx)
{
	parent->id = idx;

	for (Armature2* child : parent->children)
		CreateHierarchy(child, ++idx);

	for (Armature2* child : parent->children)
		parent->indices.emplace_back(child->id);

}

void CreateList(Armature2* parent, std::unordered_map< std::string, std::vector  < std::tuple < std::string, uint32_t>>>& dict)
{
	for (Armature2* child : parent->children)
		CreateList(child, dict);
	for (Armature2* child : parent->children)
		dict[parent->name].push_back({ child->name, child->id });
}
//void GetChildIndices(Armature2* parent, const std::unordered_map<std::string)

//TODO: ADD  PARENT POINTER AND FETCH LIST OF TRANSFORMS THAT WAY

//not needed now
/*
	auto stride1 = m_VertexBufferLayout.GetStride() / 4;
	auto boneIndexElem = m_VertexBufferLayout.GetElement(VertexType::BONE_INDEX);
	auto subStride = boneIndexElem.vertexIndex / 4;

	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		auto& as = bonemapping[i];
		for (uint32_t j = 0; j < as.size(); j++)
		{
			auto& [boneIdx, weight] = as[j];
			uint32_t idxB = stride1 * i + subStride + j; //m_VertexBufferLayout.GetElementIndex(i, j, VertexType::BONE_INDEX);
			uint32_t idxW = idxB + boneIndexElem.count;

			m_Vertices[idxB] = boneIdx;
			m_Vertices[idxW] = weight;

		}
	}

}
void Mesh::LoadMaterialTextures(
	const aiMaterial* material,
	const aiTextureType type,
	const std::string& typeName,
	const aiScene* scene)
{
	//aiMaterial* material = scene->mMaterials[0];
	bool isEmbedded = false;
	const unsigned int texCount = material->GetTextureCount(type);

	for (unsigned int i = 0; i < texCount; i++)
	{
		aiString texture_file;
		aiReturn ret = material->Get(AI_MATKEY_TEXTURE(type, i), texture_file);
		if (ret != aiReturn_SUCCESS) continue;

		if (const aiTexture* texture = scene->GetEmbeddedTexture(texture_file.C_Str()))
		{
			//returned pointer is not null, read texture from memory
			isEmbedded = true;
			if (texture->mHeight == 0)
			{
				m_Textures.emplace_back(texture, typeName);
			}
			else
			{
				fmt::print("Texture is embedded but height is not 0!");
				//image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &components_per_pixel, 0);
				//m_Textures.emplace_back(fullpath, typeName);
			}
		}
	}


	//regular file, check if it exists and read it

	if (!isEmbedded)
	{

		for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
		{
			aiString fileName;
			auto r = material->GetTexture(type, i, &fileName);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (auto& j : m_Textures)
			{
				if (std::strcmp(j.GetPath().data(), fileName.C_Str()) == 0)
				{
					//textures.push_back(j);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				const std::string fullpath = m_Directory + '/' + fileName.C_Str();
				m_Textures.emplace_back(fullpath, typeName);
			}

		}
	}
}

void Mesh::LoadAnimations(const aiScene* scene, const aiMesh* mesh, const std::unordered_map<std::string, unsigned>& pairs, const std::vector<Joint>& bones)
{
	if (scene->HasAnimations())
	{
		Animator animator;
		animator.m_Bones.resize(bones.size());
		std::copy(bones.begin(), bones.end(), animator.m_Bones.begin());
		std::unordered_map<std::string, uint32_t> boneNames;	//todo: this has already been done b4 but whatever
		for (Joint& j : animator.m_Bones)
		{
			j.m_PoseTransform = glm::mat4(1.0f);
			boneNames[j.m_Name] = j.m_Index;
		}
		for (unsigned int i = 0; i < scene->mNumAnimations; i++)
		{
			aiAnimation* anim = scene->mAnimations[i];
			const std::string anim_name = anim->mName.C_Str();
			float ticks = 1;	//;
			float duration = static_cast<float>(anim->mDuration);
			animator.m_Duration = duration;
			animator.m_Ticks = ticks;
			double test = static_cast<double>(animator.m_Ticks) / 1000.0;
			double test2 = anim->mTicksPerSecond;


			std::vector<AnimationChannel> AnimationChannels;
			AnimationChannels.resize(anim->mNumChannels); //resize to sync with bones

			for (uint32_t j = 0; j < anim->mNumChannels; j++)
			{
				const aiNodeAnim* ai_channel = anim->mChannels[j];
				const std::string channelName = ai_channel->mNodeName.C_Str();

				std::vector<PositionKey> positionkeys;
				std::vector<RotationKey> rotationKeys;
				std::vector<ScaleKey> scalingKeys;

				for (uint32_t k = 0; k < ai_channel->mNumPositionKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mPositionKeys[k];
					positionkeys.emplace_back(ai_key.mTime, ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
				}

				for (uint32_t k = 0; k < ai_channel->mNumRotationKeys; k++)
				{
					aiQuatKey ai_key = ai_channel->mRotationKeys[k];
					rotationKeys.emplace_back(ai_key.mTime, ai_key.mValue.w, ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
				}

				for (uint32_t k = 0; k < ai_channel->mNumScalingKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mScalingKeys[k];
					scalingKeys.emplace_back(ai_key.mTime, ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
				}

				//AnimationChannels.emplace_back(AnimationChannel(channelName, positionkeys, rotationKeys, scalingKeys));
				unsigned int channel_idx = boneNames[channelName];
				AnimationChannels[channel_idx] = AnimationChannel(channelName, positionkeys, rotationKeys, scalingKeys);

			}

			Animation animation(duration, AnimationChannels);
			animator.current = animation; //TODO add multiple animations!
		}

		m_animator = animator;
		m_animator.m_inverse_root = glm::mat4(1.0f);
		m_AnimationLoaded = true;
	}
}

*/


Mesh::Mesh(
	const aiMesh* mesh,
	const aiScene* scene,
	const std::string& directory)
{
	//m_Directory = directory;
	constexpr unsigned int BONESPERVERTEX = 3;

	//if (scene->HasTextures())
	//{
	//	
	//	for (int i = 0; i < scene->mNumTextures; i++)
	//	{
	//		scene->HasMaterials()
	//		//scene->mMaterials[0]->GetTexture()
	//		const aiTexture* texture = scene->GetEmbeddedTexture("*" + i);
	//		texture->pcData;
	//		
	//		if (texture->mHeight == 0)
	//		{
	//			//auto image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &components_per_pixel, 0);
	//		}
	//	}
	//}

	//VertexBufferLayout vbLayout;
	unsigned int stride = 0;
	//this->m_VertexBufferLayout
	bool pos, norm, tex, tangents, colors, bones = false;
	if (pos = mesh->HasPositions())
		vertexBuffer.AddAttribute(PositionAttribute{});
	if (norm = mesh->HasNormals())
		vertexBuffer.AddAttribute(NormalAttribute{});
	if (tex = mesh->HasTextureCoords(0))
		vertexBuffer.AddAttribute(TexCoordAttribute{});
	if (tangents = mesh->HasTangentsAndBitangents())
	{
		vertexBuffer.AddAttribute(TangentAttribute{});
		vertexBuffer.AddAttribute(BitangentAttribute{});
	}
	if (colors = mesh->HasVertexColors(0))
		vertexBuffer.AddAttribute(ColorAttribute{});

	if (bones = mesh->HasBones())
	{
		vertexBuffer.AddAttribute(BoneIndexAttribute{});
		vertexBuffer.AddAttribute(BoneWeightAttribute{});
	}

	const unsigned int morphTargetCount = mesh->mNumAnimMeshes;

	//if (morphTargetCount)
	//{
	//	for (unsigned int i = 0; i < morphTargetCount; i++)
	//	{
	//		aiAnimMesh* morph = mesh->mAnimMeshes[i];
	//		if (morph->HasPositions())
	//			m_VertexBufferLayout.Push<float>(3, VertexType::POSITION);
	//
	//		if (morph->mNumVertices != mesh->mNumVertices)
	//			std::cout << " morph target vertex m_Count != original vertex m_Count!";
	//	}
	//}

	vertexBuffer.Finalize();

	//finally create vertex buffer
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		if (pos)
		{
			auto position = PositionAttribute(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertexBuffer.AddElement(position);
		}

		if (colors)
		{
			float r = mesh->mColors[i]->r;
			float g = mesh->mColors[i]->g;
			float b = mesh->mColors[i]->b;
			vertexBuffer.AddElement(ColorAttribute(r,g,b));
		}

		//if (norm)
		//{
		//	m_Vertices.push_back(mesh->mNormals[i].x);
		//	m_Vertices.push_back(mesh->mNormals[i].y);
		//	m_Vertices.push_back(mesh->mNormals[i].z);
		//}
		//if (hasTexCoords)
		//{
		//	m_Vertices.push_back(mesh->mTextureCoords[0][i].x);
		//	m_Vertices.push_back(mesh->mTextureCoords[0][i].y);
		//}
		//manage morph animation by interleaving
		//for (unsigned int j = 0; j < morphTargetCount; j++)
		//{
		//	aiAnimMesh* morph = mesh->mAnimMeshes[j];
		//	//TODO: check for each vertex element to see if they are in animation
		//	if (morph->HasPositions())
		//		m_Vertices.push_back(morph->mVertices[i].x);
		//	m_Vertices.push_back(morph->mVertices[i].y);
		//	m_Vertices.push_back(morph->mVertices[i].z);
		//}
		//if (hasTangents)
		//{
		//	m_Vertices.push_back(mesh->mTangents[i].x);
		//	m_Vertices.push_back(mesh->mTangents[i].y);
		//	m_Vertices.push_back(mesh->mTangents[i].z);
		//	// bitangemplace_back( 
		//	m_Vertices.push_back(mesh->mBitangents[i].x);
		//	m_Vertices.push_back(mesh->mBitangents[i].y);
		//	m_Vertices.push_back(mesh->mBitangents[i].z);
		//}
		//if (hasBones)
		//{
		//	for (uint32_t b = 0; b < BONESPERVERTEX; b++)
		//	{
		//		m_Vertices.push_back(0);
		//		m_Vertices.push_back(0);
		//	}
		//}

	}
	
	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex m_Indices.
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all m_Indices of the face and store them in the m_Indices vector
		std::array<uint32_t, 3> faceIndices;
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			faceIndices[j] = face.mIndices[j];
		}

		indexBuffer.AddAttribute(IndexAttribute(faceIndices[0], faceIndices[1], faceIndices[2]));

	}



	//if (tex)
	//{
	//	const uint32_t indxSize = m_Indices.size() / 3;
	//	//for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	//	for (uint32_t i = 0; i < indxSize; i++)
	//	{
	//		auto a = i * 3;
	//		auto b = i * 3 + 1u;
	//		auto c = i * 3 + 2u;
	//
	//		uint32_t idx1 = m_Indices[a];
	//		uint32_t idx2 = m_Indices[b];
	//		uint32_t idx3 = m_Indices[c];
	//
	//		float u1 = mesh->mTextureCoords[0][idx1].x;
	//		float v1 = mesh->mTextureCoords[0][idx1].y;
	//		glm::vec2 uv1(u1, v1);
	//
	//		float u2 = mesh->mTextureCoords[0][idx2].x;
	//		float v2 = mesh->mTextureCoords[0][idx2].y;
	//		glm::vec2 uv2(u2, v2);
	//
	//		float u3 = mesh->mTextureCoords[0][idx3].x;
	//		float v3 = mesh->mTextureCoords[0][idx3].y;
	//		glm::vec2 uv3(u3, v3);
	//		//std::array< glm::vec2, 3>> face = {uv1, uv2, uv3};
	//		std::array< glm::vec2, 3> face = { uv1, uv2, uv3 };
	//		m_UVs.emplace_back(face);
	//
	//	}
	 
 	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	auto a = material->GetTextureCount(aiTextureType_DIFFUSE);
	auto b = material->GetTextureCount(aiTextureType_SPECULAR);
	auto e = material->GetTextureCount(aiTextureType_AMBIENT);
	auto k = material->GetTextureCount(aiTextureType_EMISSIVE);
	auto c = material->GetTextureCount(aiTextureType_HEIGHT);
	auto i = material->GetTextureCount(aiTextureType_NORMALS);
	auto o = material->GetTextureCount(aiTextureType_SHININESS);
	auto p = material->GetTextureCount(aiTextureType_OPACITY);
	auto paa = material->GetTextureCount(aiTextureType_DISPLACEMENT);
	auto zzp = material->GetTextureCount(aiTextureType_LIGHTMAP);
	auto f = material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
	auto m = material->GetTextureCount(aiTextureType_REFLECTION);

	auto rg = material->GetTextureCount(aiTextureType_BASE_COLOR);
	auto s = material->GetTextureCount(aiTextureType_NORMAL_CAMERA);
	auto t = material->GetTextureCount(aiTextureType_EMISSION_COLOR);
	auto u = material->GetTextureCount(aiTextureType_METALNESS);
	auto v = material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
	//auto g = material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
	//auto h = material->GetTextureCount(aiTextureType_METALNESS);
	//auto j = material->GetTextureCount(aiTextureType_EMISSION_COLOR);

	auto aal = material->GetTextureCount(aiTextureType_UNKNOWN);
	auto pss = material->GetTextureCount(aiTextureType_NONE);

	//LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
	//LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
	//LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height", scene);
	//LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient", scene);
	//LoadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive", scene);
	//LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normals", scene);
	//LoadMaterialTextures(material, aiTextureType_LIGHTMAP, "texture_lightmap", scene);
	//LoadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, "texture_ambient_occlusion", scene);
	//LoadMaterialTextures(material, aiTextureType_REFLECTION, "texture_reflection", scene);
	//LoadMaterialTextures(material, aiTextureType_BASE_COLOR, "texture_base_color", scene);
	//LoadMaterialTextures(material, aiTextureType_NORMAL_CAMERA, "texture_normal_camera", scene);
	//LoadMaterialTextures(material, aiTextureType_EMISSION_COLOR, "texture_emission_color", scene);
	//LoadMaterialTextures(material, aiTextureType_METALNESS, "texture_metalness", scene);
	//LoadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_diffuse_roughness", scene);
	//LoadMaterialTextures(material, aiTextureType_UNKNOWN, "texture_unknown", scene);
	//LoadMaterialTextures(material, aiTextureType_NONE, "texture_none", scene);


	//aiAABB  ai_aabb = mesh->mAABB;
	//
	//m_aabb = AABB({ ai_aabb.mMin.x, ai_aabb.mMin.y, ai_aabb.mMin.z },
	//	{ ai_aabb.mMax.x, ai_aabb.mMax.y, ai_aabb.mMax.z });
	//m_aabb_OG = m_aabb;

	//for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	//	meshnew.m_PositionVertices.emplace_back(
	//		glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
	//for (const int v_idx : m_Indices)
	//{
	//	auto& ai_v = mesh->mVertices[v_idx];
	//	glm::vec4 v{ ai_v.x, ai_v.y, ai_v.z, 0 }; //added 0 for padding with gpu
	//	m_PositionVertices.emplace_back(v);
	//
	//}

	//CreateBuffers();
}