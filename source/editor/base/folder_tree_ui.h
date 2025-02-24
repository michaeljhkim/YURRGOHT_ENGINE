#pragma once

#include <vector>
#include <map>
#include <string>
#include <functional>
#include <stdint.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include "fonts/IconsFontAwesome5.h"

namespace Yurrgoht {

	struct FolderNode {
		std::string name;
		std::string dir;
		std::vector<std::string> child_files;
		std::vector<uint32_t> child_folders;

		bool is_root;
		bool is_leaf;
	};

	struct HoverState {
		bool is_hovered;
		ImVec2 rect_min;
		ImVec2 rect_max;
	};

	class IFolderTreeUI {
	protected:
		void pollFolders();
		void constructFolderTree();

		virtual void openFolder(const std::string& folder);
		std::string createFolder();
		bool deleteFolder(const std::string& folder_name);
		bool rename(const std::string& filename, const ImVec2& size = { 0.0f, 0.0f });

		std::string m_selected_folder;
		std::vector<FolderNode> m_folder_nodes;
		bool show_engine_assets = true;
		bool is_folder_tree_hovered = false;
		bool is_renaming = false;

		char new_name_buffer[64] = "";

	private:
		void constructFolderTree(const std::vector<FolderNode>& folder_nodes, uint32_t index);

		std::map<std::string, bool> m_folder_opened_map;
	};
}