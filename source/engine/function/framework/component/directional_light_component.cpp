#include "directional_light_component.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::DirectionalLightComponent>("DirectionalLightComponent")
	.property("cascade_frustum_near", &Yurrgoht::DirectionalLightComponent::m_cascade_frustum_near);
}

CEREAL_REGISTER_TYPE(Yurrgoht::DirectionalLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::LightComponent, Yurrgoht::DirectionalLightComponent)

namespace Yurrgoht
{
	DirectionalLightComponent::DirectionalLightComponent()
	{
		m_intensity = 4.0f;
		m_cascade_frustum_near = 0.0f;
	}

}
