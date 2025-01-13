#include "box_collider_component.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::BoxColliderComponent>("BoxColliderComponent")
	.property("size", &Yurrgoht::BoxColliderComponent::m_size);
}

CEREAL_REGISTER_TYPE(Yurrgoht::BoxColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::ColliderComponent, Yurrgoht::BoxColliderComponent)

namespace Yurrgoht
{

	BoxColliderComponent::BoxColliderComponent()
	{
		m_type = EColliderType::Box;
		m_size = glm::vec3(1.0f);
	}

}