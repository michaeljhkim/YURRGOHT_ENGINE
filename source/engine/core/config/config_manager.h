#pragma once

#include <string>
#include <yaml-cpp/yaml.h>

namespace Yurrgoht
{
	class ConfigManager
	{
	public:
		void init();
		void destroy() {}

		int getWindowWidth();
		int getWindowHeight();
		bool isFullscreen();

		std::string getDefaultSceneUrl();
		std::string getEditorLayout();
		bool getSaveLayout();
		
		bool isEditor();

	private:
		YAML::Node m_config_node;
	};
}