#include "rigidbody_component.h"
#include "engine/core/base/macro.h"

REGISTER_AT_RUNTIME
{
meta_hpp::enum_<Yurrgoht::EMotionType>(meta_hpp::metadata_()("name", "EMotionType"s))
        .evalue_("Static", Yurrgoht::EMotionType::Static)
        .evalue_("Kinematic", Yurrgoht::EMotionType::Kinematic)
        .evalue_("Dynamic", Yurrgoht::EMotionType::Dynamic);

meta_hpp::class_<Yurrgoht::RigidbodyComponent>(meta_hpp::metadata_()("name", "RigidbodyComponent"s))
	.member_("motion_type", &Yurrgoht::RigidbodyComponent::m_motion_type, meta_hpp::metadata_()("type_name", "EMotionType"s))
	.member_("friction", &Yurrgoht::RigidbodyComponent::m_friction, meta_hpp::metadata_()("type_name", "float"s))
	.member_("restitution", &Yurrgoht::RigidbodyComponent::m_restitution, meta_hpp::metadata_()("type_name", "float"s))
	.member_("linear_damping", &Yurrgoht::RigidbodyComponent::m_linear_damping, meta_hpp::metadata_()("type_name", "float"s))
	.member_("angular_damping", &Yurrgoht::RigidbodyComponent::m_angular_damping, meta_hpp::metadata_()("type_name", "float"s))
	.member_("gravity_factor", &Yurrgoht::RigidbodyComponent::m_gravity_factor, meta_hpp::metadata_()("type_name", "float"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::RigidbodyComponent>("RigidbodyComponent");

//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::Component>(meta_hpp::metadata_()("derived_classes", "RigidbodyComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::RigidbodyComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Component, Yurrgoht::RigidbodyComponent)

namespace Yurrgoht {
	POLYMORPHIC_DEFINITION(RigidbodyComponent)
	
	RigidbodyComponent::RigidbodyComponent() {}
}