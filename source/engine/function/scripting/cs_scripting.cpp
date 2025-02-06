#include "cs_scripting.h"
#include <filesystem>



namespace Yurrgoht
{
    // Globals to hold hostfxr exports
    hostfxr_initialize_for_dotnet_command_line_fn init_for_cmd_line_fptr;
    hostfxr_initialize_for_runtime_config_fn init_for_config_fptr;
    hostfxr_get_runtime_delegate_fn get_delegate_fptr;
    hostfxr_run_app_fn run_app_fptr;
    hostfxr_close_fn close_fptr;

    // called in engine_context init - not sure if I should move this whole thing, but I am considering it.
    int run_app_example() {
        const string_t app_path = std::filesystem::absolute("scripting/cs_interface/cs_interface.dll").string();
        const char* script_path = "scripting/test_script.cs";

        if ( !load_hostfxr(app_path.c_str()) ) {
            assert(false && "Failure: load_hostfxr()");
            return EXIT_FAILURE;
        }

        // Load .NET Core
        hostfxr_handle cxt = nullptr;
        std::vector<const char_t*> args { app_path.c_str(), STR("app_arg_1"), STR("app_arg_2") };
        int rc = init_for_cmd_line_fptr(args.size(), args.data(), nullptr, &cxt);
        if (rc != 0 || cxt == nullptr) {
            std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
            close_fptr(cxt);
            return EXIT_FAILURE;
        }

        // Get the function pointer to get function pointers
        get_function_pointer_fn get_function_pointer;
        rc = get_delegate_fptr(
            cxt,
            hdt_get_function_pointer,
            (void**)&get_function_pointer);
        if (rc != 0 || get_function_pointer == nullptr)
            std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;

        // Function pointer to Program.IsWaiting
        typedef unsigned char (CORECLR_DELEGATE_CALLTYPE* is_waiting_fn)();
        is_waiting_fn is_waiting;
        rc = get_function_pointer(
            STR("Program, cs_interface"),
            STR("IsWaiting"),
            UNMANAGEDCALLERSONLY_METHOD,
            nullptr, nullptr, (void**)&is_waiting);
        assert(rc == 0 && (is_waiting != nullptr) && "Failure: get_function_pointer()");

        // Function pointer to Program.Hello
        typedef void (CORECLR_DELEGATE_CALLTYPE* hello_fn)(const char*);
        hello_fn hello;
        rc = get_function_pointer(
            STR("Program, cs_interface"),
            STR("Hello"),
            UNMANAGEDCALLERSONLY_METHOD,
            nullptr, nullptr, (void**)&hello);
        assert(rc == 0 && hello != nullptr && "Failure: get_function_pointer()");


        // Function pointer to Program cs-script loader
        typedef void (CORECLR_DELEGATE_CALLTYPE* load_file_fn)(const char*);
        load_file_fn load_file;
        rc = get_function_pointer(
            STR("Program, cs_interface"),
            STR("LoadFile"),
            UNMANAGEDCALLERSONLY_METHOD,
            nullptr, nullptr, (void**)&load_file);
        assert(rc == 0 && load_file != nullptr && "Failure: get_function_pointer()");


        // Invoke the functions in a different thread from the main app
        // Note: the actual C# code requires the hello function to be run at least 3 times, otherwise it hangs
        std::thread t([&] {
            while (is_waiting() != 1)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

            for (int i = 0; i < 3; ++i)
                hello("from host!");
            
            load_file( script_path );
        });

        // Run the app
        run_app_fptr(cxt);
        t.join();

        close_fptr(cxt);
        return EXIT_SUCCESS;
    }



    void *load_library(const char_t *path) {
        void *h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
        assert(h != nullptr);
        return h;
    }
    void *get_export(void *h, const char *name) {
        void *f = dlsym(h, name);
        assert(f != nullptr);
        return f;
    }

    // <SnippetLoadHostFxr>
    // Using the nethost library, discover the location of hostfxr and get exports
    bool load_hostfxr(const char_t *assembly_path) {
        get_hostfxr_parameters params { sizeof(get_hostfxr_parameters), assembly_path, nullptr };
        // Pre-allocate a large buffer for the path to hostfxr
        char_t buffer[MAX_PATH];
        size_t buffer_size = sizeof(buffer) / sizeof(char_t);
        int rc = get_hostfxr_path(buffer, &buffer_size, &params);
        if (rc != 0)
            return false;

        // Load hostfxr and get desired exports
        // NOTE: The .NET Runtime does not support unloading any of its native libraries. Running
        // dlclose/FreeLibrary on any .NET libraries produces undefined behavior.
        void *lib = load_library(buffer);
        init_for_cmd_line_fptr = (hostfxr_initialize_for_dotnet_command_line_fn)get_export(lib, "hostfxr_initialize_for_dotnet_command_line");
        init_for_config_fptr = (hostfxr_initialize_for_runtime_config_fn)get_export(lib, "hostfxr_initialize_for_runtime_config");
        get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)get_export(lib, "hostfxr_get_runtime_delegate");
        run_app_fptr = (hostfxr_run_app_fn)get_export(lib, "hostfxr_run_app");
        close_fptr = (hostfxr_close_fn)get_export(lib, "hostfxr_close");

        return (init_for_config_fptr && get_delegate_fptr && close_fptr);
    }
    // </SnippetLoadHostFxr>

    // <SnippetInitialize>
    // Load and initialize .NET Core and get desired function pointer for scenario
    load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *config_path) {
        // Load .NET Core
        void *load_assembly_and_get_function_pointer = nullptr;
        hostfxr_handle cxt = nullptr;
        int rc = init_for_config_fptr(config_path, nullptr, &cxt);
        if (rc != 0 || cxt == nullptr) {
            std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
            close_fptr(cxt);
            return nullptr;
        }

        // Get the load assembly function pointer
        rc = get_delegate_fptr(
            cxt,
            hdt_load_assembly_and_get_function_pointer,
            &load_assembly_and_get_function_pointer);
        if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
            std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;

        close_fptr(cxt);
        return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
    }
    // </SnippetInitialize>
}