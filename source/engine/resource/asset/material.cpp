#include "material.h"
#include "engine/function/global/engine_context.h"
#include "engine/resource/asset/asset_manager.h"

RTTR_REGISTRATION
{
rttr::registration::class_<Yurrgoht::Material>("Material")
	 .property("base_color_texure", &Yurrgoht::Material::m_base_color_texure)
	 .property("metallic_roughness_occlusion_texure", &Yurrgoht::Material::m_metallic_roughness_occlusion_texure)
	 .property("normal_texure", &Yurrgoht::Material::m_normal_texure)
	 .property("emissive_texure", &Yurrgoht::Material::m_emissive_texure);
}

CEREAL_REGISTER_TYPE(Yurrgoht::Material)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Asset, Yurrgoht::Material)

namespace Yurrgoht
{

	void Material::bindRefs()
	{
		BIND_ASSET(m_base_color_texure, Texture2D)
		BIND_ASSET(m_metallic_roughness_occlusion_texure, Texture2D)
		BIND_ASSET(m_normal_texure, Texture2D)
		BIND_ASSET(m_emissive_texure, Texture2D)
	}

}