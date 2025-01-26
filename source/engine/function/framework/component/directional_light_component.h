#pragma once

#include "light_component.h"

namespace Yurrgoht {
	class DirectionalLightComponent : public LightComponent {
	public:
		DirectionalLightComponent();
		
		float m_cascade_frustum_near;

	private:
		REGISTER_REFLECTION(LightComponent)
		
		template<class Archive>
		void serialize(Archive& ar) {
			ar(cereal::make_nvp("light", cereal::base_class<LightComponent>(this)));
			ar(cereal::make_nvp("cascade_frustum_near", m_cascade_frustum_near));
		}
	};
}