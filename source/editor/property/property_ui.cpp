#include "property_ui.h"
#include "engine/core/event/event_system.h"
#include "engine/resource/asset/asset_manager.h"
#include "engine/function/framework/scene/scene_manager.h"
#include "engine/core/color/color.h"

#include "engine/function/framework/component/animation_component.h"
#include "engine/function/framework/component/animator_component.h"
#include "engine/function/framework/component/camera_component.h"
#include "engine/function/framework/component/rigidbody_component.h"
#include "engine/function/framework/component/transform_component.h"
#include "engine/function/framework/component/skeletal_mesh_component.h"
#include "engine/function/framework/component/static_mesh_component.h"

#include "engine/function/framework/component/directional_light_component.h"
#include "engine/function/framework/component/light_component.h"
#include "engine/function/framework/component/point_light_component.h"
#include "engine/function/framework/component/sky_light_component.h"
#include "engine/function/framework/component/spot_light_component.h"

#include "engine/function/framework/component/box_collider_component.h"
#include "engine/function/framework/component/capsule_collider_component.h"
#include "engine/function/framework/component/collider_component.h"
#include "engine/function/framework/component/cylinder_collider_component.h"
#include "engine/function/framework/component/mesh_collider_component.h"
#include "engine/function/framework/component/sphere_collider_component.h"


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
			{ EPropertyValueType::EMotion, std::bind(&PropertyUI::constructPropertyEMotion, this, std::placeholders::_1, std::placeholders::_2) },
		};

		m_property_components = {
			//{"AnimationComponent"			, std::bind(&PropertyUI::CreateComponent<AnimationComponent>, this)			},
			{"AnimatorComponent"			, std::bind(&PropertyUI::CreateComponent<AnimatorComponent>, this)			},
			{"BoxColliderComponent"			, std::bind(&PropertyUI::CreateComponent<BoxColliderComponent>, this)		},
			{"CameraComponent"	   			, std::bind(&PropertyUI::CreateComponent<CameraComponent>, this)			},
			{"CapsuleColliderComponent"		, std::bind(&PropertyUI::CreateComponent<CapsuleColliderComponent>, this)	},
			{"ColliderComponent"			, std::bind(&PropertyUI::CreateComponent<ColliderComponent>, this)			},
			{"CylinderColliderComponent" 	, std::bind(&PropertyUI::CreateComponent<CylinderColliderComponent>, this)	},
			{"DirectionalLightComponent"	, std::bind(&PropertyUI::CreateComponent<DirectionalLightComponent>, this)	},
			//{"LightComponent"				, std::bind(&PropertyUI::CreateComponent<LightComponent>, this)				},
			{"MeshColliderComponent"		, std::bind(&PropertyUI::CreateComponent<MeshColliderComponent>, this)		},
			{"PointLightComponent"			, std::bind(&PropertyUI::CreateComponent<PointLightComponent>, this)		},
			{"RigidbodyComponent"			, std::bind(&PropertyUI::CreateComponent<RigidbodyComponent>, this)			},
			{"SkeletalMeshComponent"		, std::bind(&PropertyUI::CreateComponent<SkeletalMeshComponent>, this)		},
			{"SkyLightComponent"			, std::bind(&PropertyUI::CreateComponent<SkyLightComponent>, this)			},
			{"SphereColliderComponent"		, std::bind(&PropertyUI::CreateComponent<SphereColliderComponent>, this)	},
			{"SpotLightComponent"			, std::bind(&PropertyUI::CreateComponent<SpotLightComponent>, this)			},
			{"StaticMeshComponent"			, std::bind(&PropertyUI::CreateComponent<StaticMeshComponent>, this)		},
			//{"TransformComponent"			, std::bind(&PropertyUI::CreateComponent<TransformComponent>, this)			},
			// should probably never remove TransformComponent because it will most likely always be required
		};

		g_engine.eventSystem()->addListener(EEventType::SelectEntity, std::bind(&PropertyUI::onSelectEntity, this, std::placeholders::_1));

		// get dummy texture2d
		m_dummy_image = loadImGuiImageFromImageViewSampler(g_engine.assetManager()->getDefaultTexture2D());

		// set the viewport flag to  
		noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;

		// icon + label - buffer creation
		std::string m_plus_name = "Manage Components"; 
		sprintf(m_plus_buf, "%s %s###%s", ICON_FA_COG, m_plus_name.c_str(), m_plus_name.c_str());
		std::string m_trash_name = "remove component"; 
		sprintf(m_trash_buf, "%s %s###%s", ICON_FA_TRASH, m_trash_name.c_str(), m_trash_name.c_str());

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
			// CONSTRUCT ENTITY IS CALLED HERE
			constructEntity( selected_entity->meta_poly_ptr() );
			for (const auto& component : selected_entity->getComponents()) {
				const std::string& type_name = component->getTypeName();
				std::string title = type_name.substr(0, type_name.length() - 9);

				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.1f, 0.1f));
				ImGuiChildFlags child_flags = ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders;

				if (ImGui::BeginChild(title.c_str(), ImVec2(0, 0), child_flags)) {
					// add name title
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
					ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap | 
						ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth;
					if (ImGui::TreeNodeEx(title.c_str(), tree_node_flags)) {
						//ImGui::Spacing();
						ImGui::Unindent();
						constructEntity( component->meta_poly_ptr() );
						ImGui::TreePop();
					}
					ImGui::PopStyleVar();
					ImGui::EndChild();
				}
				ImGui::PopStyleVar(2);
			}

			if (ImGui::Button(m_plus_buf)) {	
				LOG_INFO("manage components");
				showing_component_manager = true;
			}
			if (showing_component_manager)
				componentManager(selected_entity->meta_poly_ptr());
		} 
		else {
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

		/*
		- In case a component has parent class members we need to get values from
		- Basic premise is if the parent classes themselves also have a parent, we insert the elements from their base class list into the deque
		- For each class in the base class list, we get the reflected members from the members list.
		- This will allow us to get all reflected members, regardless of how many levels of inheritance
		*/
		std::deque<meta_hpp::class_type> parent_classes(
			u_instance.get_type().as_pointer().get_data_type().as_class().get_base_classes().begin(), 
			u_instance.get_type().as_pointer().get_data_type().as_class().get_base_classes().end()
		);
		while (!parent_classes.empty()) {
			auto current_class = parent_classes.front();
			parent_classes.pop_front();

			// If there are parent classes to this current class, we insert it into the deque to process their members 
			if (!current_class.get_base_classes().empty())
				parent_classes.insert(parent_classes.end(), current_class.get_base_classes().begin(), current_class.get_base_classes().end());

			// We insert the members of the current class if it has reflected members
			if (!current_class.get_members().empty())
				properties.insert(properties.end(), current_class.get_members().begin(), current_class.get_members().end());
		}

		// if it is not an entity and it does not have properties (for component)
		if (p_entity != nullptr && properties.empty())
			return;


		// add option button
		/*
		ImVec4 rect_color = ImGui::IsItemActive() ? ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive) : (
			ImGui::IsItemHovered() ? ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered) : ImGui::GetStyleColorVec4(ImGuiCol_Header)
		);
		ImVec2 p_min = ImGui::GetCursorScreenPos();
		ImVec2 p_max = ImVec2(p_min.x + ImGui::GetContentRegionAvail().x, p_min.y + ImGui::GetItemRectSize().y);
		ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, ImGui::GetColorU32(rect_color));
		
		ImGui::SameLine();
		if (p_component != nullptr) {
			//ImVec4 button_color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			//button_color.w = 0.0f;
			//ImGui::PushStyleColor(ImGuiCol_Button, button_color);
			
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.0f, 3.0f));
			ImGuiChildFlags child_flags = ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Borders;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 60.0f*m_res_scale);
			ImGui::BeginChild("#header_buttons", ImVec2(0, 0), child_flags);
			
			if (ImGui::Button(ICON_FA_TRASH)) {
				LOG_INFO("remove component");
			}
			ImGui::EndChild();
			
			ImGui::PopStyleVar();
			//ImGui::PopStyleColor();
		}
		*/

		// add properties
		for (auto& prop : properties) {
			std::string prop_name = prop.get_name();
			EPropertyType property_type = getPropertyType(prop.get_metadata().find("type_name")->second);
			ASSERT(property_type.second != EPropertyContainerType::Map, "don't support map container property type (for now)");
			
			meta_hpp::uvalue variant = prop.get(u_instance);	//the get value just returns a member
			if (property_type.second == EPropertyContainerType::Mono) {
				m_property_constructors[property_type.first](prop_name, variant);
				prop.set(u_instance, variant);
			}
			else if (property_type.second == EPropertyContainerType::Array) {
				// meta.hpp has no method of setting vectors at the moment, so a class must have such method first.
				// for animation component only (at the moment), since it is the only class with an exposed vector 
				auto u_setter = u_instance.get_type().as_pointer().get_data_type().as_class().get_method("set_vector");

				for (size_t i = 0; i < variant.size(); ++i) {
					meta_hpp::uvalue sub_variant = *variant[i];
					std::string sub_prop_name = prop_name + "_" + std::to_string(i);
					m_property_constructors.at(property_type.first)(sub_prop_name, sub_variant);
					u_setter.invoke(u_instance, i, sub_variant);
				}
				prop.set(u_instance, variant);
			}
			// PROCESS MAPS I KNOW HOW TO DO SO NOW - THATS HOW TO DO THE RIGIDBODY FLAG STUFF
		}
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
	}

	// placed checkbox inside of box that is the same size as int and float boxes - for conformity
	void PropertyUI::constructPropertyBool(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);
		ImGui::SameLine();

		// FramePadding affects size of Checkbox - Checkbox size is calculated from Frampadding
		// also modified for fitting Checkbox within child window 
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.0f, 3.0f));
		ImGuiChildFlags child_flags = ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Borders;

   		// we used child window for this because of the auto-resizing and spacing
		ImGui::BeginChild("CheckBoxFrame", ImVec2(0, 0), child_flags);
		ImGui::Checkbox(("##" + name).c_str(), &instance.as<bool>());
		ImGui::SameLine();
		//ImGui::Text("%s", name.c_str()); // Text inside the frame - cannot decide what to put in yet, so just the name
		ImGui::Text("On");
		ImGui::EndChild();
		
		ImGui::PopStyleVar();
	}

	void PropertyUI::constructPropertyIntegar(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);
		ImGui::SameLine();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		ImGui::InputInt(label.c_str(), &instance.as<int>());
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyFloat(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);
		ImGui::SameLine();
		/*
		float spacing = ImGui::GetContentRegionAvail().x*0.5f - ImGui::CalcTextSize(name.c_str()).x;
		ImGui::SameLine(0, spacing);
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		*/

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		ImGui::InputFloat(label.c_str(), &instance.as<float>());
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyString(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);
		ImGui::SameLine();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::Text("%s", (instance.as<std::string>()).c_str());
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyVec2(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		glm::vec2& vec2 = instance.as<glm::vec2>();
		ImGui::DragFloat2(label.c_str(), glm::value_ptr(vec2), DRAG_SPEED);
		ImGui::PopItemWidth();
	}



	// produces nearly the same result as DragFloat3, except labels are displayed inline
	void PropertyUI::DragFloatInlineLabel(const std::string& label, float *value, float size) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGuiChildFlags child_flags = ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Borders;

		// we used child window for this because of the auto-resizing and spacing
		ImGui::BeginChild(("DragFloatFrame_" + label).c_str(), ImVec2(size, 0), child_flags);
		ImGui::PopStyleVar();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 6.0f);
		ImGui::AlignTextToFramePadding();
			ImGui::Text("%s", (label + ":").c_str()); 
		ImGui::SameLine(0, 6.0f); 
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::DragFloat(("##" + label).c_str(), value, DRAG_SPEED);
			ImGui::PopItemWidth();
		ImGui::EndChild();
	}

	void PropertyUI::constructPropertyVec3(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);
		//ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4.0f*m_res_scale); // Move up to reduce space
		
		float single_space = ImGui::CalcTextSize("#").x * 0.6f;
		float size = ImGui::GetContentRegionAvail().x / 3.0f - single_space;
		glm::vec3& vec3 = instance.as<glm::vec3>();
		ImGuiSliderFlags flags = 0;
		//ImGui::DragFloat3(label.c_str(), glm::value_ptr(vec3), DRAG_SPEED);
		
		ImGui::PushID(("##" + name).c_str());
		ImGui::BeginGroup();
			DragFloatInlineLabel("X", &vec3.x, size);
		ImGui::SameLine(0, single_space);
			DragFloatInlineLabel("Y", &vec3.y, size);
		ImGui::SameLine(0, single_space);
			DragFloatInlineLabel("Z", &vec3.z, size);
		ImGui::EndGroup();
		ImGui::PopID();
	}

	void PropertyUI::constructPropertyVec4(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		glm::vec4 vec4 = instance.as<glm::vec4>();
		ImGui::DragFloat4(label.c_str(), glm::value_ptr(vec4), DRAG_SPEED);
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyColor3(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		Color3& color3 = instance.as<Color3>();
		ImGui::ColorEdit3(label.c_str(), color3.data());
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyColor4(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		std::string label = "##" + name;
		Color4& color4 = instance.as<Color4>();
		ImGui::ColorEdit3(label.c_str(), color4.data());
		ImGui::PopItemWidth();
	}

	void PropertyUI::constructPropertyAsset(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);
		ImGui::SameLine();

		// asset preview image
		const ImVec2 icon_size(20.0f*m_res_scale, 20.0f*m_res_scale);
		ImGui::Image((ImTextureID)m_dummy_image->tex_id, icon_size);
		ImGui::SameLine(0, 8.0f);

		// asset find combo box
		const char* asset_names[] = { "asset_0", "asset_1", "asset_2", "asset_3" };
		int selected_index = 2;
		const char* preview_value = asset_names[selected_index];
		ImGuiComboFlags combo_flags = 0;
		
		ImGui::PushItemWidth(std::max(ImGui::GetContentRegionAvail().x, ImGui::CalcTextSize(preview_value).x * 1.5f));
		if (ImGui::BeginCombo("##select_asset", preview_value, combo_flags)) {
			for (int i = 0; i < IM_ARRAYSIZE(asset_names); ++i) {
				const bool is_selected = selected_index == i;
				if (is_selected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::Selectable(asset_names[i], is_selected))
					selected_index = i;
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}

	// USE meta.hpp ENUM SYSTEM IN ORDER TO PRINT!!!
	void PropertyUI::constructPropertyEMotion(const std::string& name, meta_hpp::uvalue& instance) {
		addPropertyNameText(name);
		ImGui::SameLine();

		auto motion_types = meta_hpp::resolve_type<EMotionType>().get_evalues();
		const meta_hpp::evalue selected_value = meta_hpp::resolve_type<EMotionType>().value_to_evalue(instance);
		
		const char* preview_value = selected_value.get_name().c_str();
		ImGuiComboFlags combo_flags = 0;
	
		ImGui::PushItemWidth(std::max(ImGui::GetContentRegionAvail().x, ImGui::CalcTextSize(preview_value).x * 1.5f));
		if (ImGui::BeginCombo("##select_motion_type", preview_value, combo_flags)) {
			for (const meta_hpp::evalue& motion_value : motion_types) {
				const bool is_selected = (selected_value != motion_value); 
				if (ImGui::Selectable(motion_value.get_name().c_str(), is_selected))
					instance.as<EMotionType>() = motion_value.get_value().as<EMotionType>();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}

	/*
	I wanted to achieve:
	- the ability for the user to highlight and copy the text
	- a way to cut off the text if the window space is not accomodating, in order to maintain the ratio of 50/50 between the name and the value
	- I did NOT want the text to overlap or be overlapped

	InputText allowed me to do this
	*/
	void PropertyUI::addPropertyNameText(const std::string& name) {
		// create storage buffer for the property name
		char str[name.length() + 1]; 	// Ensure enough space
		std::strcpy(str, name.c_str());
		std::string property_label = "##property_" + name;	// unique name for label 
		
		// removes background coloring and border
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));    // Transparent background
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));     // No border
		
		// actual text
		ImGui::InputText(property_label.c_str(), str, IM_ARRAYSIZE(str), ImGuiInputTextFlags_ReadOnly);
		
		// Reset background & border color
		ImGui::PopItemWidth();	
		ImGui::PopStyleColor(2);  
	}



	void PropertyUI::componentManager(const meta_hpp::uvalue& selected_entity) {
		auto p_entity = selected_entity.try_as<Entity*>();
		float button_height = 30.0f * m_res_scale;
		//ImGui::GetStyle().ScrollbarSize = 10.f*m_res_scale;

		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		// window that pops up uses it's own viewport and not the main viewport
		//ImGui::SetNextWindowClass(&noAutoMerge);
		if (ImGui::Begin("Manage Components", &showing_component_manager, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking)) {
			//ImGui::Text("Importing gltf:");
			for (auto iter = m_property_components.begin(); iter != m_property_components.end(); ++iter) {
				const bool has_component = p_entity->hasComponent_str(iter->first);
				/*
				if (ImGui::Checkbox(("##" + iter->first).c_str(), &has_component)){
					has_component ? p_entity->removeComponent(p_entity->getComponent<Component>(iter->first)) : p_entity->addComponent(iter->second());
				}
				ImGui::SameLine();
				*/
				if (ImGui::Selectable(iter->first.c_str(), has_component, ImGuiSelectableFlags_NoAutoClosePopups)) {
					has_component ? p_entity->removeComponent(p_entity->getComponent<Component>(iter->first)) : p_entity->addComponent(iter->second());
				}
			}

			/*
			ImGuiViewport *viewport = ImGui::FindViewportByID(ImGui::FindWindowByName("Manage Components")->ID);
			if (ImGui::BeginViewportSideBar("bottom_menubar", viewport, ImGuiDir_Down, button_height, ImGuiWindowFlags_None)) {
				if (ImGui::Button("Close", ImVec2(120.0f*m_res_scale, 0)) ) {
					showing_component_manager = false;
				}
				ImGui::End();
			}
			*/
			if (ImGui::BeginMenuBar()) {
				if (ImGui::Button("Close", ImVec2(120.0f*m_res_scale, 0)) )
					showing_component_manager = false;

				ImGui::EndMenuBar();
			}

			ImGui::End();
		}

	}

	EPropertyType PropertyUI::getPropertyType(const meta_hpp::uvalue& type) {
		const std::string& type_name = type.as<std::string>();

		EPropertyContainerType container_type = EPropertyContainerType::Mono;
		if (type_name.find("std::vector") != std::string::npos)
			container_type = EPropertyContainerType::Array;
		else if (type_name.find("std::map") != std::string::npos)
			container_type = EPropertyContainerType::Map;

		EPropertyValueType value_type = EPropertyValueType::Bool;
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
		else if (type_name.find("float") != std::string::npos)
			value_type = EPropertyValueType::Float;
		else if (type_name.find("std::string") != std::string::npos)
			value_type = EPropertyValueType::String;
		else if (type_name.find("EMotionType") != std::string::npos)
			value_type = EPropertyValueType::EMotion;
		
		return std::make_pair(value_type, container_type);
	}

}