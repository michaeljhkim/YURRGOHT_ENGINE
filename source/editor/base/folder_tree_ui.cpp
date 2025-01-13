#include "folder_tree_ui.h"
#include "engine/core/base/macro.h"
#include "engine/resource/asset/asset_manager.h"

#include <queue>

namespace Yurrgoht {

	void IFolderTreeUI::pollFolders() {
		// Recursively traverse the root folder using a queue
		const auto& fs = g_engine.fileSystem();

		m_folder_nodes.clear();
		std::queue<std::string> folder_queue;
		folder_queue.push(fs->getAssetDir());

		while (!folder_queue.empty()) {
			size_t offset = m_folder_nodes.size(); // Starting index of current batch
			size_t current_batch_size = folder_queue.size();
			m_folder_nodes.resize(offset + current_batch_size); // Preallocate nodes for the current batch

			uint32_t child_offset = m_folder_nodes.size(); // Offset for child nodes
			for (size_t i = 0; i < current_batch_size; ++i) {
				const std::string& current_folder = folder_queue.front();
				FolderNode& folder_node = m_folder_nodes[offset + i];
				// Populate folder node
				folder_node.dir = current_folder;
				folder_node.name = fs->basename(current_folder);
				folder_node.is_root = (current_folder == fs->getAssetDir());

				// Process directory contents
				for (const auto& file : std::filesystem::directory_iterator(current_folder)) {
					std::string filename = file.path().generic_string();
					if (file.is_regular_file()) {	// Add valid assets only
						if (g_engine.assetManager()->getAssetType(filename) != EAssetType::Invalid) {
							folder_node.child_files.push_back(std::move(filename));
						}
					} else if (file.is_directory()) {
						bool is_engine_folder = (filename.find("asset/engine") != std::string::npos || filename.find("asset\\engine") != std::string::npos);
						// Add directories based on `show_engine_assets` option
						if (show_engine_assets || !is_engine_folder) {
							folder_node.child_folders.push_back(child_offset++);
							folder_queue.push(std::move(filename));
						}
					}
				}
				folder_node.is_leaf = folder_node.child_folders.empty();
				folder_queue.pop();
			}
		}

		// Update folder opened status
		for (const FolderNode& folder_node : m_folder_nodes) {
			m_folder_opened_map.try_emplace(folder_node.name, false);
		}

		// Update selected folder's files
		openFolder("");
	}

	void IFolderTreeUI::constructFolderTree() {
		if (!m_folder_nodes.empty()) {
			constructFolderTree(m_folder_nodes, 0);
		}
	}

	void IFolderTreeUI::constructFolderTree(const std::vector<FolderNode>& folder_nodes, uint32_t index) {
		const FolderNode& folder_node = folder_nodes[index];

		ImGuiTreeNodeFlags tree_node_flags = 0;
		tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (folder_node.is_root) {
			tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}
		else if (folder_node.is_leaf) {
			tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (folder_node.dir == m_selected_folder) {
			tree_node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool is_treenode_opened = ImGui::TreeNodeEx((void*)(intptr_t)index, tree_node_flags, "%s %s", m_folder_opened_map[folder_node.name] ? ICON_FA_FOLDER_OPEN : ICON_FA_FOLDER, folder_node.name.c_str());
		m_folder_opened_map[folder_node.name] = is_treenode_opened && !folder_node.is_leaf;
		if ((ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right)) && !ImGui::IsItemToggledOpen()) {
			openFolder(folder_node.dir);
		}

		if (ImGui::IsItemHovered()) {
			is_folder_tree_hovered |= true;
		}

		if (is_treenode_opened) {
			if (!folder_node.child_folders.empty()) {
				const float k_unindent_w = 8;
				ImGui::Unindent(k_unindent_w);

				for (uint32_t child_folder : folder_node.child_folders) {
					constructFolderTree(folder_nodes, child_folder);
				}
			}

			ImGui::TreePop();
		}
	}

	void IFolderTreeUI::openFolder(const std::string& folder) {
		if (!folder.empty()) {
			m_selected_folder = g_engine.fileSystem()->relative(folder);
		}
	}

	std::string IFolderTreeUI::createFolder() {
		LOG_INFO("create folder");
		std::string new_folder_name = m_selected_folder + "/NewFolder";

		int index = 1;
		while (!g_engine.fileSystem()->createDir(new_folder_name)) {
			new_folder_name = m_selected_folder + "/NewFolder_" + std::to_string(index++);
		}
		pollFolders();
		return new_folder_name;
	}

	bool IFolderTreeUI::deleteFolder(const std::string& folder_name) {
		LOG_INFO("delete folder: {}", folder_name);
		g_engine.fileSystem()->removeDir(folder_name, true);
		pollFolders();
		return true;
	}

	bool IFolderTreeUI::rename(const std::string& filename, const ImVec2& size) {
		std::string basename = g_engine.fileSystem()->basename(filename);
		std::string dir = g_engine.fileSystem()->dir(filename) + "/";
		ImGui::PushItemWidth(size.x);
		strcpy(new_name_buffer, basename.c_str());

		ImGui::SetKeyboardFocusHere();
		ImGui::InputText("##NewName", new_name_buffer, IM_ARRAYSIZE(new_name_buffer), ImGuiInputTextFlags_AutoSelectAll);
		ImGui::PopItemWidth();

		// if press enter or not focus, exit renaming status, else maintain renaming status
		if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter) 
			|| (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)))
		{
			g_engine.fileSystem()->renameFile(dir, basename, new_name_buffer);
			pollFolders();
			return false;
		}
		return true;
	}

}