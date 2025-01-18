#include "box_collider_component.h"

REGISTER_AT_RUNTIME {
	meta_hpp::class_<Yurrgoht::BoxColliderComponent>()
	.member_("size", &Yurrgoht::BoxColliderComponent::m_size);
}

CEREAL_REGISTER_TYPE(Yurrgoht::BoxColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::ColliderComponent, Yurrgoht::BoxColliderComponent)

namespace Yurrgoht {

	BoxColliderComponent::BoxColliderComponent() {
		m_type = EColliderType::Box;
		m_size = glm::vec3(1.0f);
	}

}