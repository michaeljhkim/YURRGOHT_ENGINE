#include "texture_cube.h"
#include <ktx.h>
#include <ktxvulkan.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tinygltf/stb_image_write.h>

CEREAL_REGISTER_TYPE(Yurrgoht::TextureCube)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Asset, Yurrgoht::TextureCube)

namespace Yurrgoht {
	void TextureCube::inflate() {	
		ktxTexture* ktx_texture;
		m_layers = 6;

		// create texture cube from ktx image data
		ktxResult result = ktxTexture_CreateFromMemory(m_image_data.data(), m_image_data.size(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktx_texture);
		ASSERT(result == KTX_SUCCESS, "failed to inflate texture cube");

		m_width = ktx_texture->baseWidth;
		m_height = ktx_texture->baseHeight;
		m_mip_levels = ktx_texture->numLevels;
		m_min_filter = m_mag_filter = VK_FILTER_LINEAR;;
		m_address_mode_u = m_address_mode_v = m_address_mode_w = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		//std::string texture_name = m_name + "_Texture2D.ktx";
		//ktxTexture_WriteToNamedFile(ktx_texture, texture_name.c_str());

		uploadKtxTexture(ktx_texture, ktxTexture_GetVkFormat(ktx_texture));
	}
}