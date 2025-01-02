#include "scene_manager.h"
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

	void SceneManager::init() {
		URL default_scene_url = g_engine.configManager()->getDefaultSceneUrl();
		const auto& fs = g_engine.fileSystem();
		std::string cache_dir = fs->getCacheDir();
		m_pie_scene_url = fs->relative(fs->combine(cache_dir, std::string("pie.scene")));
		m_scene_mode = g_engine.isEditor() ? ESceneMode::Edit : ESceneMode::Play;

		loadScene(default_scene_url);
		LOG_INFO("SUCCESS");
	}

	/*
	void SceneManager::CreateNewDefaultScene() {
		URL new_default_scene_url = "asset/scene/default.scene";
		createScene(new_default_scene_url, new_default_scene_url);
		m_current_scene = std::make_unique<Scene>();
		m_current_scene_url = new_default_scene_url;
		saveAsScene(new_default_scene_url);
	}
	*/

	void SceneManager::destroy() {
		m_current_scene.reset();
	}

	void SceneManager::tick(float delta_time) {
		// open scene async
		if (!m_open_scene_url.empty()) {
			loadScene(m_open_scene_url);
			m_open_scene_url.clear();
		}

		// create scene async
		if (!m_template_url.empty()) {
			loadScene(m_template_url);
			saveAsScene(m_save_as_url);
			loadScene(m_save_as_url);

			m_template_url.clear();
			m_save_as_url.clear();
		}

		m_current_scene->tick(delta_time);
	}

	void SceneManager::openScene(const URL& url) {
		m_open_scene_url = url;
		LOG_INFO("open scene: {}", m_open_scene_url.str());
	}

	void SceneManager::createScene(const URL& template_url, const URL& save_as_url) {
        std::cout << "Creating New Scene" << std::endl;
		m_template_url = template_url;
		m_save_as_url = save_as_url;
		LOG_INFO("create scene: {}", m_template_url.str());
	}

	bool SceneManager::saveScene() {
		g_engine.assetManager()->serializeAsset(m_current_scene, m_current_scene_url);
		LOG_INFO("save scene: {}", m_current_scene_url.str());
		return true;
	}

	bool SceneManager::saveAsScene(const URL& url) {
        std::cout << "Saving Scene As: " << url.getAbsolute() << std::endl;
		g_engine.assetManager()->serializeAsset(m_current_scene, url);
		return true;
	}

	std::string SceneManager::getCurrentSceneName() {
		return g_engine.fileSystem()->basename(m_current_scene_url.str());
	}

	std::weak_ptr<CameraComponent> SceneManager::getCameraComponent() {
		return m_current_scene->getCameraEntity().lock()->getComponent(CameraComponent);
	}

	void SceneManager::setSceneMode(ESceneMode scene_mode) {
		if (m_scene_mode == scene_mode) {
			return;
		}

		const auto& fs = g_engine.fileSystem();
		switch (scene_mode) {
			case ESceneMode::Edit: {
				// load scene from cache
				openScene(m_pie_scene_url);
				break;
			}
			case ESceneMode::Play: {
				if (m_scene_mode == ESceneMode::Edit) {
					// save scene to cache
					saveAsScene(m_pie_scene_url);

					// call beginPlay of all entities
					m_current_scene->beginPlay();
				}
				break;
			}
			default:
				break;
		}
		m_scene_mode = scene_mode;
	}

	bool SceneManager::loadScene(const URL& url) {
		LOG_INFO("LOADING: {}", url.str());
		
		if (m_current_scene) {
			m_current_scene.reset();
		}
		m_current_scene = g_engine.assetManager()->loadAsset<Scene>(url);
		if (url != m_pie_scene_url) {
			m_current_scene_url = url;
		}

		LOG_INFO("SUCCESS");
		return true;
	}

}