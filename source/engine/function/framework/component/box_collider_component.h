#pragma once

#include "collider_component.h"
#include "engine/function/framework/entity/entity.h"

namespace Yurrgoht {
	class BoxColliderComponent : public ColliderComponent {
	public:
		BoxColliderComponent();
		glm::vec3 m_size;

		//void setNewSize() {}

	private:
		REGISTER_REFLECTION(ColliderComponent)

		template<class Archive>
		void serialize(Archive& ar) {
			ar(cereal::make_nvp("collider_component", cereal::base_class<ColliderComponent>(this)));
			ar(cereal::make_nvp("size", m_size));
		}
	};
}