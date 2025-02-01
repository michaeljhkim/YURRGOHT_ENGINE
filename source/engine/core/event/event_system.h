#pragma once

#include <eventpp/eventqueue.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <vector>
#include <string>

namespace Yurrgoht {

	enum class EEventType {
		WindowReset, WindowKey, WindowChar, WindowCharMods, WindowMouseButton,
		WindowCursorPos, WindowCursorEnter, WindowScroll, WindowDrop, WindowSize, WindowClose,
		RenderCreateSwapchainObjects, RenderDestroySwapchainObjects, RenderRecordFrame, RenderConstructUI,
		SelectEntity, PickEntity
	};

	class Event {
	public:
		explicit Event(EEventType type) : type(type) {}
		EEventType type;
	};
	using EventPointer = std::shared_ptr<Event>;

	struct EventPolicy {
		static EEventType getEvent(const EventPointer& event) {
			return event->type;
		}
	};
	using EventQueue = eventpp::EventQueue<EEventType, void(const EventPointer&), EventPolicy>;

	struct EventHandle {
		EEventType type;
		EventQueue::Handle handle;
	};

	class WindowResetEvent : public Event {
	public:
		WindowResetEvent() : Event(EEventType::WindowReset) {}
	};

	class WindowKeyEvent : public Event {
	public:
		WindowKeyEvent(SDL_Keycode key, SDL_Scancode scancode, int action, Uint16 mods) : Event(EEventType::WindowKey), key(key), scancode(scancode), action(action), mods(mods) {
		}
		SDL_Keycode key;
		SDL_Scancode scancode;
		int action;
		Uint16 mods;
	};

	class WindowCharEvent : public Event {
	public:
		WindowCharEvent(unsigned int codepoint) : Event(EEventType::WindowChar), codepoint(codepoint) {
		}
		unsigned int codepoint;
	};

	class WindowCharModsEvent : public Event {
	public:
		WindowCharModsEvent(unsigned int codepoint, SDL_Keymod mods) : Event(EEventType::WindowCharMods), codepoint(codepoint), mods(mods) {
		}
		unsigned int codepoint;
		SDL_Keymod mods;
	};

	class WindowMouseButtonEvent : public Event {
	public:
		WindowMouseButtonEvent(int button, int action, SDL_Keymod mods) : Event(EEventType::WindowMouseButton), button(button), action(action), mods(mods) {
		}
		int button;
		int action;
		SDL_Keymod mods;
	};

	class WindowCursorPosEvent : public Event {
	public:
		WindowCursorPosEvent(double xpos, double ypos) : Event(EEventType::WindowCursorPos), xpos(xpos), ypos(ypos) {
		}
		double xpos;
		double ypos;
	};

	class WindowCursorEnterEvent : public Event {
	public:
		WindowCursorEnterEvent(int entered) : Event(EEventType::WindowCursorEnter), entered(entered) {
		}
		int entered;
	};

	class WindowScrollEvent : public Event {
	public:
		WindowScrollEvent(double xoffset, double yoffset) : Event(EEventType::WindowScroll), xoffset(xoffset), yoffset(yoffset) {
		}
		double xoffset;
		double yoffset;
	};

	class WindowDropEvent : public Event {
	public:
		WindowDropEvent(std::vector<std::string> paths, double xpos, double ypos) : 
			Event(EEventType::WindowDrop), filenames(paths), xpos(xpos), ypos(ypos) {
		}
		std::vector<std::string> filenames;
		double xpos;
		double ypos;
	};

	class WindowSizeEvent : public Event {
	public:
		WindowSizeEvent(int width, int height) : Event(EEventType::WindowSize), width(width), height(height) {
		}
		int width;
		int height;
	};

	class WindowCloseEvent : public Event {
	public:
		WindowCloseEvent() : Event(EEventType::WindowClose) {
		}
	};

	class RenderCreateSwapchainObjectsEvent : public Event {
	public:
		RenderCreateSwapchainObjectsEvent(uint32_t width, uint32_t height) : Event(EEventType::RenderCreateSwapchainObjects), width(width), height(height) {
		}
		uint32_t width;
		uint32_t height;
	};

	class RenderDestroySwapchainObjectsEvent : public Event {
	public:
		RenderDestroySwapchainObjectsEvent() : Event(EEventType::RenderDestroySwapchainObjects) {
		}
	};

	class RenderRecordFrameEvent : public Event {
	public:
		RenderRecordFrameEvent() : Event(EEventType::RenderRecordFrame) {
		}
	};

	class RenderConstructUIEvent : public Event {
	public:
		RenderConstructUIEvent() : Event(EEventType::RenderConstructUI) {
		}
	};
	
	class SelectEntityEvent : public Event {
	public:
		SelectEntityEvent(uint32_t entity_id) : Event(EEventType::SelectEntity), entity_id(entity_id) {
		}
		uint32_t entity_id;
	};

	class PickEntityEvent : public Event {
	public:
		PickEntityEvent(uint32_t mouse_x, uint32_t mouse_y) : Event(EEventType::PickEntity), mouse_x(mouse_x), mouse_y(mouse_y) {
		}
		uint32_t mouse_x;
		uint32_t mouse_y;
	};

	class EventSystem {
	public:
		void init() {}
		void tick();
		void destroy() {}

		void* addListener(EEventType event_type, const std::function<void(const EventPointer& event_pointer)>& callback);
		void removeListener(void* handle);
		void asyncDispatch(const EventPointer& event_pointer);
		void syncDispatch(const EventPointer& event_pointer);

	private:
		EventQueue m_event_queue;
	};
}