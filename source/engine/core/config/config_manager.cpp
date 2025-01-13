#include "config_manager.h"
#include "engine/core/base/macro.h"

namespace Yurrgoht {

	void ConfigManager::init() {
		m_config_node = YAML::LoadFile(TO_ABSOLUTE("config/engine.yaml"));
	}

	int ConfigManager::getWindowWidth() {
		return m_config_node["window"]["width"].as<int>();
	}

	int ConfigManager::getWindowHeight() {
		return m_config_node["window"]["height"].as<int>();
	}

	bool ConfigManager::isFullscreen() {
		return m_config_node["window"]["fullscreen"].as<bool>();
	}

	std::string ConfigManager::getDefaultSceneUrl() {
		return m_config_node["default_scene_url"].as<std::string>();
	}

	std::string ConfigManager::getEditorLayout() {
		return m_config_node["editor_layout"].as<std::string>();
	}

	bool ConfigManager::getSaveLayout() {
		return m_config_node["save_layout"].as<bool>();
	}

	bool ConfigManager::isEditor() {
		return m_config_node["is_editor"].as<bool>();
	}

}