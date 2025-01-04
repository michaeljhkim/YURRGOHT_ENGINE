#include "window_system.h"
#include "engine/core/base/macro.h"
#include "engine/core/config/config_manager.h"
#include "engine/core/event/event_system.h"

#include <tinygltf/stb_image.h>

namespace Yurrgoht {

	void WindowSystem::init() {
		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			LOG_FATAL("failed to initialize sdl2");
			return;
		}

		m_focus = false;
		m_fullscreen = g_engine.configManager()->isFullscreen();
		SDL_DisplayMode* mode;
		SDL_GetDisplayMode(0, 0, mode);

		// create sdl2 window
		bool is_packaged_fullscreen = m_fullscreen && g_engine.isApplication();
		int width = is_packaged_fullscreen ? mode->w : g_engine.configManager()->getWindowWidth();
		int height = is_packaged_fullscreen ? mode->h: g_engine.configManager()->getWindowHeight();
		m_window = SDL_CreateWindow(APP_NAME,    /* Title of the SDL window */
                    SDL_WINDOWPOS_UNDEFINED,    /* Position x of the window */
                    SDL_WINDOWPOS_UNDEFINED,    /* Position y of the window */
                    width, height,     /* Width and Height of the window in pixels */
                    SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE); 
		if (!m_window) {
			LOG_FATAL("failed to create sdl2 window");
			SDL_Quit(); 
			return;
		}

		// set fullscreen if needed
		if (is_packaged_fullscreen) {
			m_windowed_width = g_engine.configManager()->getWindowWidth();
			m_windowed_height = g_engine.configManager()->getWindowHeight();
			m_windowed_pos_x = (mode->w - m_windowed_width) * 0.5f;
			m_windowed_pos_y = (mode->h - m_windowed_height) * 0.5f;
			SDL_SetWindowDisplayMode(m_window, mode);
		}

		// Enable v-sync
		SDL_GL_SetSwapInterval(1);  // 1 enables v-sync

		// Disable cursor
		//SDL_ShowCursor(SDL_DISABLE);
		//SDL_WarpMouseInWindow(m_window, 0, 0);
		//SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_ShowCursor(SDL_ENABLE);
		SDL_SetRelativeMouseMode(SDL_FALSE);

		// Enable raw mouse motion (without hiding the cursor)
		/*
		if (SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE)) {
			SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");
		} else {
			SDL_Log("Raw mouse motion not forced to warp mode, but will still work.");
		}
		*/

		// set window icon
		/*
		SDL_Surface* iconSurface; 
		iconSurface->pixels = stbi_load(TO_ABSOLUTE("asset/engine/texture/icon/yurrgoht_small.png").c_str(), &iconSurface->w, &iconSurface->h, 0, 4);
		if (iconSurface != nullptr) {
			SDL_SetWindowIcon(m_window, iconSurface);
			SDL_FreeSurface(iconSurface); 
		} else { 
			SDL_Log("Failed to load icon: %s", SDL_GetError()); 
		}
		*/
	}

	void WindowSystem::destroy() {
		SDL_DestroyWindow(m_window);
		SDL_Quit(); 
	}

	void WindowSystem::pollEvents() {
		SDL_Event event;
		int action;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			switch (event.type) {
				case SDL_QUIT: {
					m_should_close = true;
					break;
				}
				case SDL_KEYUP:
				case SDL_KEYDOWN: {
					action = (event.type == SDL_KEYDOWN) ? SDL_PRESSED : SDL_RELEASED;
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowKeyEvent>(event.key.keysym.sym, event.key.keysym.scancode, action, event.key.keysym.mod));
				} break;

				case SDL_TEXTINPUT: {
					const char* text = event.text.text;
					SDL_Keymod mods = SDL_GetModState(); // Get current modifier state
					for (int i = 0; text[i] != '\0'; ++i) {
						Uint32 codepoint = static_cast<Uint32>(text[i]); // ASCII codepoint
						(mods == KMOD_NONE || (mods & ~(KMOD_NUM | KMOD_CAPS)) == 0) ?
							g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowCharEvent>(codepoint)) :	// No mods or only NUMLOCK/CAPSLOCK active
							g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowCharModsEvent>(codepoint, mods));	// One or more modifying keys (Shift, Ctrl, Alt, etc.) are active
					}
				} break;
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN: {
					action = (event.type == SDL_MOUSEBUTTONDOWN) ? SDL_PRESSED : SDL_RELEASED;
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowMouseButtonEvent>(event.button.button, action, SDL_GetModState()));
				} break;
				case SDL_MOUSEMOTION: {
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowCursorPosEvent>(event.motion.x, event.motion.y));
				} break;
				case SDL_MOUSEWHEEL: {
					g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowScrollEvent>(static_cast<float>(event.wheel.x), static_cast<float>(event.wheel.y)));
				} break;
				case SDL_WINDOWEVENT:
					switch (event.window.event) {
						case SDL_WINDOWEVENT_ENTER:
							g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowCursorEnterEvent>(true));
							break;
						case SDL_WINDOWEVENT_LEAVE:
							g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowCursorEnterEvent>(false));
							break;
						case SDL_WINDOWEVENT_SIZE_CHANGED:
							g_engine.eventSystem()->asyncDispatch(std::make_shared<WindowSizeEvent>(event.window.data1, event.window.data2));
							break;
						case SDL_WINDOWEVENT_CLOSE:
							m_should_close = true;
							break;
					} break;
				default:
					break;
			}
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
		SDL_DisplayMode* mode;
		SDL_GetDisplayMode(0, 0, mode);
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
		} return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(button)) != 0;
	}

	void WindowSystem::setFocus(bool focus) {
		m_focus = focus;
		if (m_focus) {
			SDL_SetRelativeMouseMode(SDL_TRUE);  // Hide the cursor and allow relative mouse movement
			SDL_ShowCursor(SDL_DISABLE);         // Hide the cursor
		} else {
			SDL_SetRelativeMouseMode(SDL_FALSE); // Disable relative mouse movement
			SDL_ShowCursor(SDL_ENABLE);          // Show the cursor
		}
	}

	void WindowSystem::toggleFullscreen() {
		m_fullscreen = !m_fullscreen;
		if (m_fullscreen) { 	// Save current window size and position for restoration and Switch to fullscreen
			SDL_GetWindowPosition(m_window, &m_windowed_pos_x, &m_windowed_pos_y);
			SDL_GetWindowSize(m_window, &m_windowed_width, &m_windowed_height);
			SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		} else { 	// Restore window size and position
			SDL_SetWindowFullscreen(m_window, 0);  // Disable fullscreen
			SDL_SetWindowPosition(m_window, m_windowed_pos_x, m_windowed_pos_y);
			SDL_SetWindowSize(m_window, m_windowed_width, m_windowed_height);
		}
	}

}