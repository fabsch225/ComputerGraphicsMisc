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

#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>

#include "ogldev_types.h"
#include "ogldev_vulkan_core.h"

namespace OgldevVK {

struct SimpleMesh {
	BufferAndMemory m_vb;
	size_t m_vertexBufferSize = 0;
	VulkanTexture* m_pTex = NULL;

	void Destroy(VkDevice Device)
	{
		m_vb.Destroy(Device);

		if (m_pTex) {
			m_pTex->Destroy(Device);
			delete m_pTex;
		}
	}
};

}
