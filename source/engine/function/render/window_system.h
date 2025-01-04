#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <imgui/backends/imgui_impl_sdl2.h>

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

		bool isMouseButtonDown(int button);
		bool getFocus() const { return m_focus; }
		void setFocus(bool focus);

		void toggleFullscreen();

	private:
		SDL_Window* m_window;
		int m_mouse_pos_x;
		int m_mouse_pos_y;
		bool m_focus;
		bool m_fullscreen;
		bool m_should_close = false;

		int m_windowed_width, m_windowed_height;
		int m_windowed_pos_x, m_windowed_pos_y;
	};
}