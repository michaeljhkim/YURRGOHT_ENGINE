#include "cylinder_collider_component.h"


REGISTER_AT_RUNTIME {
	meta_hpp::class_<Yurrgoht::CylinderColliderComponent>()
	.member_("radius", &Yurrgoht::CylinderColliderComponent::m_radius)
	.member_("height", &Yurrgoht::CylinderColliderComponent::m_height);
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