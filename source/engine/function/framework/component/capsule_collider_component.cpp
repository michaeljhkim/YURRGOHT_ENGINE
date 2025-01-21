#include "capsule_collider_component.h"

REGISTER_AT_RUNTIME {
	meta_hpp::class_<Yurrgoht::CapsuleColliderComponent>(meta_hpp::metadata_()("name", "CapsuleColliderComponent"s))
		.member_("radius", &Yurrgoht::CapsuleColliderComponent::m_radius)
		.member_("height", &Yurrgoht::CapsuleColliderComponent::m_height);
}

CEREAL_REGISTER_TYPE(Yurrgoht::CapsuleColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::ColliderComponent, Yurrgoht::CapsuleColliderComponent)

namespace Yurrgoht {

	CapsuleColliderComponent::CapsuleColliderComponent() {
		m_type = EColliderType::Capsule;
		m_radius = 1.0f;
		m_height = 2.0f;
	}

}