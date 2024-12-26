#pragma once

#include <spdlog/fmt/fmt.h>

#include "component.h"
#include "engine/resource/asset/skeleton.h"
#include "engine/core/vulkan/vulkan_util.h"
#include "host_device.h"
#include "engine/resource/asset/animation.h"

namespace Yurrgoht
{
	class AnimatorComponent : public Component, public IAssetRef
	{
	public:
		AnimatorComponent();
		~AnimatorComponent();

		void setSkeleton(std::shared_ptr<Skeleton>& skeleton);
		std::shared_ptr<Skeleton> getSkeleton() { return m_skeleton; }

		void play(bool loop = true);

		std::vector<VmaBuffer> m_bone_ubs;

	protected:
		virtual void inflate() override;
		virtual void tick(float delta_time) override;

	private:
		REGISTER_REFLECTION(Component)

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("component", cereal::base_class<Component>(this)));
			ar(cereal::make_nvp("asset_ref", cereal::base_class<IAssetRef>(this)));
		}

		virtual void bindRefs() override;

		std::shared_ptr<Skeleton> m_skeleton;
		Skeleton m_skeleton_inst;

		std::shared_ptr<class AnimationComponent> m_animation_component;
		BoneUBO m_bone_ubo;

		float m_time = 0.0f;
		bool m_loop = true;
		bool m_playing = false;
		bool m_paused = false;
	};
}

template <>
struct fmt::formatter<Yurrgoht::AnimationChannel::EPathType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Yurrgoht::AnimationChannel::EPathType& type, FormatContext& ctx) const { // Add `const` here
        std::string name;
        switch (type) {
            case Yurrgoht::AnimationChannel::EPathType::Translation:
                name = "Translation";
                break;
            case Yurrgoht::AnimationChannel::EPathType::Rotation:
                name = "Rotation";
                break;
            case Yurrgoht::AnimationChannel::EPathType::Scale:
                name = "Scale";
                break;
            default:
                name = "Unknown";
        }
        return fmt::format_to(ctx.out(), "{}", name);
    }
};