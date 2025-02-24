#pragma once

#include "engine/core/vulkan/vulkan_util.h"
#include <cereal/access.hpp>
#include <cereal/cereal.hpp>

enum class ETextureType {
	BaseColor, MetallicRoughnessOcclusion, Normal, Emissive, Cube, UI, Data
};

enum class EPixelType {
	RGBA8, RGBA16, RGBA32, RG16, R16, R32
};


namespace Yurrgoht {

	class Texture {
	public:
		Texture();
		virtual ~Texture();

		uint32_t m_width, m_height;
		VkFilter m_min_filter, m_mag_filter;
		VkSamplerAddressMode m_address_mode_u, m_address_mode_v, m_address_mode_w;
		ETextureType m_texture_type;
		EPixelType m_pixel_type;

		uint32_t m_mip_levels;
		uint32_t m_layers;
		VmaImageViewSampler m_image_view_sampler;

		std::vector<uint8_t> m_image_data;

		void setAddressMode(VkSamplerAddressMode address_mode);

	protected:
		bool isSRGB();
		bool isMipmap();
		VkFormat getFormat();
		void uploadKtxTexture(void* p_ktx_texture, VkFormat format = VK_FORMAT_UNDEFINED);

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& ar) {
			ar(cereal::make_nvp("width", m_width));
			ar(cereal::make_nvp("height", m_height));
			ar(cereal::make_nvp("min_filter", m_min_filter));
			ar(cereal::make_nvp("mag_filter", m_mag_filter));
			ar(cereal::make_nvp("address_mode_u", m_address_mode_u));
			ar(cereal::make_nvp("address_mode_v", m_address_mode_v));
			ar(cereal::make_nvp("address_mode_w", m_address_mode_w));
			ar(cereal::make_nvp("texture_type", m_texture_type));
			ar(cereal::make_nvp("pixel_type", m_pixel_type));
			ar(cereal::make_nvp("image_data", m_image_data));
		}
	};
}


// Specialize std::formatter for EPixelType
template <>
struct std::formatter<EPixelType> : std::formatter<std::string> {
    // Format function to convert enum to string
    template <typename FormatContext>
    auto format(EPixelType pt, FormatContext& ctx) const {
        std::string_view str;
        switch (pt) {
            case EPixelType::RGBA8: str = "RGBA8"; break;
            case EPixelType::RGBA16: str = "RGBA16"; break;
            case EPixelType::RGBA32: str = "RGBA32"; break;
            case EPixelType::RG16: str = "RG16"; break;
            case EPixelType::R16: str = "R16"; break;
            case EPixelType::R32: str = "R32"; break;
            default: str = "Unknown"; break;
        }
        return std::formatter<std::string>::format(std::string(str), ctx);
    }
};