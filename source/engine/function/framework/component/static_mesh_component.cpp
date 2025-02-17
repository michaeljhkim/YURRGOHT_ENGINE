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
		// calculates the absolute min/max vertex position for the original size of the mesh
		glm::vec3 position_min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
		glm::vec3 position_max = {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
		for (auto mesh: static_mesh->m_vertices) {
			position_min = glm::min(mesh.m_position, position_min);
			position_max = glm::max(mesh.m_position, position_max);
		}
		default_size = position_max - position_min;

		REF_ASSET(m_static_mesh, static_mesh)
	}

	void StaticMeshComponent::bindRefs() {
		BIND_ASSET(m_static_mesh, StaticMesh)
	}

}