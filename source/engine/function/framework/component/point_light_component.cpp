#include "point_light_component.h"

REGISTER_AT_RUNTIME 
{
meta_hpp::class_<Yurrgoht::PointLightComponent>()
	.member_("radius", &Yurrgoht::PointLightComponent::m_radius)
	.member_("linear_attenuation", &Yurrgoht::PointLightComponent::m_linear_attenuation)
	.member_("quadratic_attenuation", &Yurrgoht::PointLightComponent::m_quadratic_attenuation);
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