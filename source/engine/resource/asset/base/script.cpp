#include "script.h"
#include "engine/function/global/engine_context.h"
#include "engine/resource/asset/asset_manager.h"

CEREAL_REGISTER_TYPE(Yurrgoht::Script)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Yurrgoht::Asset, Yurrgoht::Script)

namespace Yurrgoht {

	void Script::inflate() {
		// setup script module
		g_engine.scriptManager()->StartModuleIfAbsent(m_module_name);
		g_engine.scriptManager()->AddScriptToModule(m_module_name.c_str(), m_script_path.c_str());
		g_engine.scriptManager()->BuildScriptModule(m_module_name);

        // Get the object type
		m_module = g_engine.scriptManager()->getScriptEngine()->GetModule(m_module_name.c_str());
        m_script_type = m_module->GetTypeInfoByDecl(m_typename.c_str());

		// all factory function names will be roughly the same
		m_factory_name = m_typename + " @" + m_typename + "()";

        // Get the factory function from the object type
        m_factory = m_script_type->GetFactoryByDecl(m_factory_name.c_str());

        // Get the object that was created
        // If you're going to store the object you must increase the reference, otherwise it will be destroyed when the context is reused or destroyed.
        m_script_object = *(asIScriptObject**)g_engine.scriptManager()->getScriptContext()->GetAddressOfReturnValue();
        m_script_object->AddRef();
	}

	void Script::bindRefs() {
		BIND_ASSET(m_scripts, Script)
	}
}