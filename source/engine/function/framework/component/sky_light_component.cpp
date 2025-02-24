#include "sky_light_component.h"
#include "engine/resource/asset/asset_manager.h"
#include "engine/function/render/pass/brdf_lut_pass.h"
#include "engine/function/render/pass/filter_cube_pass.h"
#include "engine/resource/asset/texture_2d.h"

REGISTER_AT_RUNTIME 
{
meta_hpp::class_<Yurrgoht::SkyLightComponent>(meta_hpp::metadata_()("name", "SkyLightComponent"s))
	.method_("getTextureCube", &Yurrgoht::SkyLightComponent::getTextureCube, meta_hpp::metadata_()("type_name", "std::shared_ptr<Yurrgoht::TextureCube>"s))
	.member_("texture_cube", &Yurrgoht::SkyLightComponent::m_texture_cube, meta_hpp::metadata_()("type_name", "std::shared_ptr<Yurrgoht::TextureCube>"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::SkyLightComponent>("SkyLightComponent");
	
//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::LightComponent>(meta_hpp::metadata_()("derived_classes", "SkyLightComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::SkyLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::LightComponent, Yurrgoht::SkyLightComponent)

namespace Yurrgoht {

	SkyLightComponent::SkyLightComponent() {
		m_intensity = 0.5f;
		m_prefilter_mip_levels = 0;
	}

	SkyLightComponent::~SkyLightComponent() {
		m_irradiance_texture_sampler.destroy();
		m_prefilter_texture_sampler.destroy();
	}

	void SkyLightComponent::setTextureCube(std::shared_ptr<TextureCube>& texture_cube) {
		REF_ASSET(m_texture_cube, texture_cube)
		createIBLTextures();
	}

	void SkyLightComponent::bindRefs() {
		BIND_ASSET(m_texture_cube, TextureCube)
	}

	void SkyLightComponent::inflate() {
		// create ibl textures
		createIBLTextures();
	}

	void SkyLightComponent::createBRDFTexture() {
		// create brdf lut texture if not exist
		const auto& as = g_engine.assetManager();
		if (!g_engine.fileSystem()->exists(BRDF_TEXTURE_URL)) {
			LOG_INFO("CREATING BRDF");
			std::shared_ptr<BRDFLUTPass> brdf_pass = std::make_shared<BRDFLUTPass>();
			brdf_pass->init();
			brdf_pass->render();
			brdf_pass->destroy();
		}
	}

	void SkyLightComponent::createIBLTextures() {
		createBRDFTexture();
		
		// create filter cubemap
		std::shared_ptr<FilterCubePass> filter_cube_pass = std::make_shared<FilterCubePass>(m_texture_cube);
		filter_cube_pass->init();
		filter_cube_pass->render();
		filter_cube_pass->destroy();

		m_irradiance_texture_sampler.destroy();
		m_prefilter_texture_sampler.destroy();

		m_cube_mesh = filter_cube_pass->getCubeMesh();
		m_irradiance_texture_sampler = filter_cube_pass->getIrradianceTextureSampler();
		m_prefilter_texture_sampler = filter_cube_pass->getPrefilterTextureSampler();
		m_prefilter_mip_levels = filter_cube_pass->getPrefilterMipLevels();
		m_brdf_lut_texture_sampler = g_engine.assetManager()->loadAsset<Texture2D>(BRDF_TEXTURE_URL)->m_image_view_sampler;
	}

}