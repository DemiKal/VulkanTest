#pragma once
//
//#include <glslang/Public/ShaderLang.h>
////#include <glslang/Include/ResourceLimits.h>
//////#include <glslang/OSDependent/osinclude.h>
////#include <glslang/Include/ShHandle.h>
//
////#include <SPIRV/GLSL.std.450.h>
////#include <SPIRV/GlslangToSpv.h>
//#include <glslang/Public/ShaderLang.h>
////#include <glslang/Include/ResourceLimits.h>
//
//#include <glslang/SPIRV/GlslangToSpv.h>
// 
//#include <glslang/Include/ResourceLimits.h>
//#include <glslang/Include/Common.h>
//#include <vulkan/vulkan_core.h>
//
//const TBuiltInResource DefaultTBuiltInResource = {
//    /* .MaxLights = */ 32,
//    /* .MaxClipPlanes = */ 6,
//    /* .MaxTextureUnits = */ 32,
//    /* .MaxTextureCoords = */ 32,
//    /* .MaxVertexAttribs = */ 64,
//    /* .MaxVertexUniformComponents = */ 4096,
//    /* .MaxVaryingFloats = */ 64,
//    /* .MaxVertexTextureImageUnits = */ 32,
//    /* .MaxCombinedTextureImageUnits = */ 80,
//    /* .MaxTextureImageUnits = */ 32,
//    /* .MaxFragmentUniformComponents = */ 4096,
//    /* .MaxDrawBuffers = */ 32,
//    /* .MaxVertexUniformVectors = */ 128,
//    /* .MaxVaryingVectors = */ 8,
//    /* .MaxFragmentUniformVectors = */ 16,
//    /* .MaxVertexOutputVectors = */ 16,
//    /* .MaxFragmentInputVectors = */ 15,
//    /* .MinProgramTexelOffset = */ -8,
//    /* .MaxProgramTexelOffset = */ 7,
//    /* .MaxClipDistances = */ 8,
//    /* .MaxComputeWorkGroupCountX = */ 65535,
//    /* .MaxComputeWorkGroupCountY = */ 65535,
//    /* .MaxComputeWorkGroupCountZ = */ 65535,
//    /* .MaxComputeWorkGroupSizeX = */ 1024,
//    /* .MaxComputeWorkGroupSizeY = */ 1024,
//    /* .MaxComputeWorkGroupSizeZ = */ 64,
//    /* .MaxComputeUniformComponents = */ 1024,
//    /* .MaxComputeTextureImageUnits = */ 16,
//    /* .MaxComputeImageUniforms = */ 8,
//    /* .MaxComputeAtomicCounters = */ 8,
//    /* .MaxComputeAtomicCounterBuffers = */ 1,
//    /* .MaxVaryingComponents = */ 60,
//    /* .MaxVertexOutputComponents = */ 64,
//    /* .MaxGeometryInputComponents = */ 64,
//    /* .MaxGeometryOutputComponents = */ 128,
//    /* .MaxFragmentInputComponents = */ 128,
//    /* .MaxImageUnits = */ 8,
//    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
//    /* .MaxCombinedShaderOutputResources = */ 8,
//    /* .MaxImageSamples = */ 0,
//    /* .MaxVertexImageUniforms = */ 0,
//    /* .MaxTessControlImageUniforms = */ 0,
//    /* .MaxTessEvaluationImageUniforms = */ 0,
//    /* .MaxGeometryImageUniforms = */ 0,
//    /* .MaxFragmentImageUniforms = */ 8,
//    /* .MaxCombinedImageUniforms = */ 8,
//    /* .MaxGeometryTextureImageUnits = */ 16,
//    /* .MaxGeometryOutputVertices = */ 256,
//    /* .MaxGeometryTotalOutputComponents = */ 1024,
//    /* .MaxGeometryUniformComponents = */ 1024,
//    /* .MaxGeometryVaryingComponents = */ 64,
//    /* .MaxTessControlInputComponents = */ 128,
//    /* .MaxTessControlOutputComponents = */ 128,
//    /* .MaxTessControlTextureImageUnits = */ 16,
//    /* .MaxTessControlUniformComponents = */ 1024,
//    /* .MaxTessControlTotalOutputComponents = */ 4096,
//    /* .MaxTessEvaluationInputComponents = */ 128,
//    /* .MaxTessEvaluationOutputComponents = */ 128,
//    /* .MaxTessEvaluationTextureImageUnits = */ 16,
//    /* .MaxTessEvaluationUniformComponents = */ 1024,
//    /* .MaxTessPatchComponents = */ 120,
//    /* .MaxPatchVertices = */ 32,
//    /* .MaxTessGenLevel = */ 64,
//    /* .MaxViewports = */ 16,
//    /* .MaxVertexAtomicCounters = */ 0,
//    /* .MaxTessControlAtomicCounters = */ 0,
//    /* .MaxTessEvaluationAtomicCounters = */ 0,
//    /* .MaxGeometryAtomicCounters = */ 0,
//    /* .MaxFragmentAtomicCounters = */ 8,
//    /* .MaxCombinedAtomicCounters = */ 8,
//    /* .MaxAtomicCounterBindings = */ 1,
//    /* .MaxVertexAtomicCounterBuffers = */ 0,
//    /* .MaxTessControlAtomicCounterBuffers = */ 0,
//    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
//    /* .MaxGeometryAtomicCounterBuffers = */ 0,
//    /* .MaxFragmentAtomicCounterBuffers = */ 1,
//    /* .MaxCombinedAtomicCounterBuffers = */ 1,
//    /* .MaxAtomicCounterBufferSize = */ 16384,
//    /* .MaxTransformFeedbackBuffers = */ 4,
//    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
//    /* .MaxCullDistances = */ 8,
//    /* .MaxCombinedClipAndCullDistances = */ 8,
//    /* .MaxSamples = */ 4,
//    /* .maxMeshOutputVerticesNV = */ 256,
//    /* .maxMeshOutputPrimitivesNV = */ 512,
//    /* .maxMeshWorkGroupSizeX_NV = */ 32,
//    /* .maxMeshWorkGroupSizeY_NV = */ 1,
//    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
//    /* .maxTaskWorkGroupSizeX_NV = */ 32,
//    /* .maxTaskWorkGroupSizeY_NV = */ 1,
//    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
//    /* .maxMeshViewCountNV = */ 4,
//    /* .maxDualSourceDrawBuffersEXT = */ 1,
//
//    /* .limits = */ {
//        /* .nonInductiveForLoops = */ 1,
//        /* .whileLoops = */ 1,
//        /* .doWhileLoops = */ 1,
//        /* .generalUniformIndexing = */ 1,
//        /* .generalAttributeMatrixVectorIndexing = */ 1,
//        /* .generalVaryingIndexing = */ 1,
//        /* .generalSamplerIndexing = */ 1,
//        /* .generalVariableIndexing = */ 1,
//        /* .generalConstantMatrixVectorIndexing = */ 1,
//    } };
//
//class GLSLCompiler
//{
//
//    glslang::EShTargetLanguage        env_target_language = glslang::EShTargetLanguage::EShTargetNone;
//    glslang::EShTargetLanguageVersion  env_target_language_version = (glslang::EShTargetLanguageVersion)0;
//public:
//    EShLanguage FindShaderLanguage(VkShaderStageFlagBits stage)
//    {
//        switch (stage)
//        {
//        case VK_SHADER_STAGE_VERTEX_BIT:
//            return EShLangVertex;
//
//        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
//            return EShLangTessControl;
//
//        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
//            return EShLangTessEvaluation;
//
//        case VK_SHADER_STAGE_GEOMETRY_BIT:
//            return EShLangGeometry;
//
//        case VK_SHADER_STAGE_FRAGMENT_BIT:
//            return EShLangFragment;
//
//        case VK_SHADER_STAGE_COMPUTE_BIT:
//            return EShLangCompute;
//
//        case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
//            return EShLangRayGen;
//
//        case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
//            return EShLangAnyHit;
//
//        case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
//            return EShLangClosestHit;
//
//        case VK_SHADER_STAGE_MISS_BIT_KHR:
//            return EShLangMiss;
//
//        case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
//            return EShLangIntersect;
//
//        case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
//            return EShLangCallable;
//
//        default:
//            return EShLangVertex;
//        }
//    }
//    struct ShaderVariant
//    {
//
//    };
//	bool CompileToSPIRV(VkShaderStageFlagBits stage,
//		const std::vector<uint8_t>& glsl_source,
//		const std::string& entry_point,
//		//const ShaderVariant& shader_variant,
//		std::vector<std::uint32_t>& spirv,
//		std::string& info_log)
//	{
//		glslang::InitializeProcess();
//
//		EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);
//
//		EShLanguage language = FindShaderLanguage(stage);
//		std::string source = std::string(glsl_source.begin(), glsl_source.end());
//
//		const char* file_name_list[1] = { "" };
//		const char* shader_source = reinterpret_cast<const char*>(source.data());
//
//		glslang::TShader shader(language);
//		shader.setStringsWithLengthsAndNames(&shader_source, nullptr, file_name_list, 1);
//		shader.setEntryPoint(entry_point.c_str());
//		shader.setSourceEntryPoint(entry_point.c_str());
//		shader.setPreamble("");
//		shader.addProcesses({});
//		if (GLSLCompiler::env_target_language != glslang::EShTargetLanguage::EShTargetNone)
//		{
//			shader.setEnvTarget(GLSLCompiler::env_target_language, GLSLCompiler::env_target_language_version);
//		}
//
//		if (!shader.parse(&DefaultTBuiltInResource, 100, false, messages))
//		{
//			info_log = std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog());
//			return false;
//		}
//
//		// Add shader to new program object.
//		glslang::TProgram program;
//		program.addShader(&shader);
//
//		// Link program.
//		if (!program.link(messages))
//		{
//			info_log = std::string(program.getInfoLog()) + "\n" + std::string(program.getInfoDebugLog());
//			return false;
//		}
//
//		// Save any info log that was generated.
//		if (shader.getInfoLog())
//		{
//			info_log += std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog()) + "\n";
//		}
//
//		if (program.getInfoLog())
//		{
//			info_log += std::string(program.getInfoLog()) + "\n" + std::string(program.getInfoDebugLog());
//		}
//
//		glslang::TIntermediate* intermediate = program.getIntermediate(language);
//
//		// Translate to SPIRV.
//		if (!intermediate)
//		{
//			info_log += "Failed to get shared intermediate code.\n";
//			return false;
//		}
//
//		//spv::SpvBuildLogger logger;
//
//		glslang::GlslangToSpv(*intermediate, spirv );
//
//		//info_log += logger.getAllMessages() + "\n";
//
//		// Shutdown glslang library.
//		glslang::FinalizeProcess();
//
//		return true;
//
//	}
//};
