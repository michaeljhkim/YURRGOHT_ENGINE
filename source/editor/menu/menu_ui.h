#pragma once

#include "editor/base/editor_ui.h"
#include "editor/base/folder_tree_ui.h"

namespace Yurrgoht {
	
	class MenuUI : public EditorUI, public IFolderTreeUI {
	public:
		virtual void init() override;
		virtual void construct() override;
		virtual void destroy() override;

	private:
		void constructFileMenu();
		void constructEditMenu();
		void constructViewMenu();
		void constructHelpMenu();

		void pollShortcuts();

		void newScene();
		void openScene(); 
		void saveScene();
		void saveAsScene();
		void quit(); 

		void undo(); 
		void redo(); 
		void cut(); 
		void copy(); 
		void paste(); 

		void editorSettings(); 
		void projectSettings();

		void constructTemplateScenePanel();
		void constructSceneURLPanel();

		void clearEntitySelection();

		std::string m_layout_path;

		bool showing_new_scene_popup = false;
		bool showing_open_scene_popup = false;
		bool showing_save_as_scene_popup = false;

		// new scene
		struct TemplateScene {
			std::string name;
			std::string url;
			std::shared_ptr<ImGuiImage> icon;
		};
		std::vector<TemplateScene> m_template_scenes;
		std::map<std::string, HoverState> m_template_scene_hover_states;
		uint32_t m_selected_template_scene_index;

		// open scene
		std::vector<std::string> m_current_scene_urls;
		std::string m_selected_scene_url;
	};
}