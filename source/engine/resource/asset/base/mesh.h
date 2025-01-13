#pragma once

#include "engine/resource/asset/base/sub_mesh.h"
#include "host_device.h"

struct StaticVertex {
	glm::vec3 m_position;
	glm::vec2 m_tex_coord;
	glm::vec3 m_normal;
private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& ar) {
		ar(cereal::make_nvp("position", m_position));
		ar(cereal::make_nvp("tex_coord", m_tex_coord));
		ar(cereal::make_nvp("normal", m_normal));
	}
};

struct SkeletalVertex {
    StaticVertex m_static_vertex; // Avoid inheritance, make it a member
    glm::ivec4 m_bones;   // 16 bytes
    glm::vec4 m_weights;  // 16 bytes
private:
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& ar) {
        ar(cereal::make_nvp("static_vertex", m_static_vertex)); // Serialize the StaticVertex member
        ar(cereal::make_nvp("bones", m_bones));
        ar(cereal::make_nvp("weights", m_weights));
    }
};

namespace Yurrgoht {

	class Mesh {
	public:
		virtual ~Mesh();

		std::vector<SubMesh> m_sub_meshes;
		std::vector<uint32_t> m_indices;

		VmaBuffer m_vertex_buffer;
		VmaBuffer m_index_buffer;
		
		BoundingBox m_bounding_box;

	protected:
		virtual void calcBoundingBox() = 0;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& ar) {
			ar(cereal::make_nvp("sub_meshes", m_sub_meshes));
			ar(cereal::make_nvp("indices", m_indices));
		}
	};
}