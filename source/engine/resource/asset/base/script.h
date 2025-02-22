#pragma once

#include "asset.h"
#include "engine/function/scripting/script_manager.h"

namespace Yurrgoht {

	class Script : public Asset, public IAssetRef {
	public:
		virtual void inflate() override;
		
		void setScriptPath(const std::string& script_path) 	 { m_script_path = script_path; }
		void setModuleName(const std::string& module_name) 	 { m_module_name = module_name; }
		void setScriptTypename(const std::string& type_name) { m_typename = type_name; 		}

	protected:
		// to serialize values
		std::string m_module_name;
		std::string m_script_path;
		std::string m_typename;
		std::string m_factory_name;
		
		asIScriptModule* m_module;
		asITypeInfo* m_script_type;
		asIScriptFunction* m_factory;
		asIScriptObject* m_script_object;

		std::map<std::string, asIScriptFunction*> m_script_functions;

		// not sure what to do with this, but I know I'll need something like this one day
		std::shared_ptr<Script> m_scripts;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& ar) {
			ar(cereal::make_nvp("asset_ref", cereal::base_class<IAssetRef>(this)));
			ar(cereal::make_nvp("module_name", m_module_name));
			ar(cereal::make_nvp("script_path", m_script_path));
			ar(cereal::make_nvp("typename", m_typename));
			ar(cereal::make_nvp("factory_name", m_factory_name));
		}

		virtual void bindRefs() override;
	};
}