#include "light_component.h"

REGISTER_AT_RUNTIME {
	meta_hpp::class_<Yurrgoht::LightComponent>()
		.member_("intensity", &Yurrgoht::LightComponent::m_intensity)
		.member_("color", &Yurrgoht::LightComponent::m_color)
		.member_("cast_shadow", &Yurrgoht::LightComponent::m_cast_shadow);
}

CEREAL_REGISTER_TYPE(Yurrgoht::LightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Component, Yurrgoht::LightComponent)

namespace Yurrgoht
{

	POLYMORPHIC_DEFINITION(LightComponent)

	LightComponent::LightComponent()
	{
		m_intensity = 1.0f;
		m_color = Color3::White;
		m_cast_shadow = true;
	}

	glm::vec3 LightComponent::getColor()
	{
		return (m_color * m_intensity).toVec3();
	}

}