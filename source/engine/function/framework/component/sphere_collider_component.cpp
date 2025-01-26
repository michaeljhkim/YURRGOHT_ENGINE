#include "sphere_collider_component.h"

REGISTER_AT_RUNTIME 
{
meta_hpp::class_<Yurrgoht::SphereColliderComponent>(meta_hpp::metadata_()("name", "SphereColliderComponent"s))
	.member_("radius", &Yurrgoht::SphereColliderComponent::m_radius, meta_hpp::metadata_()("type_name", "float"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::SphereColliderComponent>("SphereColliderComponent");
	
//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::ColliderComponent>(meta_hpp::metadata_()("derived_classes", "SphereColliderComponent"s));
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