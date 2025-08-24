/*

		Copyright 2024 Etay Meiri

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#ifdef _WIN64
#include <direct.h>
#endif
#include <vector>

#include <vulkan/vulkan.h>

#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "SPIRV-Reflect/spirv_reflect.h"

#include "ogldev_types.h"
#include "ogldev_util.h"
#include "ogldev_vulkan_util.h"
#include "ogldev_vulkan_shader.h"

namespace OgldevVK {

struct Shader
{
	std::vector<u32> SPIRV;
	VkShaderModule ShaderModule = NULL;

	void Initialize(glslang_program_t* program)
	{
		size_t program_size = glslang_program_SPIRV_get_size(program);
		SPIRV.resize(program_size);
		glslang_program_SPIRV_get(program, SPIRV.data());
	}

	u32 GetPushConstantSize()
	{
		SpvReflectShaderModule SpvShaderModule;
		size_t Size = ARRAY_SIZE_IN_BYTES(SPIRV);

		SpvReflectResult result = spvReflectCreateShaderModule(Size, SPIRV.data(), &SpvShaderModule);
		if (result != SPV_REFLECT_RESULT_SUCCESS) {
			OGLDEV_ERROR("spvReflectCreateShaderModule error %d\n", result);
			exit(1);
		}

		u32 PushConstantSize = 0;

		for (u32 i = 0; i < SpvShaderModule.push_constant_block_count; ++i) {
			const SpvReflectBlockVariable& block = SpvShaderModule.push_constant_blocks[i];
			PushConstantSize = std::max(PushConstantSize, block.offset + block.size);
		}

		spvReflectDestroyShaderModule(&SpvShaderModule);

		return PushConstantSize;
	}
};


static void PrintShaderSource(const char* text)
{
	int line = 1;

	printf("\n(%3i) ", line);

	while (text && *text++)
	{
		if (*text == '\n') {
			printf("\n(%3i) ", ++line);
		}
		else if (*text == '\r') {
			// nothing to do
		}
		else {
			printf("%c", *text);
		}
	}

	printf("\n");
}


static bool CompileShader(VkDevice Device, glslang_stage_t Stage, const char* pShaderCode, Shader& ShaderModule)
{
	glslang_input_t input = {
		.language = GLSLANG_SOURCE_GLSL,
		.stage = Stage,
		.client = GLSLANG_CLIENT_VULKAN,
		.client_version = GLSLANG_TARGET_VULKAN_1_3,
		.target_language = GLSLANG_TARGET_SPV,
		.target_language_version = GLSLANG_TARGET_SPV_1_6,
		.code = pShaderCode,
		.default_version = 100,
		.default_profile = GLSLANG_NO_PROFILE,
		.force_default_version_and_profile = false,
		.forward_compatible = false,
		.messages = GLSLANG_MSG_DEFAULT_BIT,
		.resource = glslang_default_resource()
	};

	glslang_shader_t* shader = glslang_shader_create(&input);

	if (!glslang_shader_preprocess(shader, &input))	{
		fprintf(stderr, "GLSL preprocessing failed\n");
		fprintf(stderr, "\n%s", glslang_shader_get_info_log(shader));
		fprintf(stderr, "\n%s", glslang_shader_get_info_debug_log(shader));
		PrintShaderSource(input.code);
		return 0;
	}

	if (!glslang_shader_parse(shader, &input)) {
		fprintf(stderr, "GLSL parsing failed\n");
		fprintf(stderr, "\n%s", glslang_shader_get_info_log(shader));
		fprintf(stderr, "\n%s", glslang_shader_get_info_debug_log(shader));
		PrintShaderSource(glslang_shader_get_preprocessed_code(shader));
		return 0;
	}

	glslang_program_t* program = glslang_program_create();
	glslang_program_add_shader(program, shader);

	if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
		fprintf(stderr, "GLSL linking failed\n");
		fprintf(stderr, "\n%s", glslang_program_get_info_log(program));
		fprintf(stderr, "\n%s", glslang_program_get_info_debug_log(program));
		return 0;
	}

	glslang_program_SPIRV_generate(program, Stage);

	ShaderModule.Initialize(program);

	const char* spirv_messages = glslang_program_SPIRV_get_messages(program);

	if (spirv_messages) {
		fprintf(stderr, "SPIR-V message: '%s'", spirv_messages);
	}

	VkShaderModuleCreateInfo shaderCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = ARRAY_SIZE_IN_BYTES(ShaderModule.SPIRV),
		.pCode = (const u32*)ShaderModule.SPIRV.data()
	};

	VkResult res = vkCreateShaderModule(Device, &shaderCreateInfo, NULL, &ShaderModule.ShaderModule);
	CHECK_VK_RESULT(res, "vkCreateShaderModule\n");

	printf("Push constant size %d\n", ShaderModule.GetPushConstantSize());

	glslang_program_delete(program);
	glslang_shader_delete(shader);

	bool ret = ShaderModule.SPIRV.size() > 0;

	return ret;
}


static glslang_stage_t ShaderStageFromFilename(const char* pFilename)
{
	std::string s(pFilename);

	if (s.ends_with(".vert")) {
		return GLSLANG_STAGE_VERTEX;
	}

	if (s.ends_with(".frag")) {
		return GLSLANG_STAGE_FRAGMENT;
	}

	if (s.ends_with(".geom")) {
		return GLSLANG_STAGE_GEOMETRY;
	}

	if (s.ends_with(".comp")) {
		return GLSLANG_STAGE_COMPUTE;
	}

	if (s.ends_with(".tesc")) {
		return GLSLANG_STAGE_TESSCONTROL;
	}

	if (s.ends_with(".tese")) {
		return GLSLANG_STAGE_TESSEVALUATION;
	}

	printf("Unknown shader stage in '%s'\n", pFilename);
	exit(1);

	return GLSLANG_STAGE_VERTEX;
}


VkShaderModule CreateShaderModuleFromText(VkDevice Device, const char* pFilename)
{
	std::string Source;

	char CurWorkDir[256];
#ifdef _WIN64
	_getcwd(&CurWorkDir[0], ARRAY_SIZE_IN_ELEMENTS(CurWorkDir));
#else
	getcwd(&CurWorkDir[0], ARRAY_SIZE_IN_ELEMENTS(CurWorkDir));
#endif

	if (!ReadFile(pFilename, Source)) {
		printf("Current work dir: %s\n", CurWorkDir);

		assert(0);
	}

	Shader ShaderModule;

	glslang_stage_t ShaderStage = ShaderStageFromFilename(pFilename);

	VkShaderModule ret = NULL;

	glslang_initialize_process();

	bool Success = CompileShader(Device, ShaderStage, Source.c_str(), ShaderModule);	

	if (Success) {
		printf("Created shader from text file '%s\\%s'\n", CurWorkDir, pFilename);
		ret = ShaderModule.ShaderModule;
		std::string BinaryFilename = string(pFilename) + ".spv";
		WriteBinaryFile(BinaryFilename.c_str(), ShaderModule.SPIRV.data(), 
						(int)ShaderModule.SPIRV.size() * sizeof(u32));
	}

	glslang_finalize_process();

	return ret;
}


VkShaderModule CreateShaderModuleFromBinary(VkDevice Device, const char* pFilename)
{
	int codeSize = 0;
	char* pShaderCode = ReadBinaryFile(pFilename, codeSize);
	assert(pShaderCode);

	VkShaderModuleCreateInfo shaderCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = (size_t)codeSize,
		.pCode = (const u32*)pShaderCode
	};

	VkShaderModule shaderModule;
	VkResult res = vkCreateShaderModule(Device, &shaderCreateInfo, NULL, &shaderModule);
	CHECK_VK_RESULT(res, "vkCreateShaderModule\n");
	printf("Created shader from binary %s\n", pFilename);
	
	free(pShaderCode);

	return shaderModule;
}


}
