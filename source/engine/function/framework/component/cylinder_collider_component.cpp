#include "cylinder_collider_component.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::CylinderColliderComponent>("CylinderColliderComponent")
	.property("radius", &Yurrgoht::CylinderColliderComponent::m_radius)
	.property("height", &Yurrgoht::CylinderColliderComponent::m_height);
}

CEREAL_REGISTER_TYPE(Yurrgoht::CylinderColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::ColliderComponent, Yurrgoht::CylinderColliderComponent)

namespace Yurrgoht
{
	
	CylinderColliderComponent::CylinderColliderComponent()
	{
		m_type = EColliderType::Cylinder;
		m_radius = 1.0f;
		m_height = 2.0f;
	}

}