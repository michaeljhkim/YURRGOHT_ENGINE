#include "application_entity.h"
#include "engine/core/base/macro.h"
#include "engine/platform/string/string_util.h"
#include "engine/core/math/math_util.h"
#include "engine/function/framework/world/world_manager.h"
#include "engine/resource/asset/asset_manager.h"
#include "engine/function/framework/component/static_mesh_component.h"
#include "engine/function/framework/component/transform_component.h"
#include "engine/function/framework/component/rigidbody_component.h"
#include "engine/function/framework/component/box_collider_component.h"
#include "engine/function/framework/component/sphere_collider_component.h"
#include "engine/function/framework/component/cylinder_collider_component.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::ApplicationEntity>("ApplicationEntity") .constructor<>()()
	.property("spawn_num", &Yurrgoht::ApplicationEntity::m_spawn_num);
}

CEREAL_REGISTER_TYPE(Yurrgoht::ApplicationEntity)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Entity, Yurrgoht::ApplicationEntity)

namespace Yurrgoht
{

	ApplicationEntity::ApplicationEntity()
	{
		m_spawn_num = 100;
		setTickEnabled(true);
	}

	void ApplicationEntity::beginPlay()
	{
		Entity::beginPlay();

		// add floor rigidbody
		const auto& world = g_engine.worldManager()->getCurrentWorld();
		std::shared_ptr<Entity> floor_entity = world->getEntity("sm_floor").lock();

		std::shared_ptr<RigidbodyComponent> rigidbody_component = std::make_shared<RigidbodyComponent>();
		rigidbody_component->m_motion_type = EMotionType::Static;
		floor_entity->addComponent(rigidbody_component);

		std::shared_ptr<BoxColliderComponent> box_collider_component = std::make_shared<BoxColliderComponent>();
		box_collider_component->m_position = glm::vec3(0.0f, -0.2f, 0.0f);
		box_collider_component->m_size = glm::vec3(5.0f, 0.2f, 5.0f);
		floor_entity->addComponent(box_collider_component);

		// add primitive rigidbodies
		std::vector<std::string> primitive_types = {
			"cube", "sphere", "cylinder"
		};

		for (int i = 0; i < m_spawn_num; ++i)
		{
			uint32_t primitive_type_index = MathUtil::randomInteger(0, primitive_types.size() - 1);
			const std::string& primitive_type = primitive_types[primitive_type_index];
			std::string url = StringUtil::format("asset/engine/mesh/primitive/sm_%s.sm", primitive_type.c_str());

			// create primitive entity
			const auto& entity = world->createEntity(StringUtil::format("%s_%d", primitive_type.c_str(), i));

			// add static mesh component
			std::shared_ptr<StaticMeshComponent> static_mesh_component = std::make_shared<StaticMeshComponent>();
			std::shared_ptr<StaticMesh> static_mesh = g_engine.assetManager()->loadAsset<StaticMesh>(url);
			static_mesh_component->setStaticMesh(static_mesh);
			entity->addComponent(static_mesh_component);

			// set transform component
			auto transform_component = entity->getComponent(TransformComponent);
			transform_component->m_position = MathUtil::randomPointInBoundingBox(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(20.0f, 20.0f, 20.0f));
			transform_component->m_rotation = MathUtil::randomRotation();
			transform_component->m_scale = glm::vec3(MathUtil::randomFloat(0.5f, 1.5f));

			// add rigidbody component
			std::shared_ptr<RigidbodyComponent> rigidbody_component = std::make_shared<RigidbodyComponent>();
			entity->addComponent(rigidbody_component);

			// add collider component
			std::shared_ptr<ColliderComponent> collider_component;
			if (primitive_type_index == 0)
			{
				collider_component = std::make_shared<BoxColliderComponent>();
			}
			else if (primitive_type_index == 1)
			{
				collider_component = std::make_shared<SphereColliderComponent>();
			}
			else if (primitive_type_index == 2)
			{
				collider_component = std::make_shared<CylinderColliderComponent>();
			}
			entity->addComponent(collider_component);
		}
	}

	void ApplicationEntity::tick(float delta_time)
	{
		Entity::tick(delta_time);

	}

	void ApplicationEntity::endPlay()
	{
		Entity::endPlay();

	}

}
