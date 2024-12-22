#include "sub_mesh.h"
#include "engine/function/global/engine_context.h"
#include "engine/resource/asset/asset_manager.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::SubMesh>("SubMesh")
	 .property("material", &Yurrgoht::SubMesh::m_material);
}

namespace Yurrgoht
{

	void SubMesh::bindRefs()
	{
		BIND_ASSET(m_material, Material)
	}

}