#include "skeletal_mesh_component.h"
#include "engine/function/global/engine_context.h"
#include "engine/resource/asset/asset_manager.h"

REGISTER_AT_RUNTIME 
{
meta_hpp::class_<Yurrgoht::SkeletalMeshComponent>(meta_hpp::metadata_()("name", "SkeletalMeshComponent"s))
	.member_("skeletal_mesh", &Yurrgoht::SkeletalMeshComponent::m_skeletal_mesh, meta_hpp::metadata_()("type_name", "std::shared_ptr<Yurrgoht::SkeletalMesh>"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::SkeletalMeshComponent>("SkeletalMeshComponent");
	
//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::Component>(meta_hpp::metadata_()("derived_classes", "SkeletalMeshComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::SkeletalMeshComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Component, Yurrgoht::SkeletalMeshComponent)

namespace Yurrgoht {

	void SkeletalMeshComponent::setSkeletalMesh(std::shared_ptr<SkeletalMesh>& skeletal_mesh) {
		REF_ASSET(m_skeletal_mesh, skeletal_mesh)
	}

	void SkeletalMeshComponent::bindRefs() {
		BIND_ASSET(m_skeletal_mesh, SkeletalMesh)
	}

}