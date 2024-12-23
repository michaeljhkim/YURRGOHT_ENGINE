#include "animation.h"
#include <limits>

/*
RTTR_REGISTRATION
{
rttr::registration::enumeration<Yurrgoht::AnimationChannel::EPathType>("EPathType") 
	(
		rttr::value("Translation", Yurrgoht::AnimationChannel::EPathType::Translation),
		rttr::value("Rotation", Yurrgoht::AnimationChannel::EPathType::Rotation),
		rttr::value("Scale", Yurrgoht::AnimationChannel::EPathType::Scale)
	);

rttr::registration::class_<Yurrgoht::AnimationChannel>("AnimationChannel")
	 .property("path_type", &Yurrgoht::AnimationChannel::m_path_type);
}
*/



CEREAL_REGISTER_TYPE(Yurrgoht::Animation)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Asset, Yurrgoht::Animation)

namespace Yurrgoht
{
	void Animation::inflate()
	{
		m_start_time = std::numeric_limits<float>::max();
		m_end_time = std::numeric_limits<float>::min();

		for (const AnimationSampler& sampler : m_samplers)
		{
			for (float time : sampler.m_times)
			{
				if (time < m_start_time)
				{
					m_start_time = time;
				}
				if (time > m_end_time)
				{
					m_end_time = time;
				}
			}
		}
		m_duration = m_end_time - m_start_time;
	}

}