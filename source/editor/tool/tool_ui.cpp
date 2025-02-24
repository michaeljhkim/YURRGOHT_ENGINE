
#include <map>

#include "tool_ui.h"
#include "engine/core/base/macro.h"
#include "engine/function/framework/scene/scene_manager.h"
#include "engine/function/physics/physics_system.h"

namespace Yurrgoht {

	void ToolUI::init() {
		m_title = "Tool";
		m_res_scale = g_engine.windowSystem()->getResolutionScale();
		m_first_init = true;

		entity_categories = {
			std::string(ICON_FA_CUBE) + " Entities",
			std::string(ICON_FA_LIGHTBULB) + " Lights",
			std::string(ICON_FA_SHAPES) + " Primitives"
		};

		entity_typess = {
			{ std::string(ICON_FA_ASTERISK) + " Empty Entity" },
			{ std::string(ICON_FA_SUN) + " Directional Light",
			  std::string(ICON_FA_CLOUD_MEATBALL) + " Sky Light",
			  std::string(ICON_FA_LIGHTBULB) + " Point Light",
			  std::string(ICON_FA_FLASH_LIGHT) + " Spot Light" },
			{ std::string(ICON_FA_STAR) + " Cube",
			  std::string(ICON_FA_STAR) + " Sphere",
			  std::string(ICON_FA_STAR) + " Cylinder",
			  std::string(ICON_FA_STAR) + " Cone",
			  std::string(ICON_FA_STAR) + " Plane" }
		};

		const auto& entity_class_names = g_engine.sceneManager()->getCurrentScene()->getEntityClassNames();
		if (!entity_class_names.empty()) {
			for (const std::string& entity_class_name : entity_class_names) {
				entity_typess.front().push_back(std::string(ICON_FA_ASTERISK) + " " + entity_class_name);
			}
		}
		LOG_INFO("SUCCESS");
	}

	void ToolUI::construct() {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar;
		sprintf(m_title_buf, "%s %s###%s", ICON_FA_TOOLS, m_title.c_str(), m_title.c_str());
		if (!ImGui::Begin(m_title_buf, nullptr, window_flags)) {
			ImGui::End();
			return;
		}
		if (m_first_init) {
			ImGuiDockNode* dock_node = ImGui::FindWindowByName(m_title_buf)->DockNode;	// Retrieve the dock node for the current window
			ImGui::DockBuilderSetNodeSize(dock_node->ID, ImVec2(dock_node->Size.x, 38 * m_res_scale));
			dock_node->SetLocalFlags(ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoTabBar);
			m_first_init = false;
		}

		float region_w = ImGui::GetContentRegionAvail().x;
		ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
		const auto& wm = g_engine.sceneManager();

		// save button 
		const float kButtonHeight = 30.0f * m_res_scale;
		const ImVec2 kButtonSize = ImVec2(kButtonHeight, kButtonHeight);

		// center save button (have to center it manually since font is off center)
		//ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.85f, 0.5f));
		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		//ImGui::PopStyleVar(2);	// restore button style
		// center all buttons on y-axis
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImGui::SetCursorPosY((windowSize.y - kButtonHeight) * 0.5f);

		ImGui::PushFont(bigIconFont());
		if (ImGui::Button(ICON_FA_SAVE, kButtonSize)) {
			wm->saveScene();
		}
		ImGui::PopFont();

		// create entities
		ImGui::SameLine();
		sprintf(m_title_buf, "Create %s", ICON_FA_CHEVRON_DOWN);
		const float kButtonWidth = 80.0f * m_res_scale;
		if (ImGui::Button(m_title_buf, ImVec2(kButtonWidth, kButtonHeight))) {
			ImGui::OpenPopup("create_entity");
		}
		constructCreateEntityPopup();

		// play/step/stop
		ImGui::SameLine();
		ImGui::SetCursorPosX(region_w * 0.5f - kButtonSize.x * 1.5f - spacing.x);

		ESceneMode current_scene_mode = wm->getSceneMode();
		const char* play_icon_text = current_scene_mode == ESceneMode::Edit ? ICON_FA_PLAY :
			(current_scene_mode == ESceneMode::Play ? ICON_FA_PAUSE : ICON_FA_FORWARD);
		if (ImGui::Button(play_icon_text, kButtonSize)) {
			switch (current_scene_mode) {
			case ESceneMode::Edit:
				wm->setSceneMode(ESceneMode::Play);
				break;
			case ESceneMode::Play:
				wm->setSceneMode(ESceneMode::Pause);
				break;
			case ESceneMode::Pause:
				wm->setSceneMode(ESceneMode::Play);
				break;
			default:
				break;
			}
		}

		ImGui::SameLine();
		if (current_scene_mode != ESceneMode::Pause) {
			ImGui::BeginDisabled(true);
		}
		if (ImGui::Button(ICON_FA_STEP_FORWARD, kButtonSize)) {
			g_engine.physicsSystem()->step();
			wm->getCurrentScene()->step();
		}
		if (current_scene_mode != ESceneMode::Pause) {
			ImGui::EndDisabled();
		}

		ImGui::SameLine();
		if (current_scene_mode == ESceneMode::Edit) {
			ImGui::BeginDisabled(true);
		}
		if (ImGui::Button(ICON_FA_STOP, kButtonSize)) {
			wm->setSceneMode(ESceneMode::Edit);
		}
		if (current_scene_mode == ESceneMode::Edit) {
			ImGui::EndDisabled();
		}

		ImGui::End();
	}

	void ToolUI::constructCreateEntityPopup() {
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
		if (ImGui::BeginPopup("create_entity")) {
			for ( size_t i = 0; i < entity_categories.size(); ++i ) {
				if ( ImGui::BeginMenu(entity_categories[i].c_str()) ) {
					for (size_t j = 0; j < entity_typess[i].size(); ++j) {
						const std::string& entity_type = entity_typess[i][j];
						ImGui::Text("%s", entity_type.c_str());
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID | ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
							std::string playload_str = std::string(entity_categories[i].begin() + 4, entity_categories[i].end()) + "-" +
								std::string(entity_type.begin() + 4, entity_type.end());
							ImGui::SetDragDropPayload("create_entity", playload_str.data(), playload_str.size());
							ImGui::EndDragDropSource();
						}

						if (j != entity_typess[i].size() - 1) {
							ImGui::Separator();
						}
					}
					ImGui::EndMenu();
				}
			} 
			ImGui::EndPopup();
		}
	}

}