#include "spot_light_component.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::SpotLightComponent>("SpotLightComponent")
	 .property("inner_cone_angle", &Yurrgoht::SpotLightComponent::m_inner_cone_angle)
	 .property("outer_cone_angle", &Yurrgoht::SpotLightComponent::m_outer_cone_angle);
}

CEREAL_REGISTER_TYPE(Yurrgoht::SpotLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::PointLightComponent, Yurrgoht::SpotLightComponent)

namespace Yurrgoht
{
	SpotLightComponent::SpotLightComponent()
	{
		m_inner_cone_angle = 30.0f;
		m_outer_cone_angle = 45.0f;
	}

}