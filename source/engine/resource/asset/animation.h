#pragma once

#include "engine/resource/asset/base/asset.h"
#include <spdlog/fmt/fmt.h>


namespace Yurrgoht
{
	struct AnimationChannel
	{
		enum class EPathType 
		{ 
			Translation, Rotation, Scale 
		};

		EPathType m_path_type;
		std::string m_bone_name;
		uint32_t m_sampler_index;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("path_type", m_path_type));
			ar(cereal::make_nvp("bone_name", m_bone_name));
			ar(cereal::make_nvp("sampler_index", m_sampler_index));
		}
	};

	struct AnimationSampler
	{
		enum class EInterpolationType
		{
			Linear, Step, CubicSpline
		};

		EInterpolationType m_interp_type;
		std::vector<float> m_times;
		std::vector<glm::vec4> m_values;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("interp_type", m_interp_type));
			ar(cereal::make_nvp("times", m_times)); 
			ar(cereal::make_nvp("values", m_values));
		}
	};

	class Animation : public Asset
	{
	public:
		std::vector<AnimationSampler> m_samplers;
		std::vector<AnimationChannel> m_channels;

		float m_start_time;
		float m_end_time;
		float m_duration;

		virtual void inflate() override;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("name", m_name));
			ar(cereal::make_nvp("samplers", m_samplers)); 
			ar(cereal::make_nvp("channels", m_channels));
		}
	};
}


	
// Custom formatter for EPathType
template <>
struct fmt::formatter<Yurrgoht::AnimationChannel::EPathType> : fmt::formatter<std::string> {
	template <typename FormatContext>
	auto format(Yurrgoht::AnimationChannel::EPathType pathType, FormatContext& ctx) {
		std::string pathTypeStr;
		switch (pathType) {
			case Yurrgoht::AnimationChannel::EPathType::Translation:
				pathTypeStr = "Translation";
				break;
			case Yurrgoht::AnimationChannel::EPathType::Rotation:
				pathTypeStr = "Rotation";
				break;
			case Yurrgoht::AnimationChannel::EPathType::Scale:
				pathTypeStr = "Scale";
				break;
			default:
				pathTypeStr = "Unknown";
		}

		return fmt::formatter<std::string>::format(pathTypeStr, ctx);
	}
};
