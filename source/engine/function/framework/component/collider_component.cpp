#include "collider_component.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::ColliderComponent>("ColliderComponent")
	.property("position", &Yurrgoht::ColliderComponent::m_position)
	.property("rotation", &Yurrgoht::ColliderComponent::m_rotation);
}

CEREAL_REGISTER_TYPE(Yurrgoht::ColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Component, Yurrgoht::ColliderComponent)

namespace Yurrgoht
{
	POLYMORPHIC_DEFINITION(ColliderComponent)
}