#include "directional_light_component.h"


REGISTER_AT_RUNTIME {
meta_hpp::class_<Yurrgoht::DirectionalLightComponent>(meta_hpp::metadata_()("name", "DirectionalLightComponent"s))
	.member_("cascade_frustum_near", &Yurrgoht::DirectionalLightComponent::m_cascade_frustum_near, meta_hpp::metadata_()("type_name", "float"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::DirectionalLightComponent>("DirectionalLightComponent");

//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::LightComponent>(meta_hpp::metadata_()("derived_classes", "DirectionalLightComponent"s));
}


CEREAL_REGISTER_TYPE(Yurrgoht::DirectionalLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::LightComponent, Yurrgoht::DirectionalLightComponent)

namespace Yurrgoht {
	DirectionalLightComponent::DirectionalLightComponent() {
		m_intensity = 4.0f;
		m_cascade_frustum_near = 0.0f;
	}
}
