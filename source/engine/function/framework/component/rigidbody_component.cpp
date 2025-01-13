#include "rigidbody_component.h"
#include "engine/core/base/macro.h"

RTTR_REGISTRATION
{
rttr::registration::enumeration<Yurrgoht::EMotionType>("EMotionType")
(
	rttr::value("Static", Yurrgoht::EMotionType::Static),
	rttr::value("Kinematic", Yurrgoht::EMotionType::Kinematic),
	rttr::value("Dynamic", Yurrgoht::EMotionType::Dynamic)
);

rttr::registration::class_<Yurrgoht::RigidbodyComponent>("RigidbodyComponent")
	.property("motion_type", &Yurrgoht::RigidbodyComponent::m_motion_type)
	.property("friction", &Yurrgoht::RigidbodyComponent::m_friction)
	.property("restitution", &Yurrgoht::RigidbodyComponent::m_restitution)
	.property("linear_damping", &Yurrgoht::RigidbodyComponent::m_linear_damping)
	.property("angular_damping", &Yurrgoht::RigidbodyComponent::m_angular_damping)
	.property("gravity_factor", &Yurrgoht::RigidbodyComponent::m_gravity_factor);
}

CEREAL_REGISTER_TYPE(Yurrgoht::RigidbodyComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Component, Yurrgoht::RigidbodyComponent)

namespace Yurrgoht
{
	POLYMORPHIC_DEFINITION(RigidbodyComponent)
}