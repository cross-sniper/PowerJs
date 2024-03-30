#include <iostream>
#include <fstream>
#include <string>
#include <duktape.h>
#include <dlfcn.h> // For dynamic loading of shared object files

// Function to read the content of a file into a string
std::string read(const char* name) {
    std::string str;
    std::ifstream file(name); // Open the file
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << name << std::endl;
        return "";
    }

    std::string line;
    while (std::getline(file, line)) { // Read file line by line
        // Process the line, for example, append it to the string
        str += line;
        str += "\n"; // Add newline character since std::getline consumes it
    }

    file.close(); // Close the file
    return str;
}

// Define a typedef for the function signature of the Duktape module open function
typedef void (*duk_module_open_function)(duk_context *);
// Function to handle print
static duk_ret_t print(duk_context *ctx) {
    std::cout << duk_safe_to_string(ctx, -1) << std::endl;
    return 0;
}


static duk_ret_t require(duk_context *ctx) {
    const char *module_name = duk_require_string(ctx, 0); // Get the module name from arguments
    std::string to_require;

    // Check if the module exists in the current directory
    to_require = "./" + std::string(module_name) + ".so";
    if (access(to_require.c_str(), F_OK) != -1) {
        // Load the shared object file dynamically
        void *handle = dlopen(to_require.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Error loading shared object file: " << dlerror() << std::endl;
            return 0; // Return 0 to indicate failure
        }

        // Construct the function name: dukopen_<module_name>
        std::string function_name = "dukopen_" + std::string(module_name);

        // Get the address of the Duktape module open function from the shared object
        duk_module_open_function module_open_function = (duk_module_open_function) dlsym(handle, function_name.c_str());
        if (!module_open_function) {
            std::cerr << "Error finding module open function: " << dlerror() << std::endl;
            dlclose(handle); // Close the handle
            return 0; // Return 0 to indicate failure
        }
        // Call the module open function to add functionality to the Duktape context/stack
        module_open_function(ctx);

        return 1; // Return 1 to indicate success
    }

    // Check if the module exists in ~/.mix/modules
    std::string home_directory = getenv("HOME");
    to_require = home_directory + "/.mix/modules/" + std::string(module_name) + ".so";
    if (access(to_require.c_str(), F_OK) != -1) {
        // Load the shared object file dynamically
        void *handle = dlopen(to_require.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Error loading shared object file: " << dlerror() << std::endl;
            return 0; // Return 0 to indicate failure
        }

        // Construct the function name: dukopen_<module_name>
        std::string function_name = "dukopen_" + std::string(module_name);

        // Get the address of the Duktape module open function from the shared object
        duk_module_open_function module_open_function = (duk_module_open_function) dlsym(handle, function_name.c_str());
        if (!module_open_function) {
            std::cerr << "Error finding module open function: " << dlerror() << std::endl;
            dlclose(handle); // Close the handle
            return 0; // Return 0 to indicate failure
        }
        // Call the module open function to add functionality to the Duktape context/stack
        module_open_function(ctx);

        return 1; // Return 1 to indicate success
    }

    std::cerr << "Module '" << module_name << "' not found." << std::endl;
    return 0; // Return 0 to indicate failure
}



int main(int argc, char const *argv[]) {
    if (argc < 2){
        printf("pass a file, %s <file>\n",argv[0]);
        exit(1);
    }
    int EXIT_CODE = 0;
    duk_context *context = duk_create_heap_default();
    if (!context) {
        std::cerr << "Error creating Duktape context." << std::endl;
        return 1;
    }

    // Push argv as a variable
    duk_push_object(context);
    duk_push_array(context);

    for (int i = 0; i < argc; ++i) {
        duk_push_string(context, argv[i]);
        duk_put_prop_index(context, -2, i);
    }
    duk_put_prop_string(context, -2 ,"args");
    duk_put_global_string(context, "mix");

    // Register the require function
    duk_push_c_function(context, require,1);
    duk_put_global_string(context, "require");
    // Register the print function
    duk_push_c_function(context, print, 1);
    duk_put_global_string(context, "print");
    // Read the content of the main JavaScript file
    std::string scriptContent = read(argv[1]);
    if (scriptContent.empty()) {
        std::cerr << "Error reading " << argv[1] << std::endl;
        duk_destroy_heap(context);
        return 1;
    }

    // Evaluate the main script
    int res = duk_peval_string(context, scriptContent.c_str());
    if (res != 0) {
        // Handle error
        std::cerr << "Error evaluating JavaScript: " << duk_safe_to_string(context, -1) << std::endl;
        EXIT_CODE = 1;
    }

    // Clean up
    duk_destroy_heap(context);
    return EXIT_CODE;
}
