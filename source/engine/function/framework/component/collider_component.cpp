#include "collider_component.h"

REGISTER_AT_RUNTIME {
meta_hpp::class_<Yurrgoht::ColliderComponent>(meta_hpp::metadata_()("name", "ColliderComponent"s))
	.member_("position", &Yurrgoht::ColliderComponent::m_position, meta_hpp::metadata_()("type_name", "glm::vec3"s))
	.member_("rotation", &Yurrgoht::ColliderComponent::m_rotation, meta_hpp::metadata_()("type_name", "glm::vec3"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::ColliderComponent>("ColliderComponent");
	
//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::Component>(meta_hpp::metadata_()("derived_classes", "ColliderComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::ColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Component, Yurrgoht::ColliderComponent)

namespace Yurrgoht {
	POLYMORPHIC_DEFINITION(ColliderComponent)
}