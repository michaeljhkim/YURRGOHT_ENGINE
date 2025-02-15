#pragma once

#include "engine/core/vulkan/vulkan_util.h"
#include <map>
#include <fstream>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include "TBuiltInResource_default.h"

namespace Yurrgoht {
	
	/*
	- Took DirStackFileIncluder from the official glslangValidator code: 
		https://github.com/KhronosGroup/glslang/blob/00ac5651f528d84cc3cf7320b34407bce7d38d93/StandAlone/StandAlone.cpp
	
	- I have no idea why this isn't the default, but IT SHOULD BE
	- basic premise is that it handles #include parameter so long as the include directories are inputed
	*/
	
	// Default include class for normal include convention of search backward through the stack of active include paths (for nested includes).
	// Can be overridden to customize.
	class DirStackFileIncluder : public glslang::TShader::Includer {
	public:
		DirStackFileIncluder() : externalLocalDirectoryCount(0) { }
	
		virtual IncludeResult* includeLocal(const char* headerName,
											const char* includerName,
											size_t inclusionDepth) override
		{
			return readLocalPath(headerName, includerName, (int)inclusionDepth);
		}
	
		virtual IncludeResult* includeSystem(const char* headerName,
											 const char* /*includerName*/,
											 size_t /*inclusionDepth*/) override
		{
			return readSystemPath(headerName);
		}
	
		// Externally set directories. E.g., from a command-line -I<dir>.
		//  - Most-recently pushed are checked first.
		//  - All these are checked after the parse-time stack of local directories
		//    is checked.
		//  - This only applies to the "local" form of #include.
		//  - Makes its own copy of the path.
		virtual void pushExternalLocalDirectory(const std::string& dir) {
			directoryStack.push_back(dir);
			externalLocalDirectoryCount = (int)directoryStack.size();
		}
	
		virtual void releaseInclude(IncludeResult* result) override {
			if (result != nullptr) {
				delete [] static_cast<tUserDataElement*>(result->userData);
				delete result;
			}
		}
	
		virtual std::set<std::string> getIncludedFiles() {
			return includedFiles;
		}
	
		virtual ~DirStackFileIncluder() override { }
	
	protected:
		typedef char tUserDataElement;
		std::vector<std::string> directoryStack;
		int externalLocalDirectoryCount;
		std::set<std::string> includedFiles;
	
		// Search for a valid "local" path based on combining the stack of include directories and the nominal name of the header.
		virtual IncludeResult* readLocalPath(const char* headerName, const char* includerName, int depth) {
			// Discard popped include directories, and initialize when at parse-time first level.
			directoryStack.resize(depth + externalLocalDirectoryCount);
			if (depth == 1)
				directoryStack.back() = getDirectory(includerName);
	
			// Find a directory that works, using a reverse search of the include stack.
			for (auto it = directoryStack.rbegin(); it != directoryStack.rend(); ++it) {
				std::string path = *it + '/' + headerName;
				std::replace(path.begin(), path.end(), '\\', '/');
				std::ifstream file(path, std::ios_base::binary | std::ios_base::ate);
				if (file) {
					directoryStack.push_back(getDirectory(path));
					includedFiles.insert(path);
					return newIncludeResult(path, file, (int)file.tellg());
				}
			}
	
			return nullptr;
		}
	
		// Search for a valid <system> path.
		// Not implemented yet; returning nullptr signals failure to find.
		virtual IncludeResult* readSystemPath(const char* /*headerName*/) const {
			return nullptr;
		}
	
		// Do actual reading of the file, filling in a new include result.
		virtual IncludeResult* newIncludeResult(const std::string& path, std::ifstream& file, int length) const {
			char* content = new tUserDataElement [length];
			file.seekg(0, file.beg);
			file.read(content, length);
			return new IncludeResult(path, content, length, content);
		}
	
		// If no path markers, return current working directory.
		// Otherwise, strip file name and return path leading up to it.
		virtual std::string getDirectory(const std::string path) const {
			size_t last = path.find_last_of("/\\");
			return last == std::string::npos ? "." : path.substr(0, last);
		}
	};


	class ShaderManager {
	public:
		void init();
		void destroy();

		VkPipelineShaderStageCreateInfo getShaderStageCI(const std::string& name, VkShaderStageFlagBits stage);

	private:
		std::string execute(const char* cmd);

		EShLanguage GetShaderStage(const std::string& filename);
		std::string ReadFile(const std::string& filename);

		std::map<std::string, VkShaderModule> m_shader_modules;
		std::map<std::string, std::string> m_shader_filenames;
	};
}