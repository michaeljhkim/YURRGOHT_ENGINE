#include "cylinder_collider_component.h"


REGISTER_AT_RUNTIME {
meta_hpp::class_<Yurrgoht::CylinderColliderComponent>(meta_hpp::metadata_()("name", "CylinderColliderComponent"s))
	.member_("radius", &Yurrgoht::CylinderColliderComponent::m_radius, meta_hpp::metadata_()("type_name", "float"s))
	.member_("height", &Yurrgoht::CylinderColliderComponent::m_height, meta_hpp::metadata_()("type_name", "float"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::CylinderColliderComponent>("CylinderColliderComponent");

//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::Component>(meta_hpp::metadata_()("derived_classes", "CylinderColliderComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::CylinderColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::ColliderComponent, Yurrgoht::CylinderColliderComponent)

namespace Yurrgoht {
	
	CylinderColliderComponent::CylinderColliderComponent() {
		m_type = EColliderType::Cylinder;
		m_radius = 1.0f;
		m_height = 2.0f;
	}

}