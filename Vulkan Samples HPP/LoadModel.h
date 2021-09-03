#pragma once
#include <string>
#include "tiny_gltf.h"
#include "fmt/format.h"


class LoadModel
{
	static tinygltf::Model Load(const std::string& filename)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
		//bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)

		if (!warn.empty()) fmt::print("Warn: {}", warn.c_str());
		if (!err.empty()) 	printf("Err: %s\n", err.c_str());
		if (!ret) printf("Failed to parse glTF\n");



	}
};

