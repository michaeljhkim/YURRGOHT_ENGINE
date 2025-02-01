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

    /*
    int main(int argc, char *argv[]) {
        // Get the current executable's directory
        // This sample assumes the managed assembly to load and its runtime configuration file are next to the host
        char_t host_path[MAX_PATH];
        auto resolved = realpath(argv[0], host_path);
        assert(resolved != nullptr);

        string_t root_path = host_path;
        auto pos = root_path.find_last_of(DIR_SEPARATOR);
        assert(pos != string_t::npos);
        root_path = root_path.substr(0, pos + 1);

        if (argc > 1 && string_compare(argv[1], STR("app")) == 0)
            return run_app_example();
    }
    */

    int run_app_example() {
        const string_t app_path = std::filesystem::absolute("cs_interface/cs_interface.dll").string();

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

        // Invoke the functions in a different thread from the main app
        std::thread t([&] {
            while (is_waiting() != 1)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

            for (int i = 0; i < 3; ++i)
                hello("from host!");
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