#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3_image/SDL_image.h>
#include <imgui/backends/imgui_impl_sdl3.h>

#include <string>
#include <functional>
#include <vector>
#include <memory>


namespace Yurrgoht {
	class WindowSystem {
	public:
		void init();
		void destroy();

		void pollEvents();
		bool shouldClose();
		void setTitle(const std::string& title);
		SDL_Window* getWindow() { return m_window; }
		void getWindowSize(int& width, int& height);
		void getScreenSize(int& width, int& height);
		void getMousePos(int& x, int& y);
		float getResolutionScale() { return m_scale; };

		bool isMouseButtonDown(int button);
		bool getFocus() const { return m_focus; }
		void setFocus(bool focus);

		void toggleFullscreen();

	private:
		SDL_Window* m_window;
		double m_mouse_pos_x;
		double m_mouse_pos_y;
		bool m_focus;
		bool m_fullscreen;
		bool m_should_close = false;
		float m_scale = 1.0f;
		std::vector<std::string> dropped_file_paths;

		int m_windowed_width, m_windowed_height;
		int m_windowed_pos_x, m_windowed_pos_y;
	};
}