#include "skeleton.h"
#include <queue>

CEREAL_REGISTER_TYPE(Bamboo::Skeleton)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Bamboo::Asset, Bamboo::Skeleton)

namespace Bamboo
{

	void Skeleton::inflate()
	{
		for (size_t i = 0; i < m_bones.size(); ++i)
		{
			m_name_index[m_bones[i].m_name] = static_cast<uint8_t>(i);
		}
	}

	bool Skeleton::hasBone(const std::string& name)
	{
		return m_name_index.find(name) != m_name_index.end();
	}

	Bone* Skeleton::getBone(const std::string& name)
	{
		if (hasBone(name))
		{
			return &m_bones[m_name_index[name]];
		}
		return nullptr;
	}

	void Skeleton::update()
	{
		std::queue<std::pair<uint8_t, glm::mat4>> queue;
		queue.push(std::make_pair(m_root_bone_index, glm::mat4(1.0)));

		while (!queue.empty())
		{
			std::pair<uint8_t, glm::mat4> pair = queue.front();
			queue.pop();

			Bone& bone = m_bones[pair.first];
			bone.update(pair.second);

			for (uint8_t child_bone_index : bone.m_children)
			{
				queue.push(std::make_pair(child_bone_index, bone.m_global_bind_pose_matrix));
			}
		}
	}

}