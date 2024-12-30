#include "world_manager.h"
#include "engine/core/base/macro.h"
#include "engine/core/config/config_manager.h"
#include "engine/resource/asset/asset_manager.h"
#include "engine/resource/asset/skeletal_mesh.h"
#include "engine/resource/asset/texture_2d.h"
#include "engine/resource/asset/texture_cube.h"
#include "engine/resource/asset/animation.h"
#include "engine/function/framework/component/transform_component.h"
#include "engine/function/framework/component/camera_component.h"
#include "engine/function/framework/component/directional_light_component.h"
#include "engine/function/framework/component/sky_light_component.h"
#include "engine/function/framework/component/spot_light_component.h"

namespace Yurrgoht {

	void WorldManager::init() {
		URL default_world_url = g_engine.configManager()->getDefaultWorldUrl();
		const auto& fs = g_engine.fileSystem();
		std::string cache_dir = fs->getCacheDir();
		m_pie_world_url = fs->relative(fs->combine(cache_dir, std::string("pie.world")));
		m_world_mode = g_engine.isEditor() ? EWorldMode::Edit : EWorldMode::Play;

		loadWorld(default_world_url);
		LOG_INFO("SUCCESS");
	}

	/*
	void WorldManager::CreateNewDefaultWorld() {
		URL new_default_world_url = "asset/world/default.world";
		createWorld(new_default_world_url, new_default_world_url);
		m_current_world = std::make_unique<World>();
		m_current_world_url = new_default_world_url;
		saveAsWorld(new_default_world_url);
	}
	*/

	void WorldManager::destroy() {
		m_current_world.reset();
	}

	void WorldManager::tick(float delta_time) {
		// open world async
		if (!m_open_world_url.empty()) {
			loadWorld(m_open_world_url);
			m_open_world_url.clear();
		}

		// create world async
		if (!m_template_url.empty()) {
			loadWorld(m_template_url);
			saveAsWorld(m_save_as_url);
			loadWorld(m_save_as_url);

			m_template_url.clear();
			m_save_as_url.clear();
		}

		m_current_world->tick(delta_time);
	}

	void WorldManager::openWorld(const URL& url) {
		m_open_world_url = url;
		LOG_INFO("open world: {}", m_open_world_url.str());
	}

	void WorldManager::createWorld(const URL& template_url, const URL& save_as_url) {
        std::cout << "Creating New World" << std::endl;
		m_template_url = template_url;
		m_save_as_url = save_as_url;
		LOG_INFO("create world: {}", m_template_url.str());
	}

	bool WorldManager::saveWorld() {
		g_engine.assetManager()->serializeAsset(m_current_world, m_current_world_url);
		LOG_INFO("save world: {}", m_current_world_url.str());
		return true;
	}

	bool WorldManager::saveAsWorld(const URL& url) {
        std::cout << "Saving World As: " << url.getAbsolute() << std::endl;
		g_engine.assetManager()->serializeAsset(m_current_world, url);
		return true;
	}

	std::string WorldManager::getCurrentWorldName() {
		return g_engine.fileSystem()->basename(m_current_world_url.str());
	}

	std::weak_ptr<CameraComponent> WorldManager::getCameraComponent() {
		return m_current_world->getCameraEntity().lock()->getComponent(CameraComponent);
	}

	void WorldManager::setWorldMode(EWorldMode world_mode) {
		if (m_world_mode == world_mode) {
			return;
		}

		const auto& fs = g_engine.fileSystem();
		switch (world_mode) {
			case EWorldMode::Edit: {
				// load world from cache
				openWorld(m_pie_world_url);
				break;
			}
			case EWorldMode::Play: {
				if (m_world_mode == EWorldMode::Edit) {
					// save world to cache
					saveAsWorld(m_pie_world_url);

					// call beginPlay of all entities
					m_current_world->beginPlay();
				}
				break;
			}
			default:
				break;
		}
		m_world_mode = world_mode;
	}

	bool WorldManager::loadWorld(const URL& url) {
		LOG_INFO("LOADING: {}", url.str());
		
		if (m_current_world) {
			m_current_world.reset();
		}
		m_current_world = g_engine.assetManager()->loadAsset<World>(url);
		if (url != m_pie_world_url) {
			m_current_world_url = url;
		}

		LOG_INFO("SUCCESS");
		return true;
	}

}