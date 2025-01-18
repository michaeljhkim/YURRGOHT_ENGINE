#pragma once

#include "component.h"
#include "engine/resource/asset/static_mesh.h"

namespace Yurrgoht {

	class StaticMeshComponent : public Component, public IAssetRef {
	public:
		void setStaticMesh(std::shared_ptr<StaticMesh>& static_mesh);
		std::shared_ptr<StaticMesh> getStaticMesh() { return m_static_mesh; }

	private:
		REGISTER_REFLECTION(Component, IAssetRef)

		template<class Archive>
		void serialize(Archive& ar) {
			ar(cereal::make_nvp("component", cereal::base_class<Component>(this)));
			ar(cereal::make_nvp("asset_ref", cereal::base_class<IAssetRef>(this)));
		}

		virtual void bindRefs() override;

		std::shared_ptr<StaticMesh> m_static_mesh;
	};
}