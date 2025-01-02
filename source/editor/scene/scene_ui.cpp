#include "scene_ui.h"
#include "engine/core/event/event_system.h"
#include "engine/function/framework/scene/scene_manager.h"

namespace Yurrgoht
{

	void SceneUI::init() {
		m_title = "Scene";

		g_engine.eventSystem()->addListener(EEventType::SelectEntity, std::bind(&SceneUI::onSelectEntity, this, std::placeholders::_1));
		LOG_INFO("SUCCESS");
	}

	void SceneUI::construct()
	{
		sprintf(m_title_buf, "%s %s###%s", ICON_FA_GLOBE, m_title.c_str(), m_title.c_str());
		if (!ImGui::Begin(m_title_buf))
		{
			ImGui::End();
			return;
		}

		// get current active scene
		const auto& current_scene = g_engine.sceneManager()->getCurrentScene();

		// traverse all entities
		const float k_unindent_w = 16;
		ImGui::Unindent(k_unindent_w);
		const auto& entities = current_scene->getEntities();
		for (const auto& iter : entities)
		{
			const auto& entity = iter.second;
			if (entity->isRoot())
			{
				constructEntityTree(entity);
			}
		}
		
		ImGui::End();
	}

	void SceneUI::destroy()
	{
		EditorUI::destroy();

	}

	void SceneUI::constructEntityTree(const std::shared_ptr<class Entity>& entity)
	{
		uint32_t entity_id = entity->getID();

		ImGuiTreeNodeFlags tree_node_flags = 0;
		tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
		if (entity->isLeaf())
		{
			tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (entity_id == m_selected_entity_id)
		{
			tree_node_flags |= ImGuiTreeNodeFlags_Selected;
		}
		
		ImGui::TreeNodeEx(entity->getName().c_str(), tree_node_flags);
		if (m_selected_entity_id != entity_id && ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			g_engine.eventSystem()->syncDispatch(std::make_shared<SelectEntityEvent>(entity_id));
		}

		for (const auto& child : entity->getChildren())
		{
			constructEntityTree(child.lock());
		}

		ImGui::TreePop();
	}

	void SceneUI::onSelectEntity(const std::shared_ptr<class Event>& event)
	{
		const SelectEntityEvent* p_event = static_cast<const SelectEntityEvent*>(event.get());
		m_selected_entity_id = p_event->entity_id;
	}

}