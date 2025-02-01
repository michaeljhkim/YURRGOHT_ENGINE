#include "window_system.h"
#include "engine/core/base/macro.h"
#include "engine/core/config/config_manager.h"
#include "engine/core/event/event_system.h"

#include <tinygltf/stb_image.h>

namespace Yurrgoht {

	void WindowSystem::init() {
		// Initialize SDL
		if (!SDL_Init(SDL_INIT_VIDEO)) {
        	std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
			LOG_FATAL("failed to initialize sdl3");
			return;
		}

		m_focus = false;
		m_fullscreen = g_engine.configManager()->isFullscreen();
		const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay());
		// If the height of monitor is greater than 1080p, we scale up
		// If the hieght is less than 1080p, we scale down
		// Very simple solution for now, will make more flexible in the future
		// Remember to get 4k fonts in the future
		m_scale = mode->h/1080.0f;

		// create sdl3 window
		bool is_packaged_fullscreen = m_fullscreen && g_engine.isApplication();
		int width = is_packaged_fullscreen ? mode->w : g_engine.configManager()->getWindowWidth();
		int height = is_packaged_fullscreen ? mode->h: g_engine.configManager()->getWindowHeight();
		m_window = SDL_CreateWindow(APP_NAME,    /* Title of the SDL window */
                    width, height,     /* Width and Height of the window in pixels */
					SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE); 
		if (!m_window) {
			LOG_FATAL("failed to create sdl3 window");
			SDL_Quit(); 
			return;
		}

		// set fullscreen if needed
		if (is_packaged_fullscreen) {
			m_windowed_width = g_engine.configManager()->getWindowWidth();
			m_windowed_height = g_engine.configManager()->getWindowHeight();
			m_windowed_pos_x = (mode->w - m_windowed_width) * 0.5f;
			m_windowed_pos_y = (mode->h - m_windowed_height) * 0.5f;
			SDL_SetWindowFullscreenMode(m_window, mode);
		}

		// Enable v-sync
		SDL_GL_SetSwapInterval(1);  // 1 enables v-sync

		// Disable cursor
		//SDL_ShowCursor(SDL_DISABLE);
		//SDL_WarpMouseInWindow(m_window, 0, 0);
		SDL_ShowCursor();
		SDL_SetWindowRelativeMouseMode(m_window, false);

		// Enable raw mouse motion (without hiding the cursor)
		/*
		if (SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE)) {
			SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");
		} else {
			SDL_Log("Raw mouse motion not forced to warp mode, but will still work.");
		}
		*/

		// Set window icon
		SDL_Surface* iconSurface = IMG_Load(TO_ABSOLUTE("asset/engine/texture/icon/yurrgoht_small.png").c_str());
		if (iconSurface != nullptr) {
			SDL_SetWindowIcon(m_window, iconSurface);
			SDL_DestroySurface(iconSurface);  // Free immediately after use
		} else {
			SDL_Log("Failed to load icon: %s", SDL_GetError());  // Log error if loading fails
		}
	}

	void WindowSystem::destroy() {
		SDL_DestroyWindow(m_window);
		SDL_Quit(); 
	}

	void WindowSystem::pollEvents() {
		SDL_Event event;
		int action;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			switch (event.type) {
				case SDL_EVENT_QUIT: {
					m_should_close = true;
					break;
				}
				case SDL_EVENT_KEY_UP:
				case SDL_EVENT_KEY_DOWN: {
					action = (event.type == SDL_EVENT_KEY_DOWN) ? true : false;
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowKeyEvent>(event.key.key, event.key.scancode, action, event.key.mod));
				} break;

				case SDL_EVENT_TEXT_INPUT: {
					const char* text = event.text.text;
					SDL_Keymod mods = SDL_GetModState(); // Get current modifier state
					for (int i = 0; text[i] != '\0'; ++i) {
						Uint32 codepoint = static_cast<Uint32>(text[i]); // ASCII codepoint
						(mods == SDL_KMOD_NONE || (mods & ~(SDL_KMOD_NUM | SDL_KMOD_CAPS)) == 0) ?
							g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowCharEvent>(codepoint)) :	// No mods or only NUMLOCK/CAPSLOCK active
							g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowCharModsEvent>(codepoint, mods));	// One or more modifying keys (Shift, Ctrl, Alt, etc.) are active
					}
				} break;
				case SDL_EVENT_MOUSE_BUTTON_UP:
				case SDL_EVENT_MOUSE_BUTTON_DOWN: {
					action = (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) ? true : false;
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowMouseButtonEvent>(event.button.button, action, SDL_GetModState()));
				} break;
				case SDL_EVENT_MOUSE_MOTION: {
					m_mouse_pos_x = static_cast<double>(event.motion.x);
					m_mouse_pos_y = static_cast<double>(event.motion.y);
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowCursorPosEvent>(m_mouse_pos_x, m_mouse_pos_y));
				} break;
				case SDL_EVENT_MOUSE_WHEEL: {
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowScrollEvent>(static_cast<float>(event.wheel.x), static_cast<float>(event.wheel.y)));
				} break;
				case SDL_EVENT_DROP_FILE: {
                    dropped_file_paths.push_back(event.drop.data);
                } break;
// SDL WINDOW EVENTS
				case SDL_EVENT_WINDOW_MOUSE_ENTER:
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowCursorEnterEvent>(true));
					break;
				case SDL_EVENT_WINDOW_MOUSE_LEAVE:
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowCursorEnterEvent>(false));
					break;
				case SDL_EVENT_WINDOW_RESIZED:
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowSizeEvent>(event.window.data1, event.window.data2));
					break;
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					m_should_close = true;
					break;
				default:
					break;
			}
		}

		if (!dropped_file_paths.empty()) {
			g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowDropEvent>(dropped_file_paths, m_mouse_pos_x, m_mouse_pos_y));
			dropped_file_paths.clear();
		}
	}

	bool WindowSystem::shouldClose() { 
		return m_should_close; 
	}
	void WindowSystem::setTitle(const std::string& title) {
		SDL_SetWindowTitle(m_window, title.c_str());
	}
	void WindowSystem::getWindowSize(int& width, int& height) {
		SDL_SetWindowSize(m_window, width, height);
	}
	void WindowSystem::getScreenSize(int& width, int& height) {
		const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
		width = mode->w;
		height = mode->h;
	}
	void WindowSystem::getMousePos(int& x, int& y) {
		x = m_mouse_pos_x;
		y = m_mouse_pos_y;
	}
	bool WindowSystem::isMouseButtonDown(int button) {
		if (button < SDL_BUTTON_LEFT || button > SDL_BUTTON_X2) {
			return false;
		} return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_MASK(button)) != 0;
	}

	void WindowSystem::setFocus(bool focus) {
		m_focus = focus;
		SDL_SetWindowRelativeMouseMode(m_window, m_focus);  // enable/disable relative mouse movement
		if (m_focus)
			SDL_HideCursor();	// Hide the cursor
		else 
			SDL_ShowCursor();	// Show the cursor
	}

	void WindowSystem::toggleFullscreen() {
		m_fullscreen = !m_fullscreen;
		if (m_fullscreen) { 	// Save current window size and position for restoration and Switch to fullscreen
			SDL_GetWindowPosition(m_window, &m_windowed_pos_x, &m_windowed_pos_y);
			SDL_GetWindowSize(m_window, &m_windowed_width, &m_windowed_height);
			SDL_SetWindowFullscreen(m_window, SDL_GetWindowFullscreenMode(m_window));
		} else { 	// Restore window size and position
			SDL_SetWindowFullscreen(m_window, 0);  // Disable fullscreen
			SDL_SetWindowPosition(m_window, m_windowed_pos_x, m_windowed_pos_y);
			SDL_SetWindowSize(m_window, m_windowed_width, m_windowed_height);
		}
	}

}