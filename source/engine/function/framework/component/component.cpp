#include "component.h"
#include "engine/function/framework/entity/entity.h"

namespace Yurrgoht {
	REGISTER_AT_RUNTIME {
	meta_hpp::class_<Yurrgoht::Component>(meta_hpp::metadata_()("name", "Component"s));
	meta_hpp::extend_scope_(global_reflection_scope)
		.typedef_<Yurrgoht::Component>("Component");
	}

	void ITickable::tickable(float delta_time) {
		if (!m_tick_enabled)
			return;
		if (m_tick_interval == 0.0f)
			tick(delta_time);

		else {
			m_tick_timer += delta_time;
			if (m_tick_timer > m_tick_interval) {
				while (m_tick_timer > m_tick_interval)
					m_tick_timer -= m_tick_interval;

				std::chrono::time_point<std::chrono::steady_clock> now_tick_time = std::chrono::steady_clock::now();
				float tick_delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(now_tick_time - m_last_tick_time).count() * 0.001f;
				m_last_tick_time = now_tick_time;
				tick(tick_delta_time);
			}
		}
	
	}

	void Component::attach(std::weak_ptr<Entity>& parent) {
		m_parent = parent;
	}

	void Component::detach() {
		m_parent.reset();
	}

}