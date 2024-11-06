#include "inc.hh"
#include "wren.hpp"

static void loadModuleComplete(WrenVM *vm, const char *module, WrenLoadModuleResult result) {
    UNUSED(vm);     // Only got one vm :)
    UNUSED(module); // Don't care
    if (result.source) {
        std::free(const_cast<char *>(result.source));
    }
}

WrenLoadModuleResult loadModule(WrenVM *vm, const char *name) {
    UNUSED(vm); // Only got one vm :)
    std::string path{name};
    std::ifstream fin;
    std::stringstream buffer;

    path = "src/" + path + ".wren";
    fin.open(path, std::ios::in);
    buffer << fin.rdbuf() << '\0';
    std::string source = buffer.str();

    char *cbuffer = static_cast<char *>(std::malloc(source.size()));
    memcpy(cbuffer, source.c_str(), source.size());
    return {.source = cbuffer, .onComplete = &loadModuleComplete, .userData = nullptr};
}

static void writeFn(WrenVM *vm, const char *text) {
    UNUSED(vm); // Only got one vm :)
    printf("%s", text);
}

void errorFn(WrenVM *vm, WrenErrorType errorType, const char *module, const int line, const char *msg) {
    UNUSED(vm); // Only got one vm :)
    switch (errorType) {
    case WREN_ERROR_COMPILE: {
        printf("[%s line %d] [Error] %s\n", module, line, msg);
    } break;
    case WREN_ERROR_STACK_TRACE: {
        printf("[%s line %d] in %s\n", module, line, msg);
    } break;
    case WREN_ERROR_RUNTIME: {
        printf("[Runtime Error] %s\n", msg);
    } break;
    }
}

int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv, argv + argc);

    WrenConfiguration config;
    wrenInitConfiguration(&config);
    config.writeFn = &writeFn;
    config.errorFn = &errorFn;
    config.loadModuleFn = &loadModule;
    WrenVM *vm = wrenNewVM(&config);

    const char *module = "hello";

    auto loadModuleResult = loadModule(vm, module);
    WrenInterpretResult result = wrenInterpret(vm, module, loadModuleResult.source);

    switch (result) {
    case WREN_RESULT_COMPILE_ERROR: {
        printf("Compile Error!\n");
    } break;
    case WREN_RESULT_RUNTIME_ERROR: {
        printf("Runtime Error!\n");
    } break;
    case WREN_RESULT_SUCCESS: {
        printf("Success!\n");
    } break;
    }

    loadModuleResult.onComplete(vm, module, loadModuleResult);
    // std::free(const_cast<char *>(loadModuleResult.source));
    wrenFreeVM(vm);
}
