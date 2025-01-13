#include "skeletal_mesh.h"

CEREAL_REGISTER_TYPE(Yurrgoht::SkeletalMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Asset, Yurrgoht::SkeletalMesh)

namespace Yurrgoht {

	void SkeletalMesh::inflate() {
		calcBoundingBox();

		VulkanUtil::createVertexBuffer(m_vertices.size() * sizeof(m_vertices[0]), m_vertices.data(), m_vertex_buffer);
		VulkanUtil::createIndexBuffer(m_indices, m_index_buffer);
	}

	void SkeletalMesh::calcBoundingBox() {
		for (const auto& vertex : m_vertices) {
			m_bounding_box.combine(vertex.m_static_vertex.m_position);
		}
	}

}