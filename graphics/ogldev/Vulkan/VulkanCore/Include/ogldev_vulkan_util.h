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
#include <vector>

#include <vulkan/vulkan.h>

#include "ogldev_types.h"

#define CHECK_VK_RESULT(res, msg) \
	if (res != VK_SUCCESS) {      \
		fprintf(stderr, "Error in %s:%d - %s, code %x\n", __FILE__, __LINE__, msg, res);  \
		exit(1);	\
	}

namespace OgldevVK {

const char* GetDebugSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity);

const char* GetDebugType(VkDebugUtilsMessageTypeFlagsEXT Type);

int GetBytesPerTexFormat(VkFormat Format);

bool HasStencilComponent(VkFormat Format);

VkFormat FindSupportedFormat(VkPhysicalDevice Device, const std::vector<VkFormat>& Candidates,
							 VkImageTiling Tiling, VkFormatFeatureFlags Features);
}
