#include "collider_component.h"

REGISTER_AT_RUNTIME {
	meta_hpp::class_<Yurrgoht::ColliderComponent>()
		.member_("position", &Yurrgoht::ColliderComponent::m_position)
		.member_("rotation", &Yurrgoht::ColliderComponent::m_rotation);
}

CEREAL_REGISTER_TYPE(Yurrgoht::ColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Component, Yurrgoht::ColliderComponent)

namespace Yurrgoht {
	POLYMORPHIC_DEFINITION(ColliderComponent)
}