#include "static_mesh_component.h"
#include "engine/function/global/engine_context.h"
#include "engine/resource/asset/asset_manager.h"

REGISTER_AT_RUNTIME 
{
meta_hpp::class_<Yurrgoht::StaticMeshComponent>(meta_hpp::metadata_()("name", "StaticMeshComponent"s))
	.member_("static_mesh", &Yurrgoht::StaticMeshComponent::m_static_mesh, meta_hpp::metadata_()("type_name", "std::shared_ptr<Yurrgoht::StaticMesh>"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::StaticMeshComponent>("StaticMeshComponent");

//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::Component>(meta_hpp::metadata_()("derived_classes", "StaticMeshComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::StaticMeshComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Component, Yurrgoht::StaticMeshComponent)

namespace Yurrgoht {
	
	void StaticMeshComponent::setStaticMesh(std::shared_ptr<StaticMesh>& static_mesh) {
		REF_ASSET(m_static_mesh, static_mesh)
	}

	void StaticMeshComponent::bindRefs() {
		BIND_ASSET(m_static_mesh, StaticMesh)
	}

}