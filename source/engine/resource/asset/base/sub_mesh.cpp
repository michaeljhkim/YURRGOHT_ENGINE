#include "sub_mesh.h"
#include "engine/function/global/engine_context.h"
#include "engine/resource/asset/asset_manager.h"

REGISTER_AT_RUNTIME
{
meta_hpp::class_<Yurrgoht::SubMesh>()
	.member_("material", &Yurrgoht::SubMesh::m_material);
}

namespace Yurrgoht
{

	void SubMesh::bindRefs()
	{
		BIND_ASSET(m_material, Material)
	}

}