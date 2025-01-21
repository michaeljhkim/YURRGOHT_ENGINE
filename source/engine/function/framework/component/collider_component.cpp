#include "collider_component.h"

REGISTER_AT_RUNTIME {
meta_hpp::class_<Yurrgoht::ColliderComponent>(meta_hpp::metadata_()("name", "ColliderComponent"s))
	.member_("position", &Yurrgoht::ColliderComponent::m_position)
	.member_("rotation", &Yurrgoht::ColliderComponent::m_rotation);
	
//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::Component>(meta_hpp::metadata_()("derived_classes", "ColliderComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::ColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Component, Yurrgoht::ColliderComponent)

namespace Yurrgoht {
	POLYMORPHIC_DEFINITION(ColliderComponent)
}