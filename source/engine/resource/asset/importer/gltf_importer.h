#pragma once

#include <tinygltf/tiny_gltf.h>
#include <cereal/access.hpp>
#include <cereal/cereal.hpp>

#include "import_option.h"
#include "engine/resource/asset/material.h"
#include "engine/resource/asset/static_mesh.h"
#include "engine/resource/asset/skeletal_mesh.h"
#include "engine/resource/asset/skeleton.h"
#include "engine/resource/asset/animation.h"

namespace Yurrgoht
{
	class GltfImporter
	{
	public:
		static VkFilter getVkFilterFromGltf(int gltf_filter);
		static VkSamplerAddressMode getVkAddressModeFromGltf(int gltf_wrap);
		static glm::mat4 getGltfNodeMatrix(const tinygltf::Node* node);
		static QTransform getGltfNodeTransform(const tinygltf::Node* node);
		static bool validateGltfMeshNode(const tinygltf::Node* node, const tinygltf::Model& gltf_model);
		static bool isGltfSkeletalMesh(const tinygltf::Mesh& gltf_mesh);
		static size_t findGltfJointNodeBoneIndex(const std::vector<std::pair<tinygltf::Node, int>>& joint_nodes, int node_index);
		static uint8_t topologizeGltfBones(std::vector<Bone>& bones, const std::vector<std::pair<tinygltf::Node, int>>& joint_nodes);

		static void importGltfTexture(const tinygltf::Model& gltf_model,
			const tinygltf::Image& gltf_image,
			const tinygltf::Sampler& gltf_sampler,
			uint32_t texture_index,
			std::shared_ptr<Texture2D>& texture);
		static void importGltfPrimitives(const tinygltf::Model& gltf_model,
			const std::vector<std::pair<tinygltf::Primitive, glm::mat4>>& primitives,
			const std::vector<std::shared_ptr<Material>>& materials,
			std::shared_ptr<StaticMesh>& static_mesh,
			std::shared_ptr<SkeletalMesh>& skeletal_mesh);

		static bool importGltf(const std::string& filename, const URL& folder, const GltfImportOption& option);

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& ar) {
			/*
			(maybe do in a different class?)
			Should contain the following:
			- Name of original asset (.glb, or .png, e.t.c) 
			- all paths to serialized assets of model/texture
			- when we need to export, we just open all of the assets and then make the appropriate file
			*/
			/*
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
			*/
		}
	};
}