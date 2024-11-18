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
    std::vector<std::string> cmd_args(argv, argv + argc);

    for (auto arg : cmd_args)
        fmt::print("#{}#\n", arg);

    tss::LuaInstance vm;
    if (vm.get_number("a").okOrDefault(0)) {
        fmt::print("Lua OK!\n");
    } else {
        fmt::print("Lua NOK!\n");
    }

    const char *wat_string = "(module\n"
                             "  (type $sum_t (func (param i32 i32) (result i32)))\n"
                             "  (func $sum_f (type $sum_t) (param $x i32) (param $y i32) (result i32)\n"
                             "    local.get $x\n"
                             "    local.get $y\n"
                             "    i32.add)\n"
                             "  (export \"sum\" (func $sum_f)))";

    wasm_byte_vec_t wat;
    wasm_byte_vec_new(&wat, strlen(wat_string), wat_string);
    wasm_byte_vec_t wasm_bytes;
    wat2wasm(&wat, &wasm_bytes);
    wasm_byte_vec_delete(&wat);

    printf("Creating the store...\n");
    wasm_engine_t *engine = wasm_engine_new();
    wasm_store_t *store = wasm_store_new(engine);

    printf("Compiling module...\n");
    wasm_module_t *module = wasm_module_new(store, &wasm_bytes);

    if (!module) {
        printf("> Error compiling module!\n");
        return 1;
    }

    wasm_byte_vec_delete(&wasm_bytes);

    printf("Creating imports...\n");
    wasm_extern_vec_t import_object = WASM_EMPTY_VEC;

    printf("Instantiating module...\n");
    wasm_instance_t *instance = wasm_instance_new(store, module, &import_object, NULL);

    if (!instance) {
        printf("> Error instantiating module!\n");
        return 1;
    }

    printf("Retrieving exports...\n");
    wasm_extern_vec_t exports;
    wasm_instance_exports(instance, &exports);

    if (exports.size == 0) {
        printf("> Error accessing exports!\n");
        return 1;
    }

    printf("Retrieving the `sum` function...\n");
    wasm_func_t *sum_func = wasm_extern_as_func(exports.data[0]);

    if (sum_func == NULL) {
        printf("> Failed to get the `sum` function!\n");
        return 1;
    }

    printf("Calling `sum` function...\n");
    wasm_val_t args_val[2] = {WASM_I32_VAL(3), WASM_I32_VAL(4)};
    wasm_val_t results_val[1] = {WASM_INIT_VAL};
    wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);
    wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

    if (wasm_func_call(sum_func, &args, &results)) {
        printf("> Error calling the `sum` function!\n");

        return 1;
    }

    printf("Results of `sum`: %d\n", results_val[0].of.i32);

    wasm_module_delete(module);
    wasm_extern_vec_delete(&exports);
    wasm_instance_delete(instance);
    wasm_store_delete(store);
    wasm_engine_delete(engine);
}
