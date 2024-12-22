#include "capsule_collider_component.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::CapsuleColliderComponent>("CapsuleColliderComponent")
	.property("radius", &Yurrgoht::CapsuleColliderComponent::m_radius)
	.property("height", &Yurrgoht::CapsuleColliderComponent::m_height);
}

CEREAL_REGISTER_TYPE(Yurrgoht::CapsuleColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::ColliderComponent, Yurrgoht::CapsuleColliderComponent)

namespace Yurrgoht
{

	CapsuleColliderComponent::CapsuleColliderComponent()
	{
		m_type = EColliderType::Capsule;
		m_radius = 1.0f;
		m_height = 2.0f;
	}

}