#include "engine_context.h"
#include "engine/platform/timer/timer.h"
#include "engine/platform/file/file_system.h"
#include "engine/core/log/log_system.h"
#include "engine/core/config/config_manager.h"
#include "engine/core/event/event_system.h"
#include "engine/function/render/window_system.h"
#include "engine/function/framework/scene/scene_manager.h"
#include "engine/function/physics/physics_system.h"
#include "engine/function/render/render_system.h"
#include "engine/function/render/debug_draw_manager.h"
#include "engine/resource/shader/shader_manager.h"
#include "engine/resource/asset/asset_manager.h"
#include "engine/core/vulkan/vulkan_rhi.h"

#include "engine/function/scripting/script_manager.h"


namespace Yurrgoht {
    EngineContext g_engine;

    void EngineContext::init() {
        std::cout << "EngineContext::init (no LOG_INFO)" << std::endl;

		m_timer_manager = std::make_shared<TimerManager>();
        m_timer_manager->init();

		m_file_system = std::make_shared<FileSystem>();
        m_file_system->init();

		m_log_system = std::make_shared<LogSystem>();
		m_log_system->init();

        m_config_manager = std::make_shared<ConfigManager>();
        m_config_manager->init();

		m_event_system = std::make_shared<EventSystem>();
        m_event_system->init();

        m_window_system = std::make_shared<WindowSystem>();
        m_window_system->init();

        VulkanRHI::get().init();

		m_shader_manager = std::make_shared<ShaderManager>();
        m_shader_manager->init();

		m_asset_manager = std::make_shared<AssetManager>();
		m_asset_manager->init();

        m_scene_manager = std::make_shared<SceneManager>();
        m_scene_manager->init();

		m_physics_system = std::make_shared<PhysicsSystem>();
        m_physics_system->init();

        m_render_system = std::make_shared<RenderSystem>();
        m_render_system->init();

		m_debug_draw_system = std::make_shared<DebugDrawManager>();
        m_debug_draw_system->init();

        //JUST A TEST
		m_script_system = std::make_shared<ScriptManager>();
        //m_script_system->init();
        m_script_system->run_app_example();

		//LOG_INFO("EngineContext::init {}", "TimerManager | FileSystem | LogSystem - INITIALIZED");
        // NOTE: Logging cannot start before this point as LogSystem is not initialized
		LOG_INFO("SUCCESS");
    }   

    void EngineContext::destroy() {
		// wait all gpu operations done
        VulkanRHI::get().waitDeviceIdle();

        // destroy with reverse initialize order
        //m_script_system->destroy();
        m_debug_draw_system->destroy();
        m_render_system->destroy();
        m_physics_system->destroy();
        m_scene_manager->destroy();
		m_asset_manager->destroy();
        m_shader_manager->destroy();
        VulkanRHI::get().destroy();
		m_window_system->destroy();
        m_event_system->destroy();
        m_config_manager->destroy();
        m_log_system->destroy();
		m_file_system->destroy();
        m_timer_manager->destroy();
	}

	bool EngineContext::isEditor() {
        return m_config_manager->isEditor();
	}

	bool EngineContext::isEditing() {
        return m_scene_manager->getSceneMode() == ESceneMode::Edit;
	}

	bool EngineContext::isPlaying() {
        return m_scene_manager->getSceneMode() == ESceneMode::Play;
	}

	bool EngineContext::isPausing() {
        return m_scene_manager->getSceneMode() == ESceneMode::Pause;
	}

}