#pragma once

#include "scene.h"

namespace Yurrgoht
{
	class SceneManager
	{
	public:
		void init();
		void destroy();
		void tick(float delta_time);

		void openScene(const URL& url);
		void CreateNewDefaultScene();
		void createScene(const URL& template_url, const URL& save_as_url);
		bool saveScene();
		bool saveAsScene(const URL& url);

		const std::shared_ptr<Scene>& getCurrentScene() { return m_current_scene; }
		std::string getCurrentSceneName();

		std::weak_ptr<class CameraComponent> getCameraComponent();

		void setSceneMode(ESceneMode scene_mode);
		ESceneMode getSceneMode() { return m_scene_mode; }

	private:
		bool loadScene(const URL& url);

		std::shared_ptr<Scene> m_current_scene;

		URL m_open_scene_url, m_template_url, m_save_as_url;
		URL m_current_scene_url, m_pie_scene_url;

		ESceneMode m_scene_mode;
	};
}