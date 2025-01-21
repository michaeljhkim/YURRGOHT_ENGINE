#include "scene.h"
#include "engine/core/base/macro.h"
#include "engine/function/framework/component/camera_component.h"
#include "engine/function/framework/component/transform_component.h"
#include "engine/function/framework/scene/scene_manager.h"
#include <fstream>

CEREAL_REGISTER_TYPE(Yurrgoht::Scene)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Asset, Yurrgoht::Scene)

namespace Yurrgoht {

	Scene::Scene() {
		auto derived_entity_types = global_reflection_scope.get_typedef("Entity").as_class().get_metadata().equal_range("derived_classes");
		for (auto it = derived_entity_types.first; it != derived_entity_types.second; ++it) {
			m_entity_class_names.push_back(it->second.as<std::string>());
		}
		
		LOG_INFO("SUCCESS");
	}

	Scene::~Scene() {
		m_camera_entity.reset();
		for (auto iter : m_entities) {
			iter.second->endPlay();
			iter.second.reset();
		}
		m_entities.clear();
	}

	void Scene::inflate() {
		for (const auto& iter : m_entities) {
			const auto& entity = iter.second;
			LOG_INFO("LOADING: {}" , entity->getName());
			entity->m_scene = weak_from_this();
			entity->inflate();
			if (g_engine.isSimulating()) {
				entity->beginPlay();
			}

			// get camera entity
			if (entity->hasComponent(CameraComponent)) {
				m_camera_entity = entity;
			}

			// update next entity id
			m_next_entity_id = std::max(m_next_entity_id, entity->getID() + 1);
		}
	}

	void Scene::beginPlay() {
		for (const auto& iter : m_entities)
			iter.second->beginPlay();
	}

	void Scene::tick(float delta_time) {
		for (const auto& iter : m_entities) {
			auto entity = iter.second;

			// update entity's own and children transforms
			entity->updateTransforms();

			// tick entity
			if (entity == m_camera_entity.lock() || g_engine.isPlaying() || is_stepping)
				entity->tickable(delta_time);
		}

		if (is_stepping)
			is_stepping = false;
	}

	void Scene::step() {
		is_stepping = true;
	}

	std::weak_ptr<Entity> Scene::getEntity(uint32_t id) {
		const auto& iter = m_entities.find(id);
		if (iter != m_entities.end()) {
			return iter->second;
		}
		return {};
	}

	std::weak_ptr<Entity> Scene::getEntity(const std::string& name) {
		for (const auto& entity : m_entities) {
			if (name == entity.second->getName()) {
				return entity.second;
			}
		}
		return {};
	}

	const std::shared_ptr<Entity>& Scene::createEntity(const std::string& name) {
		std::shared_ptr<Entity> entity;
		if (std::find(m_entity_class_names.begin(), m_entity_class_names.end(), name) == m_entity_class_names.end()) {
			entity = std::make_shared<Entity>();
		}
		else {
			const meta_hpp::class_type type = global_reflection_scope.get_typedef(name).as_class();
			meta_hpp::uvalue variant = type.create();
			entity = variant.as<std::shared_ptr<Entity>>();
		}

		entity->m_id = m_next_entity_id++;
		entity->m_name = name;
		entity->m_scene = weak_from_this();

		// every entity has transform component
		entity->addComponent(std::make_shared<TransformComponent>());

		if (g_engine.isSimulating())
			entity->beginPlay();

		m_entities[entity->m_id] = entity;
		return m_entities[entity->m_id];
	}

	bool Scene::removeEntity(uint32_t id) {
		if (g_engine.isSimulating()) {
			m_entities[id]->endPlay();
		}
		return m_entities.erase(id) > 0;
	}

}