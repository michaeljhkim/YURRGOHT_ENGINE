#include "simulation_ui.h"
#include "editor/global/editor_context.h"

#include "engine/core/vulkan/vulkan_rhi.h"
#include "engine/core/event/event_system.h"
#include "engine/function/render/render_system.h"

#include "engine/platform/timer/timer.h"
#include "engine/resource/asset/asset_manager.h"
#include "engine/function/framework/scene/scene_manager.h"
#include "engine/function/framework/component/transform_component.h"
#include "engine/function/framework/component/camera_component.h"
#include "engine/function/framework/component/static_mesh_component.h"
#include "engine/function/framework/component/skeletal_mesh_component.h"
#include "engine/function/framework/component/animation_component.h"
#include "engine/function/framework/component/animator_component.h"
#include "engine/function/framework/component/directional_light_component.h"
#include "engine/function/framework/component/sky_light_component.h"
#include "engine/function/framework/component/spot_light_component.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <ImGuizmo/ImGuizmo.h>

namespace Yurrgoht {

	void SimulationUI::init() {
		m_title = "Simulation";
		m_color_texture_sampler = VulkanUtil::createSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT);

		m_coordinate_mode = ECoordinateMode::Local;
		m_operation_mode = EOperationMode::Translate;
		m_mouse_right_button_pressed = false;
		m_res_scale = g_engine.windowSystem()->getResolutionScale();

		g_engine.eventSystem()->addListener(EEventType::WindowKey, std::bind(&SimulationUI::onKey, this, std::placeholders::_1));
		g_engine.eventSystem()->addListener(EEventType::SelectEntity, std::bind(&SimulationUI::onSelectEntity, this, std::placeholders::_1));
		LOG_INFO("SUCCESS");
	}

	void SimulationUI::construct() {
		const std::string& scene_name = g_engine.sceneManager()->getCurrentSceneName();
		sprintf(m_title_buf, "%s %s###%s", ICON_FA_GAMEPAD, scene_name.c_str(), m_title.c_str());

		bool is_simulating_fullscreen = g_engine.isSimulating() && g_editor.isSimulationPanelFullscreen();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, is_simulating_fullscreen ? 0.0f : 1.0f);

		if (!ImGui::Begin(m_title_buf)) {
			ImGui::End();
			ImGui::PopStyleVar(2);
			return;
		}
		updateWindowRegion();

		// hide window tab bar
		if (ImGui::IsWindowDocked()) {
			ImGuiWindow* window = ImGui::FindWindowByName(m_title_buf);
			ImGuiDockNode* node = window->DockNode;
			bool show_tab = !is_simulating_fullscreen;
			if ((!show_tab && !node->IsHiddenTabBar()) || (show_tab && node->IsHiddenTabBar())) {
				node->WantHiddenTabBarToggle = true;
			}
		}

		ImVec2 cursor_screen_pos = ImGui::GetCursorScreenPos();
		uint32_t mouse_x = static_cast<uint32_t>(ImGui::GetMousePos().x - cursor_screen_pos.x);
		uint32_t mouse_y = static_cast<uint32_t>(ImGui::GetMousePos().y - cursor_screen_pos.y);

		ImVec2 content_size = ImGui::GetContentRegionAvail();
		ImGui::Image((ImTextureID)m_color_texture_desc_set, content_size);

		if (g_engine.isSimulating()) {
			updateCamera();

			ImGui::End();
			ImGui::PopStyleVar(2);
			return;
		}

		// Detect if the mouse is within the image bounds using Dummy
		// Invisible Button was originally used, but imguizmo added parameters to check for buttons
		ImGui::SetCursorScreenPos(cursor_screen_pos);
		ImGui::SetNextItemAllowOverlap();
		ImGui::Dummy(ImGui::GetContentRegionAvail());	// Dummy is used because it allows managing overlapping items

		// Check if the mouse is in the image and the left mouse button is clicked
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && (!m_selected_entity.lock() || !ImGuizmo::IsOver()) ) {
			g_engine.eventSystem()->syncDispatch(std::make_shared<PickEntityEvent>(mouse_x, mouse_y));
		}
		updateCamera();

		// allow drag from asset ui
		handleDragDropTarget(glm::vec2(mouse_x, mouse_y), glm::vec2(content_size.x, content_size.y));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.8f));
		ImGui::SetCursorPos( ImVec2(10.0f*m_res_scale, 30.0f*m_res_scale) );
		sprintf(m_title_buf, "%s view", ICON_FA_DICE_D6);
		if ( ImGui::Button(m_title_buf, ImVec2(64.0f*m_res_scale, 24.0f*m_res_scale)) ) {
			ImGui::OpenPopup("view");
		}

		static int view_index = 0;
		static const std::vector<std::string> views = {
			"perspective", "top", "bottom", "left", "right", "front", "back"
		};
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(-2.0f, -2.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f));
		if (constructRadioButtonPopup("view", views, view_index)) {
			static glm::vec3 last_camera_rotation;
			static glm::vec3 ortho_camera_rotations[6] = {
				glm::vec3(0.0f, 0.0f, -90.0f), glm::vec3(0.0f, 0.0f, 90.0f),
				glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(0.0f, -90.0f, 0.0f),
				glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)
			};

			if (view_index == 0) {
				m_camera_component.lock()->m_projection_type = EProjectionType::Perspective;
				m_camera_component.lock()->getTransformComponent()->m_rotation = last_camera_rotation;
			}
			else {
				if (m_camera_component.lock()->m_projection_type == EProjectionType::Perspective) {
					m_camera_component.lock()->m_projection_type = EProjectionType::Orthographic;
					last_camera_rotation = m_camera_component.lock()->getTransformComponent()->m_rotation;
				} 
				m_camera_component.lock()->getTransformComponent()->m_rotation = ortho_camera_rotations[view_index - 1];
			}
		}

		ImGui::SameLine();
		sprintf(m_title_buf, "%s shader", ICON_FA_BOWLING_BALL);
		if (ImGui::Button(m_title_buf, ImVec2(75.0f*m_res_scale, 24.0f*m_res_scale))) {
			ImGui::OpenPopup("shader");
		}

		static int shader_index = 0;
		static const std::vector<std::string> shaders = {
			"lit", "unlit", "wireframe", "lighting only", "depth", "normal", "base color", "emissive color",
			"metallic", "roughness", "occlusion", "opacity"
		};
		if (constructRadioButtonPopup("shader", shaders, shader_index)) {
			g_engine.renderSystem()->setShaderDebugOption(shader_index);
		}

		ImGui::SameLine();
		sprintf(m_title_buf, "%s show", ICON_FA_EYE);
		if (ImGui::Button(m_title_buf, ImVec2(64.0f*m_res_scale, 24.0f*m_res_scale))) {
			ImGui::OpenPopup("show");
		}

		constructCheckboxPopup("show", g_engine.m_render_settings);
		ImGui::PopStyleVar(3);

		constructOperationModeButtons();
		ImGui::PopStyleColor();

		constructImGuizmo();

		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void SimulationUI::destroy() {
		EditorUI::destroy();

		vkDestroySampler(VulkanRHI::get().getDevice(), m_color_texture_sampler, nullptr);
		ImGui_ImplVulkan_RemoveTexture(m_color_texture_desc_set);
	}

	void SimulationUI::onWindowResize() {
		// resize render pass
		g_engine.renderSystem()->resize(m_content_region.z, m_content_region.w);

		// recreate color image and view
		if (m_color_texture_desc_set != VK_NULL_HANDLE) {
			ImGui_ImplVulkan_RemoveTexture(m_color_texture_desc_set);
		}
		m_color_texture_desc_set = ImGui_ImplVulkan_AddTexture(m_color_texture_sampler, g_engine.renderSystem()->getColorImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void SimulationUI::loadAsset(const std::string& url) {
		const auto& as = g_engine.assetManager();
		EAssetType asset_type = as->getAssetType(url);
		std::string basename = g_engine.fileSystem()->basename(url);

		const auto& scene = g_engine.sceneManager()->getCurrentScene();
		m_created_entity = scene->createEntity(basename);

		if (asset_type == EAssetType::StaticMesh) {
			std::shared_ptr<StaticMeshComponent> static_mesh_component = std::make_shared<StaticMeshComponent>();
			std::shared_ptr<StaticMesh> static_mesh = as->loadAsset<StaticMesh>(url);
			static_mesh_component->setStaticMesh(static_mesh);
			m_created_entity->addComponent(static_mesh_component);
		}
		else if (asset_type == EAssetType::SkeletalMesh) {
			std::shared_ptr<SkeletalMeshComponent> skeletal_mesh_component = std::make_shared<SkeletalMeshComponent>();
			std::shared_ptr<SkeletalMesh> skeletal_mesh = as->loadAsset<SkeletalMesh>(url);
			skeletal_mesh_component->setSkeletalMesh(skeletal_mesh);
			m_created_entity->addComponent(skeletal_mesh_component);

			const auto& fs = g_engine.fileSystem();
			std::vector<std::string> filenames = fs->traverse(fs->absolute(fs->dir(url)));
			for (const std::string& filename : filenames) {
				URL asset_url(filename);
				EAssetType asset_type = as->getAssetType(asset_url);
				if (asset_type == EAssetType::Animation && !m_created_entity->hasComponent(AnimationComponent)) {
					std::shared_ptr<AnimationComponent> animation_component = std::make_shared<AnimationComponent>();
					std::shared_ptr<Animation> animation = as->loadAsset<Animation>(asset_url);
					animation_component->addAnimation(animation);
					m_created_entity->addComponent(animation_component);
				}
				else if (asset_type == EAssetType::Skeleton && !m_created_entity->hasComponent(AnimatorComponent)) {
					std::shared_ptr<AnimatorComponent> animator_component = std::make_shared<AnimatorComponent>();
					std::shared_ptr<Skeleton> skeleton = as->loadAsset<Skeleton>(asset_url);
					animator_component->setTickEnabled(true);
					animator_component->setSkeleton(skeleton);
					m_created_entity->addComponent(animator_component);
				}
			}

			m_created_entity->setTickEnabled(true);
			m_created_entity->setTickInterval(0.0167f);
		}
	}

	bool SimulationUI::constructRadioButtonPopup(const std::string& popup_name, const std::vector<std::string>& values, int& index) {
		bool is_radio_button_pressed = false;
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
		if (ImGui::BeginPopup(popup_name.c_str())) {
			for (size_t i = 0; i < values.size(); ++i) {
				if (ImGui::RadioButton(values[i].c_str(), &index, i))
					is_radio_button_pressed = true;
				if (i != values.size() - 1)
					ImGui::Spacing();
			}
			ImGui::EndPopup();
		}

		return is_radio_button_pressed;
	}

	void SimulationUI::constructCheckboxPopup(const std::string& popup_name, std::map<std::string, bool>& values) {
		static int show_debug_option = 0;
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
		if (ImGui::BeginPopup(popup_name.c_str())) {
			size_t index = 0;
			for (auto iter = values.begin(); iter != values.end(); ++iter) {
				if (ImGui::Checkbox(iter->first.c_str(), &iter->second)) {
					if (iter->second)
						show_debug_option |= (1 << index);
					else
						show_debug_option &= ~(1 << index);
					g_engine.renderSystem()->setShowDebugOption(show_debug_option);
				}
				if (iter != values.end())
					ImGui::Spacing();

				index++;
			}
			ImGui::EndPopup();
		}
	}

	void SimulationUI::constructOperationModeButtons() {
		std::vector<std::string> names = { ICON_FA_MOUSE_POINTER, ICON_FA_MOVE, ICON_FA_SYNC_ALT, ICON_FA_EXPAND };
		// 24.0f - button size at 1080p
		for (size_t i = 0; i < names.size(); ++i) {
			ImGui::SameLine(i == 0 ? ImGui::GetContentRegionAvail().x - (130.0f*m_res_scale) : 0.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, i == (size_t)m_operation_mode ? ImVec4(0.26f, 0.59f, 0.98f, 0.8f) : ImVec4(0.4f, 0.4f, 0.4f, 0.8f));
			if ( ImGui::Button(names[i].c_str(), ImVec2(24.0f*m_res_scale, 24.0f*m_res_scale)) )
				m_operation_mode = (EOperationMode)i;

			ImGui::PopStyleColor();
		}
	}

	ImVec2 WorldToScreen(const glm::vec3& worldPos, glm::mat4 viewMatrix, glm::mat4 projMatrix, float width, float height) {
		// Step 1: Transform world position to camera space (view space)
		glm::vec4 cameraSpacePos = viewMatrix * glm::vec4(worldPos, 1.0f);

		// Step 2: Project the camera space position into clip space
		glm::vec4 clipSpacePos = projMatrix * cameraSpacePos;

		// Step 3: Convert from clip space to NDC (Normalized Device Coordinates)
		// Clip space coordinates range from -1 to 1 for all axes
		glm::vec3 ndc = glm::vec3(clipSpacePos) / clipSpacePos.w;

		// Step 4: Convert NDC to screen space (window space)
		// NDC to screen space: x and y are mapped from [-1, 1] to [0, windowWidth] and [0, windowHeight] respectively
		float x = (ndc.x + 1.0f) * 0.5f * width;
		float y = (1.0f - (ndc.y + 1.0f) * 0.5f) * height; // y is inverted for screen coordinates

		return ImVec2(x, y);
	}

	void SimulationUI::constructImGuizmo() {
 		if (!m_selected_entity.lock())
 			return;
		
		glm::inverse(m_camera_component.lock()->getViewMatrix());
		m_camera_component.lock()->getProjectionMatrix();
		
		// set translation/rotation/scale gizmos
		ImGuizmo::PushID(0);
		ImGuizmo::BeginFrame();
		ImGuizmo::SetOrthographic(m_camera_component.lock()->m_projection_type == EProjectionType::Orthographic);
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
		ImGuizmo::SetDrawlist();
		/*
		ImGuizmo::DrawGrid(glm::value_ptr(glm::inverse(m_camera_component.lock()->getViewMatrix())), 
			glm::value_ptr(m_camera_component.lock()->getProjectionMatrix()), 
			glm::value_ptr(glm::mat4(1.0f)), 100.0f);
		*/

		const float* p_view = glm::value_ptr(m_camera_component.lock()->getViewMatrix());
		const float* p_projection = glm::value_ptr(m_camera_component.lock()->getProjectionMatrixNoYInverted());

		auto transform_component = m_selected_entity.lock()->getComponent(TransformComponent);
		glm::mat4 matrix = transform_component->getGlobalMatrix();
		if (m_operation_mode != EOperationMode::Pick) {
			ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
			if (m_operation_mode == EOperationMode::Rotate)
				operation = ImGuizmo::ROTATE;
			else if (m_operation_mode == EOperationMode::Scale)
				operation = ImGuizmo::SCALE;

			glm::mat4 delta_matrix = glm::mat4(1.0);
			ImGuizmo::Manipulate(p_view, p_projection, operation, (ImGuizmo::MODE)m_coordinate_mode, 
				glm::value_ptr(matrix), glm::value_ptr(delta_matrix), nullptr, nullptr, nullptr);

			// only set gizmo's transformation to selected entity if the simulation window is focused
			if (ImGui::IsWindowFocused()) {
				glm::vec3 translation, rotation, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(matrix), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));

				if (m_operation_mode == EOperationMode::Translate)
					transform_component->m_position = translation;
				else if (m_operation_mode == EOperationMode::Rotate)
					transform_component->m_rotation = rotation;
				else if (m_operation_mode == EOperationMode::Scale)
					transform_component->m_scale = scale;
			}
		}
		ImGuizmo::PopID();
	}

	void SimulationUI::onKey(const std::shared_ptr<class Event>& event) {
		const WindowKeyEvent* key_event = static_cast<const WindowKeyEvent*>(event.get());
		if (key_event->action != true)
			return;
		if (key_event->key == SDL_SCANCODE_F11)
			g_editor.toggleFullscreen();

		if (g_engine.isEditor()) {
			ESceneMode current_scene_mode = g_engine.sceneManager()->getSceneMode();
			if ((key_event->mods & SDL_KMOD_ALT) && key_event->key == SDL_SCANCODE_P) {
				if (current_scene_mode == ESceneMode::Edit)
					g_engine.sceneManager()->setSceneMode(ESceneMode::Play);
			} 
			else if (key_event->key == SDL_SCANCODE_ESCAPE) {
				if (current_scene_mode == ESceneMode::Play)
					g_engine.sceneManager()->setSceneMode(ESceneMode::Edit);
			}
			if (m_selected_entity.lock()) {
				uint32_t selected_entity_id = m_selected_entity.lock()->getID();
				//if (key_event->key == SDL_SCANCODE_ESCAPE || key_event->key == SDL_SCANCODE_DELETE) {
				if (key_event->key == SDL_SCANCODE_ESCAPE)
					g_engine.eventSystem()->syncDispatch(std::make_shared<SelectEntityEvent>(UINT_MAX));
				if (key_event->key == SDL_SCANCODE_DELETE)
					g_engine.sceneManager()->getCurrentScene()->removeEntity(selected_entity_id);
			}
			if (!isFocused())
				return;

			if (m_selected_entity.lock() || !m_mouse_right_button_pressed) {
				switch (key_event->key) {
				case SDL_SCANCODE_Q:
					m_operation_mode = EOperationMode::Pick;
					break;
				case SDL_SCANCODE_W:
					m_operation_mode = EOperationMode::Translate;
					break;
				case SDL_SCANCODE_E:
					m_operation_mode = EOperationMode::Rotate;
					break;
				case SDL_SCANCODE_R:
					m_operation_mode = EOperationMode::Scale;
					break;
				default:
					break;
				}
			}
		}
	}

	void SimulationUI::onSelectEntity(const std::shared_ptr<class Event>& event) {
		const SelectEntityEvent* p_event = static_cast<const SelectEntityEvent*>(event.get());

		if (p_event->entity_id != m_camera_component.lock()->getParent().lock()->getID()) {
			const auto& current_scene = g_engine.sceneManager()->getCurrentScene();
			m_selected_entity = current_scene->getEntity(p_event->entity_id);
		}
	}

	void SimulationUI::updateCamera() {
		// try to get new camera if current camera is not valid
		if (!m_camera_component.lock())
			m_camera_component = g_engine.sceneManager()->getCameraComponent();

		// set camera component
		m_camera_component.lock()->m_aspect_ratio = (float)m_content_region.z / m_content_region.w;
		if (g_engine.isSimulating())
			m_mouse_right_button_pressed = ImGui::IsMouseDown(ImGuiMouseButton_Right);
		else
			m_mouse_right_button_pressed = ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right);
		
		m_camera_component.lock()->setInput(m_mouse_right_button_pressed, isFocused());
	}

	void SimulationUI::handleDragDropTarget(const glm::vec2& mouse_pos, const glm::vec2& viewport_size) {
		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = nullptr;
			ImGuiDragDropFlags flags = ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoPreviewTooltip;
			
			// vscode issues warnings if value is assigned in an if conditional, so warning was disabled for this code block
			#pragma clang diagnostic push
			#pragma clang diagnostic ignored "-Wparentheses"
			if (payload = ImGui::AcceptDragDropPayload("load_asset", flags)) {
				if (!m_created_entity) {
					std::string url((const char*)payload->Data, payload->DataSize);
					StopWatch stop_watch;
					stop_watch.start();
					loadAsset(url);
					LOG_INFO("load asset {}, elapsed time: {}ms", url, stop_watch.stopMs());
				}
			}
			else if (payload = ImGui::AcceptDragDropPayload("create_entity", flags)) {
				if (!m_created_entity) {
					const auto& scene = g_engine.sceneManager()->getCurrentScene();
					std::string playload_str((const char*)payload->Data, payload->DataSize);
					std::vector<std::string> splits = StringUtil::split(playload_str, "-");
					const std::string& entity_category = splits[0];
					const std::string& entity_type = splits[1];

					if (entity_category == "Entities") {
						m_created_entity = scene->createEntity(entity_type);
					}
					else if (entity_category == "Lights") {
						m_created_entity = scene->createEntity(entity_type);
						auto transform_component = m_created_entity->getComponent(TransformComponent);

						// add light component
						if (entity_type.find("Directional") != std::string::npos) {
							transform_component->setRotation(glm::vec3(0.0f, 135.0f, -35.2f));
							m_created_entity->addComponent(std::make_shared<DirectionalLightComponent>());
						}
						else if (entity_type.find("Sky") != std::string::npos) {
							auto sky_light_component = std::make_shared<SkyLightComponent>();
							auto sky_texture_cube = g_engine.assetManager()->loadAsset<TextureCube>(DEFAULT_TEXTURE_CUBE_URL);
							sky_light_component->setTextureCube(sky_texture_cube);
							m_created_entity->addComponent(sky_light_component);
						}
						else if (entity_type.find("Point") != std::string::npos) {
							m_created_entity->addComponent(std::make_shared<PointLightComponent>());
						}
						else if (entity_type.find("Spot") != std::string::npos) {
							m_created_entity->addComponent(std::make_shared<SpotLightComponent>());
						}
					}
					else if (entity_category == "Primitives") {
						std::string url = StringUtil::format("asset/engine/mesh/primitive/sm_%s.sm", entity_type.c_str());
						loadAsset(url);
					}
				}
			}
			#pragma clang diagnostic pop

			if (m_created_entity) {
				glm::vec3 place_pos = calcPlacePos(mouse_pos, viewport_size);
				m_created_entity->getComponent(TransformComponent)->m_position = place_pos;
			}
			if (payload && payload->IsDelivery())
				m_created_entity = nullptr;

			ImGui::EndDragDropTarget();
		}
	}

	glm::vec3 SimulationUI::calcPlacePos(const glm::vec2& mouse_pos, const glm::vec2& viewport_size) {
		glm::vec3 ray_origin = glm::unProjectZO(glm::vec3(mouse_pos.x, mouse_pos.y, 0.0f), m_camera_component.lock()->getViewMatrix(), 
			m_camera_component.lock()->getProjectionMatrix(), glm::vec4(0.0f, 0.0f, viewport_size.x, viewport_size.y));
		glm::vec3 ray_target = glm::unProjectZO(glm::vec3(mouse_pos.x, mouse_pos.y, 1.0f), m_camera_component.lock()->getViewMatrix(),
			m_camera_component.lock()->getProjectionMatrix(), glm::vec4(0.0f, 0.0f, viewport_size.x, viewport_size.y));
		glm::vec3 ray_dir = glm::normalize(ray_target - ray_origin);
		float t = -ray_origin.y / ray_dir.y;

		glm::vec3 place_pos = ray_origin + ray_dir * t;
		return place_pos;
	}

}