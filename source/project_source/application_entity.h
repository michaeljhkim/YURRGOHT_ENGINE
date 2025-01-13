#pragma once

#include "engine/function/framework/entity/entity.h"

namespace Yurrgoht
{
	class ApplicationEntity : public Entity
	{
	public:
		ApplicationEntity();

		float m_spawn_num;

	protected:
		virtual void beginPlay() override;
		virtual void tick(float delta_time) override;
		virtual void endPlay() override;

	private:
		REGISTER_REFLECTION(Entity)

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("entity", cereal::base_class<Entity>(this)));
			ar(cereal::make_nvp("spawn_num", m_spawn_num));
		}
	};
}