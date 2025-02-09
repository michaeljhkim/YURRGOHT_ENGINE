#pragma once

#include "editor/base/editor_ui.h"
#include <any>
#include <typeinfo>
#include <cxxabi.h>

namespace Yurrgoht {

	enum class EPropertyValueType {
		Bool, Integar, Float, String, Vec2, Vec3, Vec4, Color3, Color4, Asset
	};
	enum class EPropertyContainerType {
		Mono, Array, Map
	};
	using EPropertyType = std::pair<EPropertyValueType, EPropertyContainerType>;

	class PropertyUI : public EditorUI {
	public:
		virtual void init() override;
		virtual void construct() override;

	private:
		void onSelectEntity(const std::shared_ptr<class Event>& event);
		void constructEntity(const meta_hpp::uvalue& instance);

		void constructPropertyBool(const std::string& name, meta_hpp::uvalue& instance);
		void constructPropertyIntegar(const std::string& name, meta_hpp::uvalue& instance);
		void constructPropertyFloat(const std::string& name, meta_hpp::uvalue& instance);
		void constructPropertyString(const std::string& name, meta_hpp::uvalue& instance);
		void constructPropertyVec2(const std::string& name, meta_hpp::uvalue& instance);
		void constructPropertyVec3(const std::string& name, meta_hpp::uvalue& instance);
		void constructPropertyVec4(const std::string& name, meta_hpp::uvalue& instance);
		void constructPropertyColor3(const std::string& name, meta_hpp::uvalue& instance);
		void constructPropertyColor4(const std::string& name, meta_hpp::uvalue& instance);
		void constructPropertyAsset(const std::string& name, meta_hpp::uvalue& instance);

		void DragFloatInlineLabel(const std::string& name, float *value, float size);
		void addPropertyNameText(const std::string& name);
		EPropertyType getPropertyType(const meta_hpp::uvalue& type);

		void addComponent();
		char m_plus_buf[128];
		char m_trash_buf[128];

		std::weak_ptr<class Entity> m_selected_entity;
		std::shared_ptr<ImGuiImage> m_dummy_image;

		std::map<EPropertyValueType, std::function<void(const std::string&, meta_hpp::uvalue&)>> m_property_constructors;
	};
}