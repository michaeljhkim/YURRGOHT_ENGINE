#include "ui_pass.h"
#include "engine/core/vulkan/vulkan_rhi.h"
#include "engine/core/base/macro.h"
#include "engine/core/event/event_system.h"
#include "engine/function/render/window_system.h"
#include "engine/platform/timer/timer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include "fonts/IconsFontAwesome5.h"

namespace Yurrgoht {

	void checkVkResult(VkResult result) {
		CHECK_VULKAN_RESULT(result, "handle imgui");
	}

	void UIPass::init() {
		StopWatch stop_watch;
		stop_watch.start();

		// setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGuiContext* current_context = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.FontGlobalScale = g_engine.windowSystem()->getResolutionScale();	// If 1080p, the font is just 1.0
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.IniFilename = nullptr;

		// setup Dear ImGui style
		//ImGui::StyleColorsDark();  // Apply the default dark style
		// Set colors to match Godot's dark theme
		/*
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);  // #2d2d2d
		ImGui::GetStyle().Colors[ImGuiCol_ChildBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);  // #2d2d2d
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);  // #3c3c3c
		ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(0.29f, 0.29f, 0.29f, 1.0f);  // #4a4a4a
		ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg] = ImVec4(0.29f, 0.29f, 0.29f, 1.0f); 
		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);  // #505050
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.44f, 0.44f, 0.44f, 1.0f);  // #707070
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.0f);  // #909090
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);  // #c8c8c8
		*/
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		// Colors
		colors[ImGuiCol_Text] =                    ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
		colors[ImGuiCol_TextDisabled] =            ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
		colors[ImGuiCol_WindowBg] =                ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_ChildBg] =                 ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
		colors[ImGuiCol_PopupBg] =                 ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] =                  ImVec4(0.43f, 0.50f, 0.56f, 0.50f);
		colors[ImGuiCol_BorderShadow] =            ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		//colors[ImGuiCol_FrameBg] =                 ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] =          ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
		colors[ImGuiCol_FrameBgActive] =           ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		colors[ImGuiCol_TitleBg] =                 ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		colors[ImGuiCol_TitleBgActive] =           ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] =        ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] =               ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] =             ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] =           ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] =    ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] =     ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] =               ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] =              ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] =        ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
		colors[ImGuiCol_Button] =                  ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonHovered] =           ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_ButtonActive] =            ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
		
		colors[ImGuiCol_Header] =                  ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
		colors[ImGuiCol_HeaderHovered] =           ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_HeaderActive] =            ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

		colors[ImGuiCol_Separator] =               ImVec4(0.43f, 0.50f, 0.56f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] =        ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
		colors[ImGuiCol_SeparatorActive] =         ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_ResizeGrip] =              ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] =       ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] =        ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] =                     ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_TabHovered] =              ImVec4(0.28f, 0.56f, 1.00f, 0.80f);
		colors[ImGuiCol_TabActive] =               ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_TabUnfocused] =            ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] =      ImVec4(0.14f, 0.22f, 0.36f, 1.00f);
		colors[ImGuiCol_DockingPreview] =          ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] =          ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] =               ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] =        ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] =           ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] =    ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] =          ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] =          ImVec4(1.00f, 0.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] =            ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] =   ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] =       ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] =        ImVec4(0.80f, 0.80f, 0.80f, 0.35f);


		ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;
		ImGui::GetStyle().ScaleAllSizes( g_engine.windowSystem()->getResolutionScale() );

		// create descriptor pool
		createDescriptorPool();

		// create render pass
		RenderPass::init();

		// setup platform/renderer backends
		ImGui_ImplSDL3_InitForVulkan(g_engine.windowSystem()->getWindow());
		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.Instance = VulkanRHI::get().getInstance();
		init_info.PhysicalDevice = VulkanRHI::get().getPhysicalDevice();
		init_info.Device = VulkanRHI::get().getDevice();
		init_info.QueueFamily = VulkanRHI::get().getGraphicsQueueFamily();
		init_info.Queue = VulkanRHI::get().getGraphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = m_descriptor_pool;
		init_info.Subpass = 0;
		init_info.MinImageCount = VulkanRHI::get().getSwapchainImageCount();
		init_info.ImageCount = VulkanRHI::get().getSwapchainImageCount();
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = nullptr;
		init_info.CheckVkResultFn = &checkVkResult;
		init_info.RenderPass = m_render_pass;
		bool is_success = ImGui_ImplVulkan_Init(&init_info);
		ASSERT(is_success, "failed to init imgui");


		// add consola font
		const float k_base_font_size = 14.0f;
		ImFontConfig config;
		config.RasterizerDensity = g_engine.windowSystem()->getResolutionScale();	// Ensures font resolution matches DPI
		const auto& fs = g_engine.fileSystem();
		io.Fonts->AddFontFromFileTTF(fs->absolute("asset/engine/font/imgui_fonts/ProggyClean.ttf").c_str(), k_base_font_size, &config);

		// add icon font
		const float k_icon_font_size = k_base_font_size * 0.8f;
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphMinAdvanceX = k_icon_font_size;
		icons_config.RasterizerDensity = g_engine.windowSystem()->getResolutionScale();
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		io.Fonts->AddFontFromFileTTF(fs->absolute("asset/engine/font/fa-solid-900.ttf").c_str(), k_icon_font_size, &icons_config, icons_ranges);

		// add small consola font
		const float k_small_font_size = 12.0f;
		io.Fonts->AddFontFromFileTTF(fs->absolute("asset/engine/font/imgui_fonts/ProggyClean.ttf").c_str(), k_small_font_size, &config);

		// add big icon font
		const float k_big_icon_font_size = 18.0f;
		icons_config.MergeMode = false;
		icons_config.GlyphMinAdvanceX = k_big_icon_font_size;
		io.Fonts->AddFontFromFileTTF(fs->absolute("asset/engine/font/fa-solid-900.ttf").c_str(), k_big_icon_font_size, &icons_config, icons_ranges);

		// create swapchain related objects
		const VkExtent2D& extent = VulkanRHI::get().getSwapchainImageSize();
		createResizableObjects(extent.width, extent.height);

		LOG_INFO("ui pass init time: {}ms", stop_watch.stopMs());
	}

	void UIPass::prepare() {
		// process imgui frame and get draw data
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		// set docking over viewport
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(0, NULL, dockspace_flags);
		//std::cout << dockspace_id << std::endl;
		//std::cout << ImGui::GetCurrentWindow()->ID << std::endl;

		// construct imgui widgets
		g_engine.eventSystem()->syncDispatch(std::make_shared<RenderConstructUIEvent>());

		// calculate imgui draw data
		ImGui::Render();
	}

	void UIPass::render() {
		VkCommandBuffer command_buffer = VulkanRHI::get().getCommandBuffer();
		uint32_t flight_index = VulkanRHI::get().getFlightIndex();

		// record render pass
		VkRenderPassBeginInfo renderpass_bi{};
		renderpass_bi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderpass_bi.renderPass = m_render_pass;
		renderpass_bi.framebuffer = m_framebuffers[VulkanRHI::get().getImageIndex()];
		renderpass_bi.renderArea.offset = { 0, 0 };
		renderpass_bi.renderArea.extent = { m_width, m_height };

		VkClearValue clear_value = {0.0f, 0.0f, 0.0f, 1.0f};
		renderpass_bi.clearValueCount = 1;
		renderpass_bi.pClearValues = &clear_value;

		vkCmdBeginRenderPass(command_buffer, &renderpass_bi, VK_SUBPASS_CONTENTS_INLINE);

		// record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);

		// for multiple viewports
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

		vkCmdEndRenderPass(command_buffer);
	}

	void UIPass::destroy() {
		// destroy imgui
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();

		RenderPass::destroy();
	}

	void UIPass::createRenderPass() {
		// create renderpass
		VkAttachmentDescription attachment{};
		attachment.format = VulkanRHI::get().getColorFormat();
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment{};
		color_attachment.attachment = 0;
		color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo render_pass_ci{};
		render_pass_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_ci.attachmentCount = 1;
		render_pass_ci.pAttachments = &attachment;
		render_pass_ci.subpassCount = 1;
		render_pass_ci.pSubpasses = &subpass;
		render_pass_ci.dependencyCount = 1;
		render_pass_ci.pDependencies = &dependency;
		VkResult result = vkCreateRenderPass(VulkanRHI::get().getDevice(), &render_pass_ci, nullptr, &m_render_pass);
		CHECK_VULKAN_RESULT(result, "create imgui render pass");
	}

	void UIPass::createDescriptorPool() {
		const uint32_t k_max_image_count = 128;
		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, k_max_image_count }
		};
		VkDescriptorPoolCreateInfo pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = k_max_image_count;
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		VkResult result = vkCreateDescriptorPool(VulkanRHI::get().getDevice(), &pool_info, nullptr, &m_descriptor_pool);
		CHECK_VULKAN_RESULT(result, "create imgui descriptor pool");
	}

	void UIPass::createFramebuffer() {
		// create framebuffers
		VkImageView image_view;
		VkFramebufferCreateInfo framebuffer_ci{};
		framebuffer_ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_ci.renderPass = m_render_pass;
		framebuffer_ci.attachmentCount = 1;
		framebuffer_ci.pAttachments = &image_view;
		framebuffer_ci.width = m_width;
		framebuffer_ci.height = m_height;
		framebuffer_ci.layers = 1;

		uint32_t image_count = VulkanRHI::get().getSwapchainImageCount();
		m_framebuffers.resize(image_count);
		for (uint32_t i = 0; i < image_count; ++i) {
			image_view = VulkanRHI::get().getSwapchainImageViews()[i];
			VkResult result = vkCreateFramebuffer(VulkanRHI::get().getDevice(), &framebuffer_ci, nullptr, &m_framebuffers[i]);
			CHECK_VULKAN_RESULT(result, "create imgui frame buffer");
		}
	}

	void UIPass::destroyResizableObjects() {
		for (VkFramebuffer framebuffer : m_framebuffers) {
			vkDestroyFramebuffer(VulkanRHI::get().getDevice(), framebuffer, nullptr);
		}
		m_framebuffers.clear();
	}

}