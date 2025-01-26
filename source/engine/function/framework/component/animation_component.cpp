#include "animation_component.h"
#include "engine/function/global/engine_context.h"
#include "engine/resource/asset/asset_manager.h"


REGISTER_AT_RUNTIME {
meta_hpp::class_<Yurrgoht::AnimationComponent>(meta_hpp::metadata_()("name", "AnimationComponent"s))
	.member_("animations", &Yurrgoht::AnimationComponent::m_animations, meta_hpp::metadata_()("type_name", "std::vector<std::shared_ptr<Animation>>"s));
meta_hpp::extend_scope_(global_reflection_scope)
	.typedef_<Yurrgoht::AnimationComponent>("AnimationComponent");

//adding as derived_classes to Component metadata 
meta_hpp::class_<Yurrgoht::Component>(meta_hpp::metadata_()("derived_classes", "AnimationComponent"s));
}

CEREAL_REGISTER_TYPE(Yurrgoht::AnimationComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Component, Yurrgoht::AnimationComponent)

namespace Yurrgoht {
	void AnimationComponent::addAnimation(std::shared_ptr<Animation>& animation) {
		m_animations.push_back({});
		uint32_t index = m_animations.size() - 1;
		REF_ASSET_ELEM(m_animations[index], std::to_string(index), animation)
	}

	void AnimationComponent::bindRefs() {
		for (auto iter : m_ref_urls) {
			m_animations.push_back({});
			uint32_t index = m_animations.size() - 1;
			BIND_ASSET_ELEM(m_animations[index], std::to_string(index), Animation)
		}
	}

}