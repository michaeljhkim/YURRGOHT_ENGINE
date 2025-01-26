#include "mesh_collider_component.h"
#include "engine/resource/asset/asset_manager.h"

REGISTER_AT_RUNTIME {
meta_hpp::class_<Yurrgoht::MeshColliderComponent>(meta_hpp::metadata_()("name", "MeshColliderComponent"s))
	.member_("static_mesh", &Yurrgoht::MeshColliderComponent::m_static_mesh, meta_hpp::metadata_()("type_name", "std::shared_ptr<Yurrgoht::StaticMesh>"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::MeshColliderComponent>("MeshColliderComponent");

//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::ColliderComponent>(meta_hpp::metadata_()("derived_classes", "MeshColliderComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::MeshColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::ColliderComponent, Yurrgoht::MeshColliderComponent)

namespace Yurrgoht {

	MeshColliderComponent::MeshColliderComponent() {
		m_type = EColliderType::Mesh;
	}

	void MeshColliderComponent::setStaticMesh(std::shared_ptr<StaticMesh>& static_mesh) {
		REF_ASSET(m_static_mesh, static_mesh)
	}

	void MeshColliderComponent::bindRefs() {
		BIND_ASSET(m_static_mesh, StaticMesh)
	}
}