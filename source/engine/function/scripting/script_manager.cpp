#include "script_manager.h"
//#include <filesystem>


/*
- EVERY {angelscript} CLASS NEEDS TO HAVE AN {_init} AND {_process} FUNCTION!!!
- MAYBE {_destroy} as well
- this will create a standardized system where the engine ONLY needs to look at these functions for value maniplulation
- probably should make default angelscript classes where all minimum default functions/values are defined
- then user inherits from these default classes

- lower priority, so work on it later, but perhaps I should make a game manager that controls events and such
- for like actually creating a story driven game with stuff happening

ANGELSCRIPT TEST PLANS:
- 1. create a simple class in angelscript, move the main and test function into it, and try calling it (maybe instantiating)
- 2. modify class from step 1 to inherit from another simple angelscript class
- 3. try calling types from the C++ code
- 4. try instantiating an entity type


NOTE: 
- using ASSERT() means that the ENTIRE engine will stop if there is an error with the scripting
- we use ASSERT() in the init() function because we want to make sure we can at least intialize the scripting system, otherwise there is a sever problem
- we use SCRIPT_ASSERT() for non-critical sections because that is a script based error, easy to fix without anything else breaking too hard
*/


namespace Yurrgoht {
    
    /*
    - Create the script engine
    - Configure the script engine with all the functions/variables that the script should use
    - Compile script
    - Create a context that will execute the script

    - Execute the function
    */
    void ScriptManager::init() {
        m_script_engine = asCreateScriptEngine();
        ASSERT( m_script_engine != 0, 
            "Failed to create script engine.");

        // Script compiler will write compiler messages to callback
        m_script_engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        ConfigureEngine();

        m_script_context = m_script_engine->CreateContext();
        ASSERT( m_script_context != 0, 
            "Failed to create the context.", m_script_engine->Release());

        // LineCallback function sets the timeOut value (prevents scripts from hanging)
        DWORD timeOut;
        ASSERT( m_script_context->SetLineCallback(asFUNCTION(LineCallback), &timeOut, asCALL_CDECL) >= 0, 
            "Failed to set the line callback function.", assert_destroy());

        // Set the timeout before executing the function. Give the function 1000ms (1 second) to return before we'll abort it
        timeOut = timeGetTime() + 1000;

        ASSERT( StartModuleIfAbsent("test_module") >= 0, 
            "Failed to start new module.");

        ASSERT( AddScriptToModule("test_module", "asset/engine/scripts/script.as") >= 0, 
            "Failed to add script.", m_script_engine->Release());

        ASSERT( BuildScriptModule("test_module") >= 0,
            "Failed to add script.", m_script_engine->Release());


        // Get the object type
        asIScriptModule *module = m_script_engine->GetModule("test_module");
        m_script_type = module->GetTypeInfoByDecl("TEST");
        
        // Get the factory function from the object type
        asIScriptFunction *factory = m_script_type->GetFactoryByDecl("TEST @TEST()");
        
        // Prepare the context to call the factory function
        // Execute the call
        m_script_context->Prepare(factory);
        m_script_context->Execute();
        
        // Get the object that was created
        // If you're going to store the object you must increase the reference, otherwise it will be destroyed when the context is reused or destroyed.
        m_script_object = *(asIScriptObject**)m_script_context->GetAddressOfReturnValue();
        m_script_object->AddRef();



        

        AddFunction("test_module", "void _init()");
        AddFunction("test_module", "void _process()");

        PrepareFunction("test_module", "void _init()");
        ExecuteFunction();
        PrepareFunction("test_module", "void _process()");
        ExecuteFunction();

        destroy();
    }

    void ScriptManager::destroy() {
        m_script_context->Release();
        m_script_engine->ShutDownAndRelease();
    }

    int ScriptManager::assert_destroy() {
        destroy();
        return 0;
    }

    void ScriptManager::MessageCallback(const asSMessageInfo *msg, void *param) {
        std::string type = "ERR ";
        if( msg->type == asMSGTYPE_WARNING ) 
            type = "WARN";
        else if( msg->type == asMSGTYPE_INFORMATION ) 
            type = "INFO";

        printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type.c_str(), msg->message);
    }

    
    /*  
    - Functions, properties, and types can be registered in configuration groups, which define available groups for scripts during compilation.
    - Groups can also be removed to change the engine configuration without recompiling scripts.
    - assert() validates return codes to catch errors during script building without extra checks in release mode.
    */
    void ScriptManager::ConfigureEngine() {
        // Register the script string type
        RegisterStdString(m_script_engine);

        // Register script functions
        int r;
        !strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") ?
            r = m_script_engine->RegisterGlobalFunction("void print(string &in)", asFUNCTION(PrintString), asCALL_CDECL) :
            r = m_script_engine->RegisterGlobalFunction("void print(string &in)", asFUNCTION(PrintString_Generic), asCALL_GENERIC);

        assert(r >= 0);
    }

	int ScriptManager::StartModuleIfAbsent(const std::string& module_name) {
        // we make sure that the module is not already built
        if( !m_script_builders.contains(module_name) ) {

            m_script_builders[module_name] = CScriptBuilder();
            int r = m_script_builders[module_name].StartNewModule(m_script_engine, module_name.c_str()); 
            
            if (r < 0) {
                LOG_ERROR("Failed to start new module");
                m_script_engine->Release();
                return r;
            }
        }
        return 0;
    }

    /*  
    - The engine discards script sections after Build(), so they must be re-added for recompilation.
    - For unrelated scripts, compile them into separate modules with their own namespaces and scopes to avoid conflicts.
    */
    int ScriptManager::AddScriptToModule(const std::string& module_name, const std::string& script_path) {
        int r = m_script_builders[module_name].AddSectionFromFile(script_path.c_str());

        // Build the script; compiler messages are written to the message stream if there are errors or warnings.
        if (r < 0) {
            LOG_ERROR("Failed to add script file");
            return r;
        }
        return 0;
    }

    int ScriptManager::BuildScriptModule(const std::string& module_name) {
        int r = m_script_builders[module_name].BuildModule();
        if (r < 0) {
            LOG_ERROR("Failed to build the module");
            return r;
        }
        return 0;
    }



    /*
    - Find the function for the function we want to execute.
    - Always call Prepare() before executing a script function
    - Store the returned value from GetFunctionByDecl() somewhere to avoid repeating this slow call
    - IDEA: store that value inside a map where the key is a string of the function name
    */
    int ScriptManager::AddFunction(const std::string& module, const std::string& function_name) {
        asIScriptFunction* function = m_script_type->GetMethodByDecl(function_name.c_str());
        if (function == 0) {
            LOG_ERROR("The function '{}' was not found.", function_name);
            destroy();
            return -1;
        }
        m_script_functions[function_name] = function;
        return 0;
    }

    int ScriptManager::PrepareFunction(const std::string& module, const std::string& function_name) {
        if (m_script_context->Prepare(m_script_functions[function_name]) < 0) {
            LOG_ERROR("Failed to prepare the context.");
            destroy();
            return -1;
        }
        m_script_context->SetObject(m_script_object);
        return 0;
    }


    void ScriptManager::ExecuteFunction() {
        std::cout << "---- Executing The Function ----\n" << std::endl;
        int r = m_script_context->Execute();

        // The execution didn't finish as we had planned. Determine why
        std::string exec_message = "\n---- The Function Finished Successfully ----";
        if (r != asEXECUTION_FINISHED) {
            if (r == asEXECUTION_ABORTED)
                exec_message = "\n---- The Function was aborted before it could finish. Probably it timed out ----";
            else if (r == asEXECUTION_EXCEPTION) {
                exec_message = "\n---- The Function ended with an exception ----";

                // Write some information about the function exception
                asIScriptFunction* func = m_script_context->GetExceptionFunction();
                std::cout << "function:  " << func->GetDeclaration()                     << std::endl;
                std::cout << "module:    " << func->GetModuleName()                      << std::endl;
                std::cout << "section:   " << func->GetScriptSectionName()               << std::endl;
                std::cout << "line:      " << m_script_context->GetExceptionLineNumber() << std::endl;
                std::cout << "exception: " << m_script_context->GetExceptionString()     << std::endl;
            }
            else
                exec_message = "\n---- The Function ended for some unforeseen reason (" + std::to_string(r) + "). ----";
        }
        std::cout << exec_message << std::endl;
    }

}