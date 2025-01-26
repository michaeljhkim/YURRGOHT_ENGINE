#pragma once

#include "component.h"
#include "engine/resource/asset/animation.h"

namespace Yurrgoht {
	class AnimationComponent : public Component, public IAssetRef {
	public:
		void addAnimation(std::shared_ptr<Animation>& animation);
		const std::vector<std::shared_ptr<Animation>>& getAnimations() { return m_animations; }

		// base funtion is in Component.h
		void set_vector(std::size_t index, meta_hpp::uvalue sub_variant) override {
			*m_animations[index] = *sub_variant.try_as<Animation*>();
		}

	private:
		REGISTER_REFLECTION(Component)

		template<class Archive>
		void serialize(Archive& ar) {
			ar(cereal::make_nvp("component", cereal::base_class<Component>(this)));
			ar(cereal::make_nvp("asset_ref", cereal::base_class<IAssetRef>(this)));
		}

		virtual void bindRefs() override;

		std::vector<std::shared_ptr<Animation>> m_animations;
	};
}