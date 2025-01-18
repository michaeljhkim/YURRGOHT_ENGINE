#include "sphere_collider_component.h"

REGISTER_AT_RUNTIME 
{
meta_hpp::class_<Yurrgoht::SphereColliderComponent>()
	.member_("radius", &Yurrgoht::SphereColliderComponent::m_radius);
}

CEREAL_REGISTER_TYPE(Yurrgoht::SphereColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::ColliderComponent, Yurrgoht::SphereColliderComponent)

namespace Yurrgoht
{

	SphereColliderComponent::SphereColliderComponent()
	{
		m_type = EColliderType::Sphere;
		m_radius = 1.0f;
	}

}