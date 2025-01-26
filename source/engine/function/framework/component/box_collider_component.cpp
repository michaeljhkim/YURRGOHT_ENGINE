#include "box_collider_component.h"

REGISTER_AT_RUNTIME {
meta_hpp::class_<Yurrgoht::BoxColliderComponent>(meta_hpp::metadata_()("name", "BoxColliderComponent"s))
	.member_("size", &Yurrgoht::BoxColliderComponent::m_size, meta_hpp::metadata_()("type_name", "glm::vec3"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::BoxColliderComponent>("BoxColliderComponent");

//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::ColliderComponent>(meta_hpp::metadata_()("derived_classes", "BoxColliderComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::BoxColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::ColliderComponent, Yurrgoht::BoxColliderComponent)

namespace Yurrgoht {

	BoxColliderComponent::BoxColliderComponent() {
		m_type = EColliderType::Box;
		m_size = glm::vec3(1.0f);
	}

}