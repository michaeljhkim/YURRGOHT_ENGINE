#pragma once

#include <any>
#include <typeinfo>
#include <cxxabi.h>
#include "editor/base/editor_ui.h"
#include "engine/function/framework/component/component.h"

namespace Yurrgoht {
	
	enum class EPropertyComponentType {
		AnimationComponent,
		AnimatorComponent, 
		BoxColliderComponent, 
		CameraComponent,
		CapsuleColliderComponent,
		ColliderComponent,
		CylinderColliderComponent,
		DirectionalLightComponent,
		LightComponent,
		MeshColliderComponent,
		PointLightComponent,
		RigidbodyComponent,
		SkeletalMeshComponent,
		SkyLightComponent,
		SphereColliderComponent,
		SpotLightComponent,
		StaticMeshComponent,
		TransformComponent
	};

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

		// managing components
		template <typename T>
		std::shared_ptr<T> CreateComponent() { return std::make_shared<T>(); }
		void componentManager(const meta_hpp::uvalue& selected_entity);
		bool showing_component_manager = false;
		ImGuiWindowClass noAutoMerge;

		char m_plus_buf[128];
		char m_trash_buf[128];

		std::weak_ptr<class Entity> m_selected_entity;
		std::shared_ptr<ImGuiImage> m_dummy_image;

		std::map<EPropertyValueType, std::function<void(const std::string&, meta_hpp::uvalue&)>> m_property_constructors;
		std::map<std::string, std::function<std::shared_ptr<Component>()>> m_property_components;
	};
}