#include "script_manager.h"
//#include <filesystem>

namespace Yurrgoht {
    
    void ScriptManager::init() {
        RunApplication();

        // Hello World
        std::cout << std::endl << "Hello World" << std::endl;

        return;
    }

    void ScriptManager::MessageCallback(const asSMessageInfo *msg, void *param) {
        const char *type = "ERR ";
        if( msg->type == asMSGTYPE_WARNING ) 
            type = "WARN";
        else if( msg->type == asMSGTYPE_INFORMATION ) 
            type = "INFO";

        printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
    }

    int ScriptManager::RunApplication() {
        int r;

        // Create the script engine
        asIScriptEngine *engine = asCreateScriptEngine();
        if( engine == 0 ) {
            std::cout << "Failed to create script engine." << std::endl;
            return -1;
        }

        // The script compiler will write any compiler messages to the callback.
        engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

        // Configure the script engine with all the functions, 
        // and variables that the script should be able to use.
        ConfigureEngine(engine);
        
        // Compile the script code
        r = CompileScript(engine);
        if( r < 0 ) {
            engine->Release();
            return -1;
        }

        // Create a context that will execute the script.
        asIScriptContext *ctx = engine->CreateContext();
        if( ctx == 0 ) {
            std::cout << "Failed to create the context." << std::endl;
            engine->Release();
            return -1;
        }

        // We don't want to allow the script to hang the application, e.g. with an
        // infinite loop, so we'll use the line callback function to set a timeout
        // that will abort the script after a certain time. Before executing the 
        // script the timeOut variable will be set to the time when the script must 
        // stop executing. 
        DWORD timeOut;
        r = ctx->SetLineCallback(asFUNCTION(LineCallback), &timeOut, asCALL_CDECL);
        if( r < 0 ) {
            std::cout << "Failed to set the line callback function." << std::endl;
            ctx->Release();
            engine->Release();
            return -1;
        }

        // Find the function for the function we want to execute.
        asIScriptFunction *func = engine->GetModule(0)->GetFunctionByDecl("void main()");
        if( func == 0 ) {
            std::cout << "The function 'void main()' was not found." << std::endl;
            ctx->Release();
            engine->Release();
            return -1;
        }

        /*
        - Prepare the script context with the function we wish to execute. 
        - Prepare() must be called on the context before each new script function that will be executed. 
        - Note, that if you intend to execute the same function several times, it might be a good idea to store the function returned by GetFunctionByDecl().
        - This will allow the relatively slow call to be skipped.
        */
        r = ctx->Prepare(func);
        if( r < 0 ) {
            std::cout << "Failed to prepare the context." << std::endl;
            ctx->Release();
            engine->Release();
            return -1;
        }

        // Set the timeout before executing the function. Give the function 1 sec to return before we'll abort it.
        timeOut = timeGetTime() + 1000;

        // Execute the function
        std::cout << "Executing the script." << std::endl;
        std::cout << "---" << std::endl;
        r = ctx->Execute();
        std::cout << "---" << std::endl;
        if( r != asEXECUTION_FINISHED ) {
            // The execution didn't finish as we had planned. Determine why.
            if( r == asEXECUTION_ABORTED )
                std::cout << "The script was aborted before it could finish. Probably it timed out." << std::endl;
            else if( r == asEXECUTION_EXCEPTION ) {
                std::cout << "The script ended with an exception." << std::endl;

                // Write some information about the script exception
                asIScriptFunction *func = ctx->GetExceptionFunction();
                std::cout << "func: " << func->GetDeclaration() << std::endl;
                std::cout << "modl: " << func->GetModuleName() << std::endl;
                std::cout << "sect: " << func->GetScriptSectionName() << std::endl;
                std::cout << "line: " << ctx->GetExceptionLineNumber() << std::endl;
                std::cout << "desc: " << ctx->GetExceptionString() << std::endl;
            }
            else
            std::cout << "The script ended for some unforeseen reason (" << r << ")." << std::endl;
        }
        else {
            std::cout << "The script finished successfully." << std::endl;
        }

        // We must release the contexts when no longer using them
        ctx->Release();

        // Shut down the engine
        engine->ShutDownAndRelease();

        return 0;
    }

    void ScriptManager::ConfigureEngine(asIScriptEngine *engine) {
        int r;

        /*
        - Register the script string type
        - Look at the implementation for this function for more information on how to register a custom string type, and other object types.
        */
        RegisterStdString(engine);

        if (!strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY")) {
            /*
            - Register the functions that the scripts will be allowed to use.
            - Note how the return code is validated with an assert().
            - This helps us discover where a problem occurs, and doesn't pollute the code with a lot of if's. 
            - If an error occurs in release mode it will be caught when a script is being built.
            - So it is not necessary to do the verification here as well.
            */
            r = engine->RegisterGlobalFunction("void print(string &in)", asFUNCTION(PrintString), asCALL_CDECL); assert( r >= 0 );
        }
        else {
            // Notice how the registration is almost identical to the above. 
            r = engine->RegisterGlobalFunction("void print(string &in)", asFUNCTION(PrintString_Generic), asCALL_GENERIC); assert( r >= 0 );
        }
        
        /*
        - It is possible to register the functions, properties, and types in configuration groups as well
        - When compiling the scripts it then be defined which configuration groups should be available for that script. 
        - If necessary a configuration group can also be removed from the engine, so that the engine configuration could be changed without having to recompile all the scripts.
        */
    }

    int ScriptManager::CompileScript(asIScriptEngine *engine) {
        int r;

        // The builder is a helper class that will load the script file, search for #include directives, and load any included files as well.
        CScriptBuilder builder;

        /*
        - Build the script. 
        - If there are any compiler messages they will be written to the message stream that we set right after creating the script engine. 
        - If there are no errors, and no warnings, nothing will be written to the stream.
        */
        r = builder.StartNewModule(engine, 0);
        if( r < 0 ) {
            std::cout << "Failed to start new module" << std::endl;
            return r;
        }

        // MODIFY THIS LINE TO FIND THE LOCATION OF THE SCRIPT
        r = builder.AddSectionFromFile("asset/engine/scripts/script.as");
        if( r < 0 ) {
            std::cout << "Failed to add script file" << std::endl;
            return r;
        }
        r = builder.BuildModule();
        if( r < 0 ) {
            std::cout << "Failed to build the module" << std::endl;
            return r;
        }
        
        /*
        - The engine doesn't keep a copy of the script sections after Build() has returned. 
        - So if the script needs to be recompiled, then all the script sections must be added again.
        - If we want to have several scripts executing at different times but that have no direct relation with each other, then we can compile them into separate script modules. 
        - Each module use their own namespace and scope, so function names, and global variables will not conflict with each other.
        */

        return 0;
    }

    void ScriptManager::LineCallback(asIScriptContext *ctx, DWORD *timeOut) {
        // If the time out is reached we abort the script
        if( *timeOut < timeGetTime() )
            ctx->Abort();

        /*
        - It would also be possible to only suspend the script, instead of aborting it. 
        - That would allow the application to resume the execution where it left of at a later time, by simply calling Execute() again.
        */
    }

    // Function implementation with native calling convention
    void ScriptManager::PrintString(std::string &str) {
        std::cout << str;
    }

    // Function implementation with generic script interface
    void ScriptManager::PrintString_Generic(asIScriptGeneric *gen) {
        std::string *str = (std::string*)gen->GetArgAddress(0);
        std::cout << *str;
    }
}