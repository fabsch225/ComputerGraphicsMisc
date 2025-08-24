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

#pragma once
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ogldev_vulkan_simple_mesh.h"


namespace OgldevVK {

class GraphicsPipeline {

public:

	GraphicsPipeline(VkDevice Device,
					 GLFWwindow* pWindow,
					 VkRenderPass RenderPass,
					 VkShaderModule vs,
					 VkShaderModule fs,
					 const SimpleMesh* pMesh,
					 int NumImages,
					 std::vector<BufferAndMemory>& UniformBuffers,
					 int UniformDataSize,
					 bool DepthEnabled);


	~GraphicsPipeline();

	void Bind(VkCommandBuffer CmdBuf, int ImageIndex);

private:

	void CreateDescriptorPool(int NumImages);
	void CreateDescriptorSets(const SimpleMesh* pMesh, int NumImages,
						  	  std::vector<BufferAndMemory>& UniformBuffers, int UniformDataSize);
	void CreateDescriptorSetLayout(std::vector<BufferAndMemory>& UniformBuffers, int UniformDataSize, VulkanTexture* pTex);
	void AllocateDescriptorSets(int NumImages);
	void UpdateDescriptorSets(const SimpleMesh* pMesh, int NumImages, std::vector<BufferAndMemory>& UniformBuffers, int UniformDataSize);

	VkDevice m_device = VK_NULL_HANDLE;
	VkPipeline m_pipeline = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> m_descriptorSets;
};
}
