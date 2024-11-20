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

auto wasm_mutability_to_str(wasm_mutability_t mutability) -> std::string {
    switch (mutability) {
    case WASM_CONST: {
        return "const";
    }
    case WASM_VAR: {
        return "var";
    }
    default: {
        return fmt::format("unknown ({})", mutability);
    }
    }
}

auto wasm_externkind_to_str(wasm_externkind_t externkind) -> std::string {
    switch (externkind) {
    case WASM_EXTERN_FUNC: {
        return "func";
    }
    case WASM_EXTERN_GLOBAL: {
        return "global";
    }
    case WASM_EXTERN_TABLE: {
        return "table";
    }
    case WASM_EXTERN_MEMORY: {
        return "memory";
    }
    default: {
        return fmt::format("unknown ({})", externkind);
    }
    }
}

auto wasm_externtype_to_str(const wasm_externtype_t *externtype) -> std::string {
    std::string str;
    auto externkind = ::wasm_externtype_kind(externtype);

    switch (externkind) {
    case WASM_EXTERN_FUNC: {
        auto functype = ::wasm_externtype_as_functype_const(externtype);
        auto params = ::wasm_functype_params(functype);
        auto results = ::wasm_functype_results(functype);

        str += "(";
        for (size_t index = 0; index < params->size; index++) {
            auto param = params->data[index];
            auto valkind = ::wasm_valtype_kind(param);
            str += wasm_valkind_to_str(valkind);
            if (index != params->size - 1)
                str += ", ";
        }
        str += ") -> (";
        for (size_t index = 0; index < results->size; index++) {
            auto result = results->data[index];
            auto valkind = ::wasm_valtype_kind(result);
            str += wasm_valkind_to_str(valkind);
            if (index != results->size - 1)
                str += ", ";
        }
        str += ")";
        break;
    }
    case WASM_EXTERN_GLOBAL:
    case WASM_EXTERN_TABLE:
    case WASM_EXTERN_MEMORY: {
        str += "NOT YET IMPLEMENTED";
        break;
    }
    default: {
        break;
    }
    }

    return str;
}

auto wasm_importtype_to_str(const wasm_importtype_t *importtype) -> std::string {
    std::string str;
    auto module = ::wasm_importtype_module(importtype);
    auto name = ::wasm_importtype_name(importtype);
    auto externtype = ::wasm_importtype_type(importtype);
    auto externkind = ::wasm_externtype_kind(externtype);

    str += wasm_externkind_to_str(externkind) + " ";
    str += std::string(module->data, module->size) + "::";
    str += std::string(name->data, name->size);
    str += wasm_externtype_to_str(externtype);

    return str;
}

auto wasm_exporttype_to_str(const wasm_exporttype_t *exporttype, const std::string modulename = "") -> std::string {
    std::string str;
    auto name = ::wasm_exporttype_name(exporttype);
    auto externtype = ::wasm_exporttype_type(exporttype);
    auto externkind = ::wasm_externtype_kind(externtype);

    str += wasm_externkind_to_str(externkind) + " ";
    str += modulename + "::";
    str += std::string(name->data, name->size);
    str += wasm_externtype_to_str(externtype);

    return str;
}

auto wasm_module_imports_to_str(const wasm_module_t *module) -> std::string {
    std::string str;
    wasm_importtype_vec_t imports;
    ::wasm_module_imports(module, &imports);

    for (size_t index = 0; index < imports.size; index++) {
        str += fmt::format("{}\n", tss::wasm_importtype_to_str(imports.data[index]));
    }

    ::wasm_importtype_vec_delete(&imports);

    return str;
}

auto wasm_module_exports_to_str(const wasm_module_t *module) -> std::string {
    std::string str;
    wasm_exporttype_vec_t exports;
    ::wasm_module_exports(module, &exports);

    for (size_t index = 0; index < exports.size; index++) {
        str += fmt::format("{}\n", tss::wasm_exporttype_to_str(exports.data[index]));
    }

    ::wasm_exporttype_vec_delete(&exports);

    return str;
}

auto wasm_frame_to_str(const wasm_frame_t *frame) -> std::string {
    std::string str;

    if (frame) {
        str += fmt::format("{} @ {:#08x} = {}.{:#08x}",
                           static_cast<void *>(::wasm_frame_instance(frame)),
                           ::wasm_frame_module_offset(frame),
                           ::wasm_frame_func_index(frame),
                           ::wasm_frame_func_offset(frame));
    } else {
        str += fmt::format("{}", static_cast<const void *>(frame));
    }

    return str;
}

auto wasm_new_populated_imports_vec(wasm_extern_vec_t *out, const wasm_module_t *module, wasm_store_t *store) {
    wasm_importtype_vec_t imports;
    ::wasm_module_imports(module, &imports);
    wasm_extern_vec_new_uninitialized(out, imports.size);

    for (size_t index = 0; index < imports.size; index++) {
        auto importtype = imports.data[index];
        auto externtype = ::wasm_importtype_type(importtype);
        auto externkind = ::wasm_externtype_kind(externtype);
        switch (externkind) {
        case WASM_EXTERN_FUNC: {
            auto functype = ::wasm_externtype_as_functype_const(externtype);
            auto str = wasm_importtype_to_str(importtype);
            auto stub = tss::wasi_get_stub_from_str(str);
            fmt::print("{} {}\n", stub != nullptr, str);
            wasm_func_t *func = wasm_func_new(store, functype, stub);
            out->data[index] = wasm_func_as_extern(func);

            break;
        }
        case WASM_EXTERN_GLOBAL:
        case WASM_EXTERN_TABLE:
        case WASM_EXTERN_MEMORY: {
            fmt::print(stderr, "Warning: Unsupported import: {}\n", externkind);
            continue;
        }
        default: {
            break;
        }
        }
    }

    ::wasm_importtype_vec_delete(&imports);
}
} // namespace tss

int main(int argc, char *argv[]) {
    // using namespace std::string_view_literals;

    std::vector<std::string> cmd_args(argv, argv + argc);

    for (auto arg : cmd_args)
        fmt::print("#{}#\n", arg);

    // tss::LuaInstance vm;
    // if (vm.get_number("a").okOrDefault(0)) {
    //     fmt::print("Lua OK!\n");
    // } else {
    //     fmt::print("Lua NOK!\n");
    // }

    // const char *wat_string = "(module\n"
    //                          "  (type $sum_t (func (param i32 i32) (result i32)))\n"
    //                          "  (func $sum_f (type $sum_t) (param $x i32) (param $y i32) (result i32)\n"
    //                          "    local.get $x\n"
    //                          "    local.get $y\n"
    //                          "    i32.add)\n"
    //                          "  (export \"sum\" (func $sum_f)))";

    // wasm_byte_vec_t wat;
    // wasm_byte_vec_new(&wat, strlen(wat_string), wat_string);
    wasm_byte_vec_t wasm_bytes;
    // wat2wasm(&wat, &wasm_bytes);
    // wasm_byte_vec_delete(&wat);

    std::ifstream ifs("python.wasm", std::ios::binary | std::ios::ate);
    if (!ifs) {
        fmt::print(stderr, "python.wasm: {}\n", std::strerror(errno));
        return 1;
    }

    auto end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    auto size = std::size_t(end - ifs.tellg());

    if (size == 0) // avoid undefined behavior
        return {};

    std::vector<char> buffer(size);

    if (!ifs.read(&buffer[0], static_cast<std::streamsize>(buffer.size()))) {
        fmt::print(stderr, "python.wasm: {}\n", std::strerror(errno));
        return 1;
    }

    wasm_byte_vec_new(&wasm_bytes, buffer.size(), &buffer[0]);

    fmt::print("Read wasm file of size {}\n", wasm_bytes.size);

    fmt::print("Creating the store...\n");
    wasm_engine_t *engine = wasm_engine_new();
    wasm_store_t *store = wasm_store_new(engine);

    fmt::print("Compiling module...\n");
    wasm_module_t *module = wasm_module_new(store, &wasm_bytes);

    if (!module) {
        fmt::print(stderr, "> Error compiling module!\n");
        wasm_module_delete(module);
        wasm_store_delete(store);
        wasm_engine_delete(engine);
        return 1;
    }

    wasm_byte_vec_delete(&wasm_bytes);
    buffer.clear();

    fmt::print("{}", tss::wasm_module_imports_to_str(module));

    fmt::print("Creating imports...\n");
    // // wasm_functype_t *host_func_type = wasm_functype_new_0_1(wasm_valtype_new_i32());
    // wasm_functype_t *host_func_type = wasm_functype_new_0_0();
    // wasm_func_t *host_func = wasm_func_new(store, host_func_type, tss::wasi_args_get_stub);
    // // wasm_func_get_host_info(host_func);
    // wasm_functype_delete(host_func_type);

    // wasm_extern_t *externs[] = {wasm_func_as_extern(host_func)};

    wasm_extern_vec_t imports;
    tss::wasm_new_populated_imports_vec(&imports, module, store);
    // wasm_extern_vec_new(&imports, ARRLEN(externs), externs);
    // wasm_extern_vec_t imports = WASM_ARRAY_VEC(externs);
    // wasm_extern_vec_t imports = WASM_EMPTY_VEC;

    fmt::print("Instantiating module...\n");
    wasm_trap_t *trap = NULL;
    wasm_instance_t *instance = wasm_instance_new(store, module, &imports, &trap);

    wasm_extern_vec_delete(&imports);

    if (!instance || trap) {
        fmt::print(stderr, "> Error instantiating module!\n");
        if (trap) {
            fmt::print(stderr, "> Trap detected\n");
        }
        wasm_module_delete(module);
        wasm_instance_delete(instance);
        wasm_store_delete(store);
        wasm_engine_delete(engine);
        return 1;
    }

    fmt::print("{}", tss::wasm_module_exports_to_str(module));

    fmt::print("Retrieving exports...\n");
    wasm_extern_vec_t exports;
    wasm_instance_exports(instance, &exports);
    fmt::print("Num exports found: {}\n", exports.size);

    if (exports.size == 0) {
        fmt::print(stderr, "> Error accessing exports!\n");
        wasm_module_delete(module);
        wasm_extern_vec_delete(&exports);
        wasm_instance_delete(instance);
        wasm_store_delete(store);
        wasm_engine_delete(engine);
        return 1;
    }

    fmt::print("Retrieving the `_start` function...\n");
    wasm_func_t *start_func = wasm_extern_as_func(exports.data[1]);

    if (start_func == NULL) {
        fmt::print("> Failed to get the `_start` function!\n");
        wasm_module_delete(module);
        wasm_extern_vec_delete(&exports);
        wasm_instance_delete(instance);
        wasm_store_delete(store);
        wasm_engine_delete(engine);
        return 1;
    }

    fmt::print("Calling `_start` function...\n");
    // wasm_val_t args_val[2] = {WASM_I32_VAL(3), WASM_I32_VAL(4)};
    // wasm_val_t results_val[1] = {WASM_INIT_VAL};
    // wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);
    // wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);
    wasm_val_vec_t args = WASM_EMPTY_VEC;
    wasm_val_vec_t results = WASM_EMPTY_VEC;

    // if (wasm_func_call(start_func, nullptr, nullptr)) {
    trap = wasm_func_call(start_func, &args, &results);
    if (trap) {
        wasm_message_t message;
        wasm_trap_message(trap, &message);
        fmt::print(stderr, "> Error calling the `_start` function: {}\n", &message.data[0]);
        wasm_name_delete(&message);

        auto frame = wasm_trap_origin(trap);
        if (frame) {
            fmt::print(stderr, "> origin: {}\n", tss::wasm_frame_to_str(frame));
        }

        wasm_frame_vec_t frame_vec;
        wasm_trap_trace(trap, &frame_vec);
        for (size_t index = 0; index < frame_vec.size; index++) {
            fmt::print(stderr, "> frame {}: {}\n", index, tss::wasm_frame_to_str(frame_vec.data[index]));
        }

        wasm_trap_delete(trap);
        wasm_module_delete(module);
        wasm_extern_vec_delete(&exports);
        wasm_instance_delete(instance);
        wasm_store_delete(store);
        wasm_engine_delete(engine);
        return 1;
    }

    fmt::print("Done!\n");
    // fmt::print("Results of `sum`: %d\n", results_val[0].of.i32);

    wasm_module_delete(module);
    wasm_extern_vec_delete(&exports);
    wasm_instance_delete(instance);
    wasm_store_delete(store);
    wasm_engine_delete(engine);
}
