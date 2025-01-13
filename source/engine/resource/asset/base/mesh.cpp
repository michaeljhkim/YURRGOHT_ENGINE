#include "mesh.h"

namespace Yurrgoht {

	Mesh::~Mesh() {
		m_vertex_buffer.destroy();
		m_index_buffer.destroy();
	}

}