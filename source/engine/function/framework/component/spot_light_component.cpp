#include "spot_light_component.h"

REGISTER_AT_RUNTIME 
{
meta_hpp::class_<Yurrgoht::SpotLightComponent>(meta_hpp::metadata_()("name", "SpotLightComponent"s))
	 .member_("inner_cone_angle", &Yurrgoht::SpotLightComponent::m_inner_cone_angle)
	 .member_("outer_cone_angle", &Yurrgoht::SpotLightComponent::m_outer_cone_angle);
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::Component>("SpotLightComponent");
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