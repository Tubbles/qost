#include "inc.hh"

namespace tss {
template <typename T>
concept Printable = requires(T t) {
    { std::cout << t } -> std::same_as<std::ostream &>;
};

template <Printable SuccessType, Printable ErrorType>
class Result {
public:
    enum ResultVariant { OkVariant, ErrorVariant };

    static Result Ok(SuccessType value) {
        auto output = Result(value);
        output.m_Ok = true;
        return output;
    }

    static Result Error(ErrorType value) {
        auto output = Result(value);
        output.m_Ok = false;
        return output;
    }

    inline bool isOk() const {
        return m_Ok;
    }

    inline bool isError() const {
        return !m_Ok;
    }

    SuccessType okOrDefault(SuccessType defaultValue) const {
        return m_Ok ? std::get<OkVariant>(m_Data) : defaultValue;
    }

    SuccessType unwrap(std::string_view error_msg = "Not OK.") const {
        if (isError()) {
            throw std::bad_variant_access();
        }
        return std::get<OkVariant>(m_Data);
    }

    ErrorType getError(std::string_view error_msg = "Not Error.") const {
        if (m_Ok) {
            throw std::bad_variant_access();
        }
        return std::get<ErrorVariant>(m_Data);
    }

private:
    Result(std::variant<SuccessType, ErrorType> result) : m_Data{result} {}
    std::variant<SuccessType, ErrorType> m_Data;
    bool m_Ok{false};
};

enum class LuaError {
    FileOpenError,
    StringNotFound,
    StringParseError,
    NumberNotFound,
};

std::ostream &operator<<(std::ostream &stream, const LuaError &e);

class LuaInstance {
public:
    using StringResult = Result<std::string, LuaError>;
    using NumberResult = Result<double, LuaError>;
    LuaInstance();
    ~LuaInstance();
    StringResult execute_file(const char *fn);
    StringResult get_string(const char *var_name);
    NumberResult get_number(const char *var_name);

private:
    lua_State *const m_L;
};

LuaInstance::LuaInstance() //(1)
    : m_L{luaL_newstate()} {
    luaL_openlibs(m_L); //(2)
}

LuaInstance::~LuaInstance() {
    lua_close(m_L); //(3)
}

LuaInstance::StringResult LuaInstance::execute_file(const char *fn) {
    auto result = luaL_loadfile(m_L, fn); //(4)
    if (result != 0) {
        std::cout << lua_tostring(m_L, -1) << std::endl; //(5)
        lua_pop(m_L, 1);
        return LuaInstance::StringResult::Error(LuaError::FileOpenError); //(6)
    }
    lua_pcall(m_L, 0, LUA_MULTRET, 0);                                  //(7)
    return LuaInstance::StringResult::Ok("File successfully executed"); //(8)
}

LuaInstance::StringResult LuaInstance::get_string(const char *var_name) //(9)
{
    auto result = lua_getglobal(m_L, var_name);
    if (result != LUA_TSTRING) {
        lua_pop(m_L, 1);
        return LuaInstance::StringResult::Error(LuaError::StringNotFound);
    }
    auto s = LuaInstance::StringResult::Ok(lua_tostring(m_L, -1));
    lua_pop(m_L, 1);
    return s;
}

LuaInstance::NumberResult LuaInstance::get_number(const char *var_name) {
    auto result = lua_getglobal(m_L, var_name); //(10)
    if (result != LUA_TNUMBER) {
        lua_pop(m_L, 1);
        return LuaInstance::NumberResult::Error(LuaError::NumberNotFound);
    }
    auto s = LuaInstance::NumberResult::Ok(lua_tonumber(m_L, -1));
    lua_pop(LuaInstance::m_L, 1);
    return s;
}

std::ostream &operator<<(std::ostream &stream, const LuaError &error) {
    auto output = [](const LuaError &e) //(11)
    {
        switch (e) {
        case LuaError::FileOpenError:
            return "LUA ERROR: Error opening file.";
        case LuaError::StringNotFound:
            return "LUA ERROR: String not found.";
        case LuaError::StringParseError:
            return "LUA ERROR: String could not be parsed.";
        case LuaError::NumberNotFound:
            return "LUA ERROR: Number not found.";
        default:
            return "LUA ERROR: Unknown error";
        }
    };
    return stream << output(error);
}
} // namespace tss

int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv, argv + argc);
}

#if 0
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
#endif
