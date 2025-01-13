#include "mesh_collider_component.h"
#include "engine/resource/asset/asset_manager.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::MeshColliderComponent>("MeshColliderComponent")
	.property("static_mesh", &Yurrgoht::MeshColliderComponent::m_static_mesh);
}

CEREAL_REGISTER_TYPE(Yurrgoht::MeshColliderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::ColliderComponent, Yurrgoht::MeshColliderComponent)

namespace Yurrgoht
{

	MeshColliderComponent::MeshColliderComponent()
	{
		m_type = EColliderType::Mesh;
	}

	void MeshColliderComponent::setStaticMesh(std::shared_ptr<StaticMesh>& static_mesh)
	{
		REF_ASSET(m_static_mesh, static_mesh)
	}

	void MeshColliderComponent::bindRefs()
	{
		BIND_ASSET(m_static_mesh, StaticMesh)
	}
}