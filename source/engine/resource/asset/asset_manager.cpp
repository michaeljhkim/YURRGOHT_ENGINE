#include <fstream>
#include <filesystem>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "importer/gltf_importer.h"
#include <tinygltf/stb_image.h>
#include <ktx.h>
#include <ktxvulkan.h>

#include "asset_manager.h"
#include "engine/resource/asset/texture_2d.h"
#include "engine/resource/asset/texture_cube.h"
#include "engine/function/framework/scene/scene.h"
#include "engine/function/framework/component/sky_light_component.h"


namespace Yurrgoht {

	void AssetManager::init() {
		/*
		.tx2d = texture 2d
		.txcube = texture cube
		.mat = material
		.skl = skeleton
		.stmesh = static mesh
		.skl = skeleton
		.stm = static mesh
		.skm = skeleton mesh
		.anim = animation
		.scene = scene (a bit obvious)
		*/
		m_asset_type_exts = {
			/*
			{ EAssetType::Texture2D, "tex" },
			{ EAssetType::TextureCube, "texc" }, 
			{ EAssetType::Material, "mat" }, 
			{ EAssetType::Skeleton, "skl" },
			{ EAssetType::StaticMesh, "sm"}, 
			{ EAssetType::SkeletalMesh, "skm" }, 
			{ EAssetType::Animation, "anim" },
			*/
			{ EAssetType::Texture2D, "tex2d" },
			{ EAssetType::TextureCube, "texcb" },
			{ EAssetType::Material, "mat" },
			{ EAssetType::Skeleton, "skl" },
			{ EAssetType::StaticMesh, "stm" },
			{ EAssetType::SkeletalMesh, "skm" },
			{ EAssetType::Animation, "anim" },
			{ EAssetType::Scene, "scene" }
		};

		m_asset_archive_types = {
			{ EAssetType::Texture2D, EArchiveType::Binary },
			{ EAssetType::TextureCube, EArchiveType::Binary },
			{ EAssetType::Material, EArchiveType::Json },
			{ EAssetType::Skeleton, EArchiveType::Binary },
			{ EAssetType::StaticMesh, EArchiveType::Binary },
			{ EAssetType::SkeletalMesh, EArchiveType::Binary },
			{ EAssetType::Animation, EArchiveType::Binary },
			{ EAssetType::Scene, EArchiveType::Json }
		};

		for (const auto& iter : m_asset_type_exts) {
			m_ext_asset_types[iter.second] = iter.first;
		}

		// create default BRDF Texture if it does not exist yet - will try to figure out better place to put the system in than SkyLightComponent
        SkyLightComponent::createBRDFTexture();

		// load default texture
		m_default_texture_2d = VulkanUtil::loadImageViewSampler(DEFAULT_TEXTURE_2D_FILE);
		LOG_INFO("SUCCESS");
		/*
		importTexture2D_KTX("asset/car/car_0.ktx", "asset/car/");
		importTexture2D_KTX("asset/car/car_0.ktx", "asset/car/");
		importTexture2D_KTX("asset/car/car_1.ktx", "asset/car/");
		importTexture2D_KTX("asset/car/car_2.ktx", "asset/car/");
		importTexture2D_KTX("asset/car/car_3.ktx", "asset/car/");
		importTexture2D_KTX("asset/engine/mesh/floor/floor_0.ktx", "asset/engine/mesh/floor/");
		importTexture2D_KTX("asset/engine/mesh/floor/floor_1.ktx", "asset/engine/mesh/floor/");
		importTexture2D_KTX("asset/engine/mesh/floor/floor_2.ktx", "asset/engine/mesh/floor/");
		importTexture2D_KTX("asset/engine/mesh/shader_ball/shader_ball_0.ktx", "asset/engine/mesh/shader_ball/");
		importTexture2D_KTX("asset/engine/mesh/shader_ball/shader_ball_1.ktx", "asset/engine/mesh/shader_ball/");
		importTexture2D_KTX("asset/engine/mesh/shader_ball/shader_ball_2.ktx", "asset/engine/mesh/shader_ball/");
		*/
		bool force_static_mesh = false;
		bool combine_meshes = true;
		bool contains_occlusion_channel = false;
		importGltf("asset/engine/mesh/primitive/cube.glb", "asset/engine/mesh/primitive/", { combine_meshes, force_static_mesh, contains_occlusion_channel });
	}

	void AssetManager::destroy() {
		for (auto& iter : m_assets) {
			iter.second.reset();
		}
		m_assets.clear();
		m_default_texture_2d.destroy();
	}

	bool AssetManager::importGltf(const std::string& filename, const URL& folder, const GltfImportOption& option) {
		return GltfImporter::importGltf(filename, folder, option);
	}

	bool AssetManager::importTexture2D(const std::string& filename, const URL& folder) {
		uint32_t width, height;
		uint32_t k_channels = 4;
		uint8_t* image_data = stbi_load(filename.c_str(), (int*)&width, (int*)&height, 0, k_channels);
		ASSERT(image_data != nullptr, "failed to import texture: {}", filename);

		std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
		std::string asset_name = getAssetName(filename, EAssetType::Texture2D);
		URL url = URL::combine(folder.str(), asset_name);
		texture->setURL(url);
		texture->m_width = width;
		texture->m_height = height;

		size_t image_size = width * height * k_channels;
		texture->m_image_data.resize(image_size);
		memcpy(texture->m_image_data.data(), image_data, image_size);
		stbi_image_free(image_data);

		texture->inflate();
		serializeAsset(texture);

		LOG_INFO("Imported - {}", filename);
		return true;
	}

	//figure out more direct way of importing ktx files, this is just a temp solution for now
	bool AssetManager::importTexture2D_KTX(const std::string& filename, const URL& folder) {
		ktxTexture* ktx_texture2D;
		ktxResult result = ktxTexture_CreateFromNamedFile(filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktx_texture2D);
		ASSERT(result == KTX_SUCCESS, "failed to import texture: {}", filename);

		std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
		std::string asset_name = getAssetName(filename, EAssetType::Texture2D);
		URL url = URL::combine(folder.str(), asset_name);
		texture->setURL(url);
		texture->m_width = ktx_texture2D->baseWidth;
		texture->m_height = ktx_texture2D->baseHeight;

		size_t image_size = ktxTexture_GetDataSize(ktx_texture2D);
		texture->m_image_data.resize(image_size);
		memcpy(texture->m_image_data.data(), ktxTexture_GetData(ktx_texture2D), image_size);

		texture->inflate_ktx(ktx_texture2D);
		serializeAsset(texture);

		LOG_INFO("Imported - {}", filename);
		return true;
	}


	bool AssetManager::importTextureCube(const std::string& filename, const URL& folder) {
		std::shared_ptr<TextureCube> texture_cube = std::make_shared<TextureCube>();
		std::string asset_name = getAssetName(filename, EAssetType::TextureCube);
		URL url = URL::combine(folder.str(), asset_name);
		texture_cube->setURL(url);

		texture_cube->setAddressMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
		texture_cube->m_texture_type = ETextureType::Cube;
		texture_cube->m_pixel_type = EPixelType::RGBA16;
		g_engine.fileSystem()->loadBinary(filename, texture_cube->m_image_data);

		texture_cube->inflate();
		serializeAsset(texture_cube);

		LOG_INFO("Imported - {}", filename);
		return true;
	}

	bool AssetManager::isGltfFile(const std::string& filename) {
		std::string extension = g_engine.fileSystem()->extension(filename);
		return extension == "glb" || extension == "gltf";
	}

	bool AssetManager::isTexture2DFile(const std::string& filename) {
		std::string extension = g_engine.fileSystem()->extension(filename);
		return extension == "png" || extension == "jpg";
	}

	bool AssetManager::isTextureCubeFile(const std::string& filename) {
		std::string extension = g_engine.fileSystem()->extension(filename);
		return extension == "ktx";
	}

	EAssetType AssetManager::getAssetType(const URL& url) {
		std::string extension = g_engine.fileSystem()->extension(url.str());
		if (m_ext_asset_types.find(extension) != m_ext_asset_types.end()) {
			return m_ext_asset_types[extension];
		}
		return EAssetType::Invalid;
	}

	void AssetManager::serializeAsset(std::shared_ptr<Asset> asset, const URL& url) {
		LOG_INFO("ASSET SERIALIZING: {}", url.getAbsolute());
		// reference asset
		EAssetType asset_type = asset->getAssetType();
		const std::string& asset_ext = m_asset_type_exts[asset_type];
		EArchiveType archive_type = m_asset_archive_types[asset_type];
		std::string filename = url.empty() ? asset->getURL().getAbsolute() : url.getAbsolute();

		switch (archive_type) {
			case EArchiveType::Json: {
				std::ofstream ofs(filename);
				cereal::JSONOutputArchive archive(ofs);
				archive(cereal::make_nvp(asset_ext.c_str(), asset));
				break;
			}
			case EArchiveType::Binary: {
				std::ofstream ofs(filename, std::ios::binary);
				cereal::BinaryOutputArchive archive(ofs);
				archive(cereal::make_nvp(asset_ext.c_str(), asset));
				break;
			}
			default:
				break;
		}

		// don't cache scene!
		if (asset_type != EAssetType::Scene) {
			m_assets[url] = asset;
		}
		LOG_INFO("SUCCESS: {}", url.getAbsolute());
	}

	std::shared_ptr<Asset> AssetManager::deserializeAsset(const URL& url) {
		LOG_INFO("ASSET DESERIALIZING: {}", url.getAbsolute());
		// check if the asset url exists
		if (!g_engine.fileSystem()->exists(url.str())) {
			return nullptr;
		}
		// check if the asset has been loaded
		if (m_assets.find(url) != m_assets.end()) {
			return m_assets[url];
		}

		EAssetType asset_type = getAssetType(url);
		EArchiveType archive_type = m_asset_archive_types[asset_type];
		const std::string& asset_ext = m_asset_type_exts[asset_type];
		std::string filename = url.getAbsolute();
		std::shared_ptr<Asset> asset = nullptr;
		if (archive_type == EArchiveType::Json) { LOG_INFO("ASSET TYPE: Json"); }
		else { LOG_INFO("ASSET TYPE: Binary"); }

		switch (archive_type) {
			case EArchiveType::Json: {
				std::ifstream ifs(filename);
				cereal::JSONInputArchive archive(ifs);
				archive(asset);
				LOG_INFO("This is the issue?");
				break;
			}
			case EArchiveType::Binary: {
				std::ifstream ifs(filename, std::ios::binary);
				cereal::BinaryInputArchive archive(ifs);
				archive(asset);
				break;
			}
			default:
				break;
		}

		asset->setURL(url);
		asset->inflate();

		// don't cache scene!
		if (asset_type != EAssetType::Scene) {
			m_assets[url] = asset;
		}

		LOG_INFO("SUCCESS: {}", url.getAbsolute());
		return asset;
	}

	std::string AssetManager::getAssetName(const std::string& asset_name, EAssetType asset_type, int asset_index, const std::string& basename) {
		const std::string& ext = m_asset_type_exts[asset_type];
		if (!asset_name.empty()) {
			std::string asset_basename = g_engine.fileSystem()->basename(asset_name);
			asset_basename = g_engine.fileSystem()->validateBasename(asset_basename);
			return g_engine.fileSystem()->format("%s_%s.%s", ext.c_str(), asset_basename.c_str(), ext.c_str());
		}
		return g_engine.fileSystem()->format("%s_%s_%d.%s", ext.c_str(), basename.c_str(), asset_index, ext.c_str());
	}
}
