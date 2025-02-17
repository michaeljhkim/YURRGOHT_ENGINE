#include "point_light_component.h"

REGISTER_AT_RUNTIME 
{
meta_hpp::class_<Yurrgoht::PointLightComponent>(meta_hpp::metadata_()("name", "PointLightComponent"s))
	.member_("radius", &Yurrgoht::PointLightComponent::m_radius, meta_hpp::metadata_()("type_name", "float"s))
	.member_("linear_attenuation", &Yurrgoht::PointLightComponent::m_linear_attenuation, meta_hpp::metadata_()("type_name", "float"s))
	.member_("quadratic_attenuation", &Yurrgoht::PointLightComponent::m_quadratic_attenuation, meta_hpp::metadata_()("type_name", "float"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::PointLightComponent>("PointLightComponent");
	
//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::LightComponent>(meta_hpp::metadata_()("derived_classes", "PointLightComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::PointLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::LightComponent, Yurrgoht::PointLightComponent)

namespace Yurrgoht {
	POLYMORPHIC_DEFINITION(PointLightComponent)

	PointLightComponent::PointLightComponent() {
		m_radius = 64.0f;
		m_linear_attenuation = 0.14f;
		m_quadratic_attenuation = 0.07f;
	}

}