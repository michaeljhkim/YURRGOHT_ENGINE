#pragma once

#include "engine/function/render/render_data.h"

#define MAX_VIEWPORTS 1

namespace Yurrgoht {
	class RenderPass {
	public:
		virtual void init();
		virtual void render() = 0;
		virtual void destroy();

		virtual void createRenderPass() = 0;
		virtual void createDescriptorSetLayouts() = 0;
		virtual void createPipelineLayouts() = 0;
		virtual void createPipelineCache();
		virtual void createPipelines() = 0;
		virtual void createFramebuffer() = 0;
		virtual void createResizableObjects(uint32_t width, uint32_t height);
		virtual void destroyResizableObjects();

		void setRenderDatas(const std::vector<std::shared_ptr<RenderData>>& render_datas) { m_render_datas = render_datas; }
		void onResize(uint32_t width, uint32_t height);
		virtual bool isEnabled();

	protected:
		void updatePushConstants(VkCommandBuffer command_buffer, VkPipelineLayout pipeline_layout, 
			const std::vector<const void*>& pcos, std::vector<VkPushConstantRange> push_constant_ranges = {});
		void addBufferDescriptorSet(std::vector<VkWriteDescriptorSet>& desc_writes, 
			VkDescriptorBufferInfo& desc_buffer_info, VmaBuffer buffer, uint32_t binding);
		void addImageDescriptorSet(std::vector<VkWriteDescriptorSet>& desc_writes, 
			VkDescriptorImageInfo& desc_image_info, VmaImageViewSampler texture, uint32_t binding);
		void addImagesDescriptorSet(std::vector<VkWriteDescriptorSet>& desc_writes,
			VkDescriptorImageInfo* p_desc_image_info, const std::vector<VmaImageViewSampler>& textures, uint32_t binding);

		// vulkan objects
		VkRenderPass m_render_pass = VK_NULL_HANDLE;
		VkDescriptorPool m_descriptor_pool = VK_NULL_HANDLE;
		std::vector<VkDescriptorSetLayout> m_desc_set_layouts;
		std::vector<VkPushConstantRange> m_push_constant_ranges;
		std::vector<VkPipelineLayout> m_pipeline_layouts;
		VkPipelineCache m_pipeline_cache;

		// pipeline create info structures
		VkGraphicsPipelineCreateInfo m_pipeline_ci{};
		VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state_ci{};
		VkPipelineRasterizationStateCreateInfo m_rasterize_state_ci{};
		VkPipelineMultisampleStateCreateInfo m_multisampling_ci{};
		VkPipelineDepthStencilStateCreateInfo m_depth_stencil_ci{};
		std::vector<VkPipelineColorBlendAttachmentState> m_color_blend_attachments;
		VkPipelineColorBlendStateCreateInfo m_color_blend_ci{};
		VkPipelineViewportStateCreateInfo m_viewport_ci{};
		std::vector<VkDynamicState> m_dynamic_states;
		VkPipelineDynamicStateCreateInfo m_dynamic_state_ci{};

		std::vector<VkPipeline> m_pipelines;
		VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

		// render dependent data
		std::vector<std::shared_ptr<RenderData>> m_render_datas;

		// render target size
		uint32_t m_width = 0, m_height = 0;
	};
}