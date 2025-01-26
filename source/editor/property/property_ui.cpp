#include "property_ui.h"
#include "engine/core/event/event_system.h"
#include "engine/resource/asset/asset_manager.h"
#include "engine/function/framework/scene/scene_manager.h"
#include "engine/core/color/color.h"

#define DRAG_SPEED 0.1f

namespace Yurrgoht {

	void PropertyUI::init() {
		m_title = "Property";
		m_res_scale = g_engine.windowSystem()->getResolutionScale();
		m_property_constructors = {
			{ EPropertyValueType::Bool, std::bind(&PropertyUI::constructPropertyBool, this, std::placeholders::_1, std::placeholders::_2) },
			{ EPropertyValueType::Integar, std::bind(&PropertyUI::constructPropertyIntegar, this, std::placeholders::_1, std::placeholders::_2) },
			{ EPropertyValueType::Float, std::bind(&PropertyUI::constructPropertyFloat, this, std::placeholders::_1, std::placeholders::_2) },
			{ EPropertyValueType::String, std::bind(&PropertyUI::constructPropertyString, this, std::placeholders::_1, std::placeholders::_2) },
			{ EPropertyValueType::Vec2, std::bind(&PropertyUI::constructPropertyVec2, this, std::placeholders::_1, std::placeholders::_2) },
			{ EPropertyValueType::Vec3, std::bind(&PropertyUI::constructPropertyVec3, this, std::placeholders::_1, std::placeholders::_2) },
			{ EPropertyValueType::Vec4, std::bind(&PropertyUI::constructPropertyVec4, this, std::placeholders::_1, std::placeholders::_2) },
			{ EPropertyValueType::Color3, std::bind(&PropertyUI::constructPropertyColor3, this, std::placeholders::_1, std::placeholders::_2) },
			{ EPropertyValueType::Color4, std::bind(&PropertyUI::constructPropertyColor4, this, std::placeholders::_1, std::placeholders::_2) },
			{ EPropertyValueType::Asset, std::bind(&PropertyUI::constructPropertyAsset, this, std::placeholders::_1, std::placeholders::_2) },
		};

		g_engine.eventSystem()->addListener(EEventType::SelectEntity, std::bind(&PropertyUI::onSelectEntity, this, std::placeholders::_1));

		// get dummy texture2d
		m_dummy_image = loadImGuiImageFromImageViewSampler(g_engine.assetManager()->getDefaultTexture2D());
		LOG_INFO("SUCCESS");
	}

	void PropertyUI::construct() {
		auto selected_entity = m_selected_entity.lock();
		std::string entity_name = selected_entity ? selected_entity->getName() : "";
		sprintf(m_title_buf, "%s %s###%s", ICON_FA_STREAM, (entity_name.empty() ? m_title : entity_name).c_str(), m_title.c_str());
		if (!ImGui::Begin(m_title_buf)) {
			ImGui::End();
			return;
		}

		if (selected_entity) {
			// FOR ImGuiTableFlags_ContextMenuInBody, MUST CHANGE CONTEXT MENU (look into it first)
			ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_ContextMenuInBody;
			if (ImGui::BeginTable("components", 2, table_flags)) {
				ImGui::TableSetupColumn("column_0", ImGuiTableColumnFlags_WidthFixed, 100.0f*m_res_scale);
				ImGui::TableSetupColumn("column_1", ImGuiTableColumnFlags_WidthStretch);

				// CONSTRUCT ENTITY IS CALLED HERE
				constructEntity( selected_entity->meta_poly_ptr() );
				for (const auto& component : selected_entity->getComponents()) {
					constructEntity( component->meta_poly_ptr() );
				}
				ImGui::EndTable();
			}
		} else {
			const char* hint_text = "select any entity to display properties";
			float window_width = ImGui::GetWindowSize().x;
			float window_height = ImGui::GetWindowSize().y;

			//ImGui::SetCursorPosX((window_width - (ImGui::CalcTextSize(hint_text).x)) * 0.5f);
			ImGui::SetCursorPosY(window_height * 0.2f);
			// Enable word wrapping
			ImGui::PushTextWrapPos(window_width * 0.9f);  // Wrap at 90% of the window width
			ImGui::Text("%s", hint_text);
			ImGui::PopTextWrapPos();  // Restore the previous wrap position
		}

		ImGui::End();
	}

	void PropertyUI::onSelectEntity(const std::shared_ptr<class Event>& event) {
		const SelectEntityEvent* p_event = static_cast<const SelectEntityEvent*>(event.get());

		const auto& current_scene = g_engine.sceneManager()->getCurrentScene();
		m_selected_entity = current_scene->getEntity(p_event->entity_id);
	}
	
	void PropertyUI::constructEntity(const meta_hpp::uvalue& u_instance) {
		const auto p_entity = u_instance.try_as<Entity*>();
		const auto p_component = u_instance.try_as<Component*>();
		auto properties = u_instance.get_type().as_pointer().get_data_type().as_class().get_members();

		// In case a component has parent class members we need to get values from
		// NOTE: this only goes up to the IMMEDIATE parents and not grandparents or such
		for (const auto& parent_class : u_instance.get_type().as_pointer().get_data_type().as_class().get_base_classes()) {
			auto& members = parent_class.get_members();		// for cleanliness only really
			properties.insert(properties.end(), members.begin(), members.end());
		}
		//std::cout << "properties size: " << properties.size() << std::endl;

		if (p_entity != nullptr && properties.empty()) {
			return;
		}

		// add name title
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ImGuiTreeNodeFlags tree_node_flags = 0;
		tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap;
		const std::string& type_name = (p_component != nullptr) ? p_component->getTypeName() : "Entity";
		std::string title = type_name.substr(0, type_name.length() - 9);
		bool is_tree_open = ImGui::TreeNodeEx(title.c_str(), tree_node_flags);

		// add option button
		ImGui::TableNextColumn();
		ImVec4 rect_color = ImGui::IsItemActive() ? ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive) : (
			ImGui::IsItemHovered() ? ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered) : ImGui::GetStyleColorVec4(ImGuiCol_Header)
		);
		ImVec2 p_min = ImGui::GetCursorScreenPos();
		ImVec2 p_max = ImVec2(p_min.x + ImGui::GetContentRegionAvail().x, p_min.y + ImGui::GetItemRectSize().y);
		ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, ImGui::GetColorU32(rect_color));

		if (p_component != nullptr) {
			ImVec4 button_color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			button_color.w = 0.0f;
			ImGui::PushStyleColor(ImGuiCol_Button, button_color);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 50);
			if (ImGui::Button(ICON_FA_PLUS)) {
				// THIS PART ACTUALLY NEEDS CODE!!!
				LOG_INFO("add component");
			}
			ImGui::SameLine(0, 5);
			if (ImGui::Button(ICON_FA_TRASH)) {
				LOG_INFO("remove component");
			}
			ImGui::PopStyleColor();
		}

		// add properties
		if (is_tree_open) {
			ImGui::PushFont(smallFont());
			ImGui::TableNextRow();

			for (auto& prop : properties) {
				std::string prop_name = prop.get_name();
				EPropertyType property_type = getPropertyType(prop.get_metadata().find("type_name")->second);
				ASSERT(property_type.second != EPropertyContainerType::Map, "don't support map container property type now");
				
				meta_hpp::uvalue variant = prop.get(u_instance);	//the get value just returns a member
				if (property_type.second == EPropertyContainerType::Mono) {
					m_property_constructors[property_type.first](prop_name, variant);
					prop.set(u_instance, variant);
				}
				else if (property_type.second == EPropertyContainerType::Array) {
					// meta.hpp has no method of setting vectors at the moment, so a class must have such method first.
					// for animation component only (at the moment), since it is the only class with a vector
					auto u_setter = u_instance.get_type().as_pointer().get_data_type().as_class().get_method("set_vector");

					for (size_t i = 0; i < variant.size(); ++i) {
						meta_hpp::uvalue sub_variant = *variant[i];
						std::string sub_prop_name = prop_name + "_" + std::to_string(i);
						m_property_constructors[property_type.first](sub_prop_name, sub_variant);
						u_setter.invoke(u_instance, i, sub_variant);
					}
					prop.set(u_instance, variant);
				}
			}
			ImGui::TreePop();
			ImGui::PopFont();
		}
	}

	void PropertyUI::constructPropertyBool(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::TableNextColumn();
		std::string label = "##" + name;
		ImGui::Checkbox(label.c_str(), &instance.as<bool>());
	}

	void PropertyUI::constructPropertyIntegar(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::TableNextColumn();
		std::string label = "##" + name;
		ImGui::InputInt(label.c_str(), &instance.as<int>());
	}

	void PropertyUI::constructPropertyFloat(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);
		
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		ImGui::InputFloat(label.c_str(), &instance.as<float>());
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyString(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::TableNextColumn();
		ImGui::Text("%s", (instance.as<std::string>()).c_str());
	}

	void PropertyUI::constructPropertyVec2(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		glm::vec2& vec2 = instance.as<glm::vec2>();
		ImGui::DragFloat2(label.c_str(), glm::value_ptr(vec2), DRAG_SPEED);
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyVec3(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		glm::vec3& vec3 = instance.as<glm::vec3>();
		ImGui::DragFloat3(label.c_str(), glm::value_ptr(vec3), DRAG_SPEED);
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyVec4(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		glm::vec4 vec4 = instance.as<glm::vec4>();
		ImGui::DragFloat4(label.c_str(), glm::value_ptr(vec4), DRAG_SPEED);
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyColor3(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		Color3& color3 = instance.as<Color3>();
		ImGui::ColorEdit3(label.c_str(), color3.data());
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyColor4(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		Color4& color4 = instance.as<Color4>();
		ImGui::ColorEdit3(label.c_str(), color4.data());
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyAsset(const std::string& name, meta_hpp::uvalue& instance) {
		ImGui::TableNextColumn();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
		ImGui::Text("%s", name.c_str());

		ImGui::TableNextColumn();

		// asset preview image
		const ImVec2 icon_size(60, 60);
		ImGui::Image((ImTextureID)m_dummy_image->tex_id, icon_size);

		// asset find combo box
		ImGui::SameLine();
		const char* asset_names[] = { "asset_0", "asset_1", "asset_2", "asset_3" };
		int selected_index = 0;
		const char* preview_value = asset_names[selected_index];
		ImGuiComboFlags combo_flags = 0;
		if (ImGui::BeginCombo("##select_asset", preview_value, combo_flags)) {
			for (int i = 0; i < IM_ARRAYSIZE(asset_names); ++i) {
				const bool is_selected = selected_index == i;
				if (ImGui::Selectable(asset_names[i], is_selected))
					selected_index = i;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	void PropertyUI::addPropertyNameText(const std::string& name) {
		ImGui::TableNextColumn();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
		ImGui::Text("%s", name.c_str());
	}

	EPropertyType PropertyUI::getPropertyType(const meta_hpp::uvalue& type) {
		const std::string& type_name = type.as<std::string>();

		EPropertyValueType value_type = EPropertyValueType::Bool;
		EPropertyContainerType container_type = EPropertyContainerType::Mono;
		if (type_name.find("std::vector") != std::string::npos)
			container_type = EPropertyContainerType::Array;
		else if (type_name.find("std::map") != std::string::npos)
			container_type = EPropertyContainerType::Map;

		if (type_name.find("glm::vec2") != std::string::npos)
			value_type = EPropertyValueType::Vec2;
		else if (type_name.find("glm::vec3") != std::string::npos)
			value_type = EPropertyValueType::Vec3;
		else if (type_name.find("glm::vec4") != std::string::npos)
			value_type = EPropertyValueType::Vec4;
		else if (type_name.find("Color3") != std::string::npos)
			value_type = EPropertyValueType::Color3;
		else if (type_name.find("Color4") != std::string::npos)
			value_type = EPropertyValueType::Color4;
		else if (type_name.find("std::shared_ptr") != std::string::npos)
			value_type = EPropertyValueType::Asset;
		else if (type_name.find("bool") != std::string::npos)
			value_type = EPropertyValueType::Bool;
		else if (type_name.find("int") != std::string::npos)
			value_type = EPropertyValueType::Integar;
		else if (type_name.find("float") != std::string::npos) {
			value_type = EPropertyValueType::Float;
		}
		else if (type_name.find("std::string") != std::string::npos)
			value_type = EPropertyValueType::String;
		
		return std::make_pair(value_type, container_type);
	}

}