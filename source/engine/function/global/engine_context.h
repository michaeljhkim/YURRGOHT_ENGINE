#pragma once

#include <memory>
#include <vector>
#include <array>
#include <map>

namespace Yurrgoht
{
    class EngineContext {
        public:
            void init();
            void destroy();

			const auto& timerManager() { return m_timer_manager; }
			const auto& fileSystem() { return m_file_system; }
			const auto& logSystem() { return m_log_system; }
            const auto& configManager() { return m_config_manager; }
            const auto& eventSystem() { return m_event_system; }
            const auto& windowSystem() { return m_window_system; }
            const auto& shaderManager() { return m_shader_manager; }
            const auto& assetManager() { return m_asset_manager; }
            const auto& sceneManager() { return m_scene_manager; }
			const auto& physicsSystem() { return m_physics_system; }
			const auto& renderSystem() { return m_render_system; }
            const auto& debugDrawSystem() { return m_debug_draw_system; }

            void setDeltaTime(float delta_time) { m_delta_time = delta_time; }
            float getDeltaTime() { return m_delta_time; }

            // runtime modes
            bool isEditor();
            bool isApplication() { return !isEditor(); }

            bool isEditing();
            bool isPlaying();
            bool isPausing();
            bool isSimulating() { return isPlaying() || isPausing(); }

            // render settings
            std::map<std::string, bool> m_render_settings = {
                { "anti-aliasing", false },
                { "bounding boxes", false }, 
                { "collision", false }, 
                { "grid", true }, 
                { "static meshes", true }, 
                { "skeletal meshes", true }, 
                { "translucency", true }
            };

        private:
            std::shared_ptr<class TimerManager> m_timer_manager;
			std::shared_ptr<class FileSystem> m_file_system;
			std::shared_ptr<class LogSystem> m_log_system;
            std::shared_ptr<class ConfigManager> m_config_manager;
            std::shared_ptr<class EventSystem> m_event_system;
			std::shared_ptr<class WindowSystem> m_window_system;
			std::shared_ptr<class ShaderManager> m_shader_manager;
			std::shared_ptr<class AssetManager> m_asset_manager;
			std::shared_ptr<class SceneManager> m_scene_manager;
			std::shared_ptr<class PhysicsSystem> m_physics_system;
			std::shared_ptr<class RenderSystem> m_render_system;
			std::shared_ptr<class DebugDrawManager> m_debug_draw_system;

            float m_delta_time = 0.0f;
    };

    extern EngineContext g_engine;
}
