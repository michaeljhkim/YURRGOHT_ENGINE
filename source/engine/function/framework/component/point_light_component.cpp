#include "point_light_component.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::PointLightComponent>("PointLightComponent")
	 .property("radius", &Yurrgoht::PointLightComponent::m_radius)
	 .property("linear_attenuation", &Yurrgoht::PointLightComponent::m_linear_attenuation)
	 .property("quadratic_attenuation", &Yurrgoht::PointLightComponent::m_quadratic_attenuation);
}

CEREAL_REGISTER_TYPE(Yurrgoht::PointLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::LightComponent, Yurrgoht::PointLightComponent)

namespace Yurrgoht
{
	POLYMORPHIC_DEFINITION(PointLightComponent)

	PointLightComponent::PointLightComponent()
	{
		m_radius = 64.0f;
		m_linear_attenuation = 0.14f;
		m_quadratic_attenuation = 0.07f;
	}

}