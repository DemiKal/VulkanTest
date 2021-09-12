#include "Texture2D.h"
#include <stb_image.h>
#include "Logger.hpp"
#include "VulkanHPP.h"

void Texture2D::LoadFromFile(VmaAllocator alloc, const std::string& path)
{
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels) {
		LOGE("Failed to load texture file {}", path);
		return  ;
	}

	void* pixel_ptr = pixels;
	VkDeviceSize imageSize = texWidth * texHeight * 4ll;

	VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

	//AllocatedBuffer stagingBuffer = engine.create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	VmaAllocation stagingBufferAllocation{};
	void* data;
	vmaMapMemory(alloc, stagingBufferAllocation, &data);

	memcpy(data, pixel_ptr, static_cast<size_t>(imageSize));

	vmaUnmapMemory(alloc, stagingBufferAllocation);

	stbi_image_free(pixels);

	VkExtent3D imageExtent;
	imageExtent.width = static_cast<uint32_t>(texWidth);
	imageExtent.height = static_cast<uint32_t>(texHeight);
	imageExtent.depth = 1;

	//VkImageCreateInfo dimg_info = vkinit::image_create_info(image_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

	//AllocatedImage newImage;

	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	//allocate and create the image
	//vmaCreateImage(alloc, &dimg_info, &dimg_allocinfo, &newImage._image, &newImage._allocation, nullptr);

}
