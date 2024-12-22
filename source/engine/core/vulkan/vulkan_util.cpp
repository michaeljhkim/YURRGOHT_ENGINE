#define VMA_IMPLEMENTATION
#include "vulkan_util.h"
#include "engine/core/vulkan/vulkan_rhi.h"

#include <tinygltf/stb_image.h>
#include <fstream>

namespace Yurrgoht
{
	const char* vkErrorString(VkResult result)
	{
		switch (result)
		{
#define STR(r) case VK_##r: return #r
			STR(NOT_READY);
			STR(TIMEOUT);
			STR(EVENT_SET);
			STR(EVENT_RESET);
			STR(INCOMPLETE);
			STR(ERROR_OUT_OF_HOST_MEMORY);
			STR(ERROR_OUT_OF_DEVICE_MEMORY);
			STR(ERROR_OUT_OF_POOL_MEMORY);
			STR(ERROR_INITIALIZATION_FAILED);
			STR(ERROR_DEVICE_LOST);
			STR(ERROR_MEMORY_MAP_FAILED);
			STR(ERROR_LAYER_NOT_PRESENT);
			STR(ERROR_EXTENSION_NOT_PRESENT);
			STR(ERROR_FEATURE_NOT_PRESENT);
			STR(ERROR_INCOMPATIBLE_DRIVER);
			STR(ERROR_TOO_MANY_OBJECTS);
			STR(ERROR_FORMAT_NOT_SUPPORTED);
			STR(ERROR_SURFACE_LOST_KHR);
			STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
			STR(SUBOPTIMAL_KHR);
			STR(ERROR_OUT_OF_DATE_KHR);
			STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
			STR(ERROR_VALIDATION_FAILED_EXT);
			STR(ERROR_INVALID_SHADER_NV);
#undef STR
		default:
			return "UNKNOWN_ERROR";
		}
	}

	std::string vkPhysicalDeviceTypeString(VkPhysicalDeviceType type)
	{
		switch (type)
		{
#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_##r: return #r
			STR(OTHER);
			STR(INTEGRATED_GPU);
			STR(DISCRETE_GPU);
			STR(VIRTUAL_GPU);
			STR(CPU);
#undef STR
		default: return "UNKNOWN_DEVICE_TYPE";
		}
	}

	void VmaBuffer::destroy()
	{
		if (buffer != VK_NULL_HANDLE)
		{
			vmaDestroyBuffer(VulkanRHI::get().getAllocator(), buffer, allocation);
		}
	}

	void VmaImage::destroy()
	{
		if (image != VK_NULL_HANDLE)
		{
			vmaDestroyImage(VulkanRHI::get().getAllocator(), image, allocation);
		}
	}

	void VmaImageView::destroy()
	{
		if (view != VK_NULL_HANDLE)
		{
			vkDestroyImageView(VulkanRHI::get().getDevice(), view, nullptr);
		}

		vma_image.destroy();
	}

	void VmaImageViewSampler::destroy()
	{
		if (sampler != VK_NULL_HANDLE)
		{
			vkDestroySampler(VulkanRHI::get().getDevice(), sampler, nullptr);
		}
		if (view != VK_NULL_HANDLE)
		{
			vkDestroyImageView(VulkanRHI::get().getDevice(), view, nullptr);
		}
		vma_image.destroy();
	}

	VkCommandBuffer VulkanUtil::beginInstantCommands()
	{
		VkCommandBufferAllocateInfo command_buffer_ai{};
		command_buffer_ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_ai.commandPool = VulkanRHI::get().getInstantCommandPool();
		command_buffer_ai.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(VulkanRHI::get().getDevice(), &command_buffer_ai, &command_buffer);

		VkCommandBufferBeginInfo command_buffer_bi{};
		command_buffer_bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(command_buffer, &command_buffer_bi);

		return command_buffer;
	}

	void VulkanUtil::endInstantCommands(VkCommandBuffer command_buffer)
	{
		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		VkFenceCreateInfo fence_ci{};
		fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_ci.flags = 0;

		VkFence fence;
		vkCreateFence(VulkanRHI::get().getDevice(), &fence_ci, nullptr, &fence);

		VkQueue queue = VulkanRHI::get().getGraphicsQueue();
		vkQueueSubmit(queue, 1, &submit_info, fence);

		vkWaitForFences(VulkanRHI::get().getDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
		vkDestroyFence(VulkanRHI::get().getDevice(), fence, nullptr);

		vkFreeCommandBuffers(VulkanRHI::get().getDevice(), VulkanRHI::get().getInstantCommandPool(), 1, &command_buffer);
	}

	void VulkanUtil::createBuffer(VkDeviceSize size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, VmaBuffer& buffer)
	{
		VkBufferCreateInfo buffer_ci{};
		buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_ci.size = size;
		buffer_ci.usage = buffer_usage;
		buffer_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		buffer_ci.flags = 0;

		VmaAllocationCreateInfo vma_alloc_ci{};
		vma_alloc_ci.usage = memory_usage;
		if (memory_usage == VMA_MEMORY_USAGE_AUTO_PREFER_HOST)
		{
			vma_alloc_ci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}
		
		buffer.size = size;
		vmaCreateBuffer(VulkanRHI::get().getAllocator(), &buffer_ci, &vma_alloc_ci, &buffer.buffer, &buffer.allocation, nullptr);
	}

	void VulkanUtil::copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
	{
		VkCommandBuffer command_buffer = beginInstantCommands();

		VkBufferCopy copy_region{};
		copy_region.size = size;
		vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

		endInstantCommands(command_buffer);
	}

	void VulkanUtil::updateBuffer(VmaBuffer& buffer, void* data, size_t size)
	{
		void* mapped_data;
		vmaMapMemory(VulkanRHI::get().getAllocator(), buffer.allocation, &mapped_data);
		memcpy(mapped_data, data, size);
		vmaUnmapMemory(VulkanRHI::get().getAllocator(), buffer.allocation);
	}

	VmaImageViewSampler VulkanUtil::loadImageViewSampler(const std::string& filename,
		uint32_t mip_levels, uint32_t layers, VkFormat format, VkFilter min_filter, VkFilter mag_filter, 
			VkSamplerAddressMode address_mode, VkImageUsageFlags ext_use_flags)
	{
		int width, height;
		const int k_channels = 4;
		const auto& fs = g_engine.fileSystem();
		uint8_t* image_data = stbi_load(fs->absolute(filename).c_str(), &width, &height, 0, k_channels);
		ASSERT(image_data != nullptr, "failed to load image {}", filename);

		VmaImageViewSampler image_view_sampler;
		VulkanUtil::createImageViewSampler(width, height, image_data, mip_levels, layers, format,
			min_filter, mag_filter, address_mode, image_view_sampler, ext_use_flags);
		stbi_image_free(image_data);

		return image_view_sampler;
	}

	void VulkanUtil::createImageViewSampler(uint32_t width, uint32_t height, uint8_t* image_data,
		uint32_t mip_levels, uint32_t layers, VkFormat format, VkFilter min_filter, VkFilter mag_filter,
		VkSamplerAddressMode address_mode, VmaImageViewSampler& vma_image_view_sampler, VkImageUsageFlags ext_use_flags)
	{
		// create Image
		VkImageUsageFlags image_usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		if (image_data)
		{
			image_usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		if (ext_use_flags)
		{
			image_usage |= ext_use_flags;
		}

		createImage(width, height, mip_levels, layers, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_TILING_OPTIMAL,
			image_usage, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, vma_image_view_sampler.vma_image);

		VkImage image = vma_image_view_sampler.image();
		vma_image_view_sampler.view = createImageView(image, format, calcImageAspectFlags(format), mip_levels, layers);

		// create VkSampler
		vma_image_view_sampler.sampler = createSampler(min_filter, mag_filter, mip_levels, address_mode, address_mode, address_mode);

		// set image layout add descriptor type
		vma_image_view_sampler.image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		vma_image_view_sampler.descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		if (ext_use_flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			vma_image_view_sampler.image_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		}
		if (ext_use_flags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
		{
			vma_image_view_sampler.descriptor_type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		}

		if (image_data)
		{
			size_t image_size = width * height * calcFormatSize(format);
			VmaBuffer staging_buffer;
			createBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, staging_buffer);

			// copy image pixel data to staging buffer
			updateBuffer(staging_buffer, image_data, image_size);

			// transition image to DST_OPT state for copy into
			transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, format, mip_levels);

			// copy staging buffer to image
			copyBufferToImage(staging_buffer.buffer, image, width, height);

			// clear staging buffer
			vmaDestroyBuffer(VulkanRHI::get().getAllocator(), staging_buffer.buffer, staging_buffer.allocation);

			// generate image mipmaps, and transition image to READ_ONLY_OPT state for shader reading
			createImageMipmaps(image, width, height, mip_levels);
		}
	}

	void VulkanUtil::loadImageData(const std::string& filename, uint32_t& width, uint32_t& height, std::vector<uint8_t>& image_data)
	{
		const int k_channels = 4;
		const auto& fs = g_engine.fileSystem();
		uint8_t* p_image_data = stbi_load(fs->absolute(filename).c_str(), (int*)&width, (int*)&height, 0, k_channels);
		ASSERT(p_image_data != nullptr, "failed to load image {}", filename);
		image_data = std::vector<uint8_t>(p_image_data, p_image_data + width * height * k_channels);
		stbi_image_free(p_image_data);
	}

	void VulkanUtil::createImageAndView(uint32_t width, uint32_t height, uint32_t mip_levels, uint32_t layers, VkSampleCountFlagBits num_samples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags image_usage, VmaMemoryUsage memory_usage, VkImageAspectFlags aspect_flags, VmaImageView& vma_image_view)
	{
		createImage(width, height, mip_levels, layers, num_samples, format, tiling, image_usage, memory_usage, vma_image_view.vma_image);
		vma_image_view.view = createImageView(vma_image_view.vma_image.image, format, aspect_flags, mip_levels, layers);
	}

	void VulkanUtil::createImage(uint32_t width, uint32_t height, uint32_t mip_levels, uint32_t layers, VkSampleCountFlagBits num_samples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags image_usage, VmaMemoryUsage memory_usage, VmaImage& image)
	{
		VkImageCreateInfo image_ci{};
		image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_ci.imageType = VK_IMAGE_TYPE_2D;
		image_ci.extent.width = width;
		image_ci.extent.height = height;
		image_ci.extent.depth = 1;
		image_ci.mipLevels = mip_levels;
		image_ci.arrayLayers = layers;
		image_ci.format = format;
		image_ci.tiling = tiling;
		image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_ci.usage = image_usage;
		image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_ci.samples = num_samples;
		image_ci.flags = layers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

		VmaAllocationCreateInfo vma_alloc_ci{};
		vma_alloc_ci.usage = memory_usage;
		if (memory_usage == VMA_MEMORY_USAGE_AUTO_PREFER_HOST)
		{
			vma_alloc_ci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}

		VkResult result = vmaCreateImage(VulkanRHI::get().getAllocator(), &image_ci, &vma_alloc_ci, &image.image, &image.allocation, nullptr);
		CHECK_VULKAN_RESULT(result, "vma create image");
	}

	VkImageView VulkanUtil::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels, uint32_t layers)
	{
		VkImageViewCreateInfo image_view_ci{};
		image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_ci.image = image;
		image_view_ci.viewType = layers == 6 ? VK_IMAGE_VIEW_TYPE_CUBE : (layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D);
		image_view_ci.format = format;

		image_view_ci.subresourceRange.aspectMask = aspect_flags;
		image_view_ci.subresourceRange.baseMipLevel = 0;
		image_view_ci.subresourceRange.levelCount = mip_levels;
		image_view_ci.subresourceRange.baseArrayLayer = 0;
		image_view_ci.subresourceRange.layerCount = layers;

		VkImageView image_view;
		vkCreateImageView(VulkanRHI::get().getDevice(), &image_view_ci, nullptr, &image_view);

		return image_view;
	}

	VkSampler VulkanUtil::createSampler(VkFilter min_filter, VkFilter mag_filter, uint32_t mip_levels,
		VkSamplerAddressMode address_mode_u, VkSamplerAddressMode address_mode_v, VkSamplerAddressMode address_mode_w)
	{
		VkSamplerCreateInfo sampler_ci{};
		sampler_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_ci.minFilter = min_filter;
		sampler_ci.magFilter = mag_filter;
		sampler_ci.addressModeU = address_mode_u;
		sampler_ci.addressModeV = address_mode_v;
		sampler_ci.addressModeW = address_mode_w;
		sampler_ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler_ci.anisotropyEnable = true;
		sampler_ci.maxAnisotropy = VulkanRHI::get().getPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
		sampler_ci.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_ci.unnormalizedCoordinates = VK_FALSE;
		sampler_ci.compareEnable = VK_FALSE;
		sampler_ci.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler_ci.mipLodBias = 0.0f;
		sampler_ci.minLod = 0.0f;
		sampler_ci.maxLod = static_cast<float>(mip_levels);

		VkSampler sampler;
		vkCreateSampler(VulkanRHI::get().getDevice(), &sampler_ci, nullptr, &sampler);

		return sampler;
	}

	void VulkanUtil::createVertexBuffer(uint32_t buffer_size, void* vertex_data, VmaBuffer& vertex_buffer)
	{
		VmaBuffer staging_buffer;
		createBuffer(buffer_size, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VMA_MEMORY_USAGE_AUTO_PREFER_HOST, 
			staging_buffer);

		// copy vertex staging_buffer_data to staging buffer
		updateBuffer(staging_buffer, vertex_data, static_cast<size_t>(buffer_size));

		createBuffer(buffer_size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
			vertex_buffer);

		copyBuffer(staging_buffer.buffer, vertex_buffer.buffer, buffer_size);

		vmaDestroyBuffer(VulkanRHI::get().getAllocator(), staging_buffer.buffer, staging_buffer.allocation);
	}

	void VulkanUtil::createIndexBuffer(const std::vector<uint32_t>& indices, VmaBuffer& index_buffer)
	{
		VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();

		VmaBuffer staging_buffer;
		createBuffer(buffer_size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
			staging_buffer);

		// copy index data to staging buffer
		updateBuffer(staging_buffer, (void*)indices.data(), static_cast<size_t>(buffer_size));

		createBuffer(buffer_size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
			index_buffer);

		copyBuffer(staging_buffer.buffer, index_buffer.buffer, buffer_size);

		vmaDestroyBuffer(VulkanRHI::get().getAllocator(), staging_buffer.buffer, staging_buffer.allocation);
	}

	VkAccessFlags accessFlagsForImageLayout(VkImageLayout layout)
	{
		switch (layout)
		{
		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			return VK_ACCESS_HOST_WRITE_BIT;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			return VK_ACCESS_TRANSFER_WRITE_BIT;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			return VK_ACCESS_TRANSFER_READ_BIT;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			return VK_ACCESS_SHADER_READ_BIT;
		default:
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

	VkPipelineStageFlags pipelineStageForLayout(VkImageLayout layout)
	{
		switch (layout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			return VK_PIPELINE_STAGE_TRANSFER_BIT;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			return VK_PIPELINE_STAGE_HOST_BIT;
		case VK_IMAGE_LAYOUT_UNDEFINED:
			return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		default:
			return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
	}

	void VulkanUtil::transitionImageLayout(VkImage image, VkImageLayout old_layout, VkImageLayout new_layout, VkFormat format, uint32_t mip_levels, uint32_t layers)
	{
		if (old_layout == new_layout)
		{
			return;
		}

		VkCommandBuffer command_buffer = beginInstantCommands();

		// init image memory barrier
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;

		barrier.subresourceRange.aspectMask = calcImageAspectFlags(format);
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mip_levels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layers;

		VkPipelineStageFlags src_stage = pipelineStageForLayout(old_layout);
		VkPipelineStageFlags dst_stage = pipelineStageForLayout(new_layout);
		barrier.srcAccessMask = accessFlagsForImageLayout(old_layout);
		barrier.dstAccessMask = accessFlagsForImageLayout(new_layout);

		vkCmdPipelineBarrier(
			command_buffer,
			src_stage, dst_stage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endInstantCommands(command_buffer);
	}

	void VulkanUtil::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer command_buffer = beginInstantCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endInstantCommands(command_buffer);
	}

	void VulkanUtil::createImageMipmaps(VkImage image, uint32_t width, uint32_t height, uint32_t mip_levels)
	{
		VkCommandBuffer command_buffer = beginInstantCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mip_width = width;
		int32_t mip_height = height;

		for (uint32_t i = 1; i < mip_levels; ++i)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mip_width, mip_height, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mip_width > 1 ? mip_width >> 1 : 1, mip_height > 1 ? mip_height >> 1 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(command_buffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mip_width > 1) mip_width >>= 1;
			if (mip_height > 1) mip_height >>= 1;
		}

		barrier.subresourceRange.baseMipLevel = mip_levels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		endInstantCommands(command_buffer);
	}

	bool VulkanUtil::hasStencil(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	bool VulkanUtil::hasDepth(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	VkImageAspectFlags VulkanUtil::calcImageAspectFlags(VkFormat format)
	{
		VkImageAspectFlags flags = 0;
		if (hasDepth(format))
		{
			flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		if (hasStencil(format))
		{
			flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		return flags ? flags : VK_IMAGE_ASPECT_COLOR_BIT;
	}

	uint32_t VulkanUtil::calcFormatSize(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_R16_SFLOAT:
			return 2;
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_R16G16_SFLOAT:
			return 4;
		case VK_FORMAT_R16G16B16A16_SFLOAT:
		return 8;
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			return 16;
		default:
			LOG_FATAL("unsupported format: {}", format);
			return 0;
		}
	}

	uint32_t VulkanUtil::calcMipLevel(uint32_t width, uint32_t height /*= 0*/)
	{
		if (height == 0)
		{
			height = width;
		}
		return static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
	}

	void VulkanUtil::extractImage(VkImage image, uint32_t width, uint32_t height, VkFormat format, 
		std::vector<uint8_t>& image_data, VkImageLayout initial_layout, 
		uint32_t mip_levels, uint32_t layers, VkImageLayout final_layout)
	{
		// create staging buffer
		VmaBuffer staging_buffer;
		size_t image_size = width * height * calcFormatSize(format) * layers;
		createBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, staging_buffer);

		// copy to staging buffer
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferImageHeight = 0;
		region.bufferRowLength = 0;
		region.imageSubresource.aspectMask = calcImageAspectFlags(format);
		region.imageSubresource.mipLevel = mip_levels - 1;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layers;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		// transition image layouts
		VulkanUtil::transitionImageLayout(image, initial_layout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, format, mip_levels, layers);

		VkCommandBuffer command_buffer = VulkanUtil::beginInstantCommands();

		// copy image to buffer
		vkCmdCopyImageToBuffer(
			command_buffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			staging_buffer.buffer,
			1,
			&region
		);

		VulkanUtil::endInstantCommands(command_buffer);

		// reset image layouts
		if (final_layout != VK_IMAGE_LAYOUT_UNDEFINED)
		{
			VulkanUtil::transitionImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, final_layout, format, mip_levels, layers);
		}

		// mapping data
		void* mapped_data = nullptr;
		vmaMapMemory(VulkanRHI::get().getAllocator(), staging_buffer.allocation, &mapped_data);
		image_data.resize(image_size);
		memcpy(image_data.data(), mapped_data, image_size);
		vmaUnmapMemory(VulkanRHI::get().getAllocator(), staging_buffer.allocation);
		staging_buffer.destroy();
	}

	void VulkanUtil::saveImage(VkImage image, uint32_t width, uint32_t height, VkFormat format, 
		const std::string& filename, VkImageLayout initial_layout, 
		uint32_t mip_levels, uint32_t layers, VkImageLayout final_layout)
	{
		std::vector<uint8_t> image_data;
		extractImage(image, width, height, format, image_data, initial_layout, mip_levels, layers, final_layout);

		std::ofstream ofs(filename, std::ios::binary);
		ofs.write((const char*)image_data.data(), image_data.size());
		ofs.close();
	}

}