#pragma once

#include "inc.hh"

#include <ctime>

namespace tss {

inline wasm_store_t *g_store = nullptr;

inline wasm_memory_t *g_memory = nullptr;

inline auto wasm_valkind_to_str(wasm_valkind_t valkind) -> std::string {
    switch (valkind) {
    case WASM_I32: {
        return "i32";
    }
    case WASM_I64: {
        return "i64";
    }
    case WASM_F32: {
        return "f32";
    }
    case WASM_F64: {
        return "f64";
    }
    case WASM_EXTERNREF: {
        return "externref";
    }
    case WASM_FUNCREF: {
        return "funcref";
    }
    default: {
        return fmt::format("unknown ({})", valkind);
    }
    }
}

inline auto wasm_val_to_str(wasm_val_t val) {
    std::string str;

    switch (val.kind) {
    case WASM_I32: {
        str += fmt::format("{:#Lx}:i32", val.of.i32);
        break;
    }
    case WASM_I64: {
        str += fmt::format("{:#Lx}:i64", val.of.i64);
        break;
    }
    case WASM_F32: {
        str += fmt::format("{}:f32", val.of.f32);
        break;
    }
    case WASM_F64: {
        str += fmt::format("{}:f64", val.of.f64);
        break;
    }
    case WASM_EXTERNREF: {
        str += fmt::format("{}:externref", static_cast<void *>(val.of.ref));
        break;
    }
    case WASM_FUNCREF: {
        str += fmt::format("{}:funcref", static_cast<void *>(val.of.ref));
        break;
    }
    }

    return str;
}

inline auto wasm_val_vec_to_str(const wasm_val_vec_t *vals) -> std::string {
    std::string str;

    for (size_t index = 0; index < vals->size; index++) {
        str += wasm_val_to_str(vals->data[index]);
        if (index != vals->size - 1)
            str += ", ";
    }

    return str;
}

inline auto wasm_check_pointer(int32_t pointer_value, size_t object_size, size_t mem_size) -> bool {
    if (object_size > mem_size) {
        return false;
    }
    if (pointer_value > static_cast<int32_t>(mem_size - object_size)) {
        return false;
    }
    return true;
}

inline auto wasm_wasi_fix_return_type(wasm_valkind_t *kind, bool *printed) {
    if (*kind != WASM_I32) {
        if (!*printed) {
            *printed = true;
            fmt::print("Correcting return type from {}\n", wasm_valkind_to_str(*kind));
        }
        *kind = WASM_I32;
    }
}

inline auto wasm_print_func_called(std::string func, const wasm_val_vec_t *args, const wasm_val_vec_t *results) {
    fmt::print("==> func called {}({}) -> ({})\n", func, wasm_val_vec_to_str(args), wasm_val_vec_to_str(results));
}

template <wasm_valkind_t T>
inline constexpr auto wasm_get_from_val(wasm_val_t val) {
    assert(val.kind == T);
    if constexpr (T == WASM_I32) {
        return val.of.i32;
    }
    if constexpr (T == WASM_I64) {
        return val.of.i64;
    }
    if constexpr (T == WASM_F32) {
        return val.of.f32;
    }
    if constexpr (T == WASM_F64) {
        return val.of.f64;
    }
    if constexpr (T == WASM_EXTERNREF) {
        return val.of.ref;
    }
    if constexpr (T == WASM_FUNCREF) {
        return val.of.ref;
    }
}

inline auto wasm_not_implemented_trap() -> wasm_trap_t * {
    wasm_message_t message;
    ::wasm_name_new_from_string_nt(&message, "not implemented");
    wasm_trap_t *trap = ::wasm_trap_new(g_store, &message);
    ::wasm_name_delete(&message);
    return trap;
}

#define NANOSECONDS_PER_SECOND 1000000000ULL

inline auto timespec_to_nanoseconds(const timespec *ts) -> __wasi_timestamp_t {
    if (ts->tv_sec < 0)
        return 0;
    if (static_cast<__wasi_timestamp_t>(ts->tv_sec) >= UINT64_MAX / NANOSECONDS_PER_SECOND)
        return UINT64_MAX;
    return static_cast<__wasi_timestamp_t>(ts->tv_sec) * NANOSECONDS_PER_SECOND +
           static_cast<__wasi_timestamp_t>(ts->tv_nsec);
}

inline auto convert_errno(int error) -> __wasi_errno_t {
    __wasi_errno_t code = std::to_underlying(Errno::e_nosys);

    switch (error) {
    case EDOM:
        code = std::to_underlying(Errno::e_dom);
        break;

    case EILSEQ:
        code = std::to_underlying(Errno::e_ilseq);
        break;

    case ERANGE:
        code = std::to_underlying(Errno::e_range);
        break;

    case E2BIG:
        code = std::to_underlying(Errno::e_2big);
        break;

    case EACCES:
        code = std::to_underlying(Errno::e_acces);
        break;

    case EADDRINUSE:
        code = std::to_underlying(Errno::e_addrinuse);
        break;

    case EADDRNOTAVAIL:
        code = std::to_underlying(Errno::e_addrnotavail);
        break;

    case EAFNOSUPPORT:
        code = std::to_underlying(Errno::e_afnosupport);
        break;

    case EAGAIN:
        code = std::to_underlying(Errno::e_again);
        break;

    case EALREADY:
        code = std::to_underlying(Errno::e_already);
        break;

    case EBADF:
        code = std::to_underlying(Errno::e_badf);
        break;

    case EBADMSG:
        code = std::to_underlying(Errno::e_badmsg);
        break;

    case EBUSY:
        code = std::to_underlying(Errno::e_busy);
        break;

    case ECANCELED:
        code = std::to_underlying(Errno::e_canceled);
        break;

    case ECHILD:
        code = std::to_underlying(Errno::e_child);
        break;

    case ECONNABORTED:
        code = std::to_underlying(Errno::e_connaborted);
        break;

    case ECONNREFUSED:
        code = std::to_underlying(Errno::e_connrefused);
        break;

    case ECONNRESET:
        code = std::to_underlying(Errno::e_connreset);
        break;

    case EDEADLK:
        code = std::to_underlying(Errno::e_deadlk);
        break;

    case EDESTADDRREQ:
        code = std::to_underlying(Errno::e_destaddrreq);
        break;

    case EDQUOT:
        code = std::to_underlying(Errno::e_dquot);
        break;

    case EEXIST:
        code = std::to_underlying(Errno::e_exist);
        break;

    case EFAULT:
        code = std::to_underlying(Errno::e_fault);
        break;

    case EFBIG:
        code = std::to_underlying(Errno::e_fbig);
        break;

    case EHOSTUNREACH:
        code = std::to_underlying(Errno::e_hostunreach);
        break;

    case EIDRM:
        code = std::to_underlying(Errno::e_idrm);
        break;

    case EINPROGRESS:
        code = std::to_underlying(Errno::e_inprogress);
        break;

    case EINTR:
        code = std::to_underlying(Errno::e_intr);
        break;

    case EINVAL:
        code = std::to_underlying(Errno::e_inval);
        break;

    case EIO:
        code = std::to_underlying(Errno::e_io);
        break;

    case EISCONN:
        code = std::to_underlying(Errno::e_isconn);
        break;

    case EISDIR:
        code = std::to_underlying(Errno::e_isdir);
        break;

    case ELOOP:
        code = std::to_underlying(Errno::e_loop);
        break;

    case EMFILE:
        code = std::to_underlying(Errno::e_mfile);
        break;

    case EMLINK:
        code = std::to_underlying(Errno::e_mlink);
        break;

    case EMSGSIZE:
        code = std::to_underlying(Errno::e_msgsize);
        break;

    case EMULTIHOP:
        code = std::to_underlying(Errno::e_multihop);
        break;

    case ENAMETOOLONG:
        code = std::to_underlying(Errno::e_nametoolong);
        break;

    case ENETDOWN:
        code = std::to_underlying(Errno::e_netdown);
        break;

    case ENETRESET:
        code = std::to_underlying(Errno::e_netreset);
        break;

    case ENETUNREACH:
        code = std::to_underlying(Errno::e_netunreach);
        break;

    case ENFILE:
        code = std::to_underlying(Errno::e_nfile);
        break;

    case ENOBUFS:
        code = std::to_underlying(Errno::e_nobufs);
        break;

    case ENODEV:
        code = std::to_underlying(Errno::e_nodev);
        break;

    case ENOENT:
        code = std::to_underlying(Errno::e_noent);
        break;

    case ENOEXEC:
        code = std::to_underlying(Errno::e_noexec);
        break;

    case ENOLCK:
        code = std::to_underlying(Errno::e_nolck);
        break;

    case ENOLINK:
        code = std::to_underlying(Errno::e_nolink);
        break;

    case ENOMEM:
        code = std::to_underlying(Errno::e_nomem);
        break;

    case ENOMSG:
        code = std::to_underlying(Errno::e_nomsg);
        break;

    case ENOPROTOOPT:
        code = std::to_underlying(Errno::e_noprotoopt);
        break;

    case ENOSPC:
        code = std::to_underlying(Errno::e_nospc);
        break;

    case ENOSYS:
        code = std::to_underlying(Errno::e_nosys);
        break;

#ifdef ENOTCAPABLE
    case ENOTCAPABLE:
        code = std::to_underlying(Errno::e_notcapable);
        break;
#endif

    case ENOTCONN:
        code = std::to_underlying(Errno::e_notconn);
        break;

    case ENOTDIR:
        code = std::to_underlying(Errno::e_notdir);
        break;

    case ENOTEMPTY:
        code = std::to_underlying(Errno::e_notempty);
        break;

    case ENOTRECOVERABLE:
        code = std::to_underlying(Errno::e_notrecoverable);
        break;

    case ENOTSOCK:
        code = std::to_underlying(Errno::e_notsock);
        break;

    case ENOTSUP:
        code = std::to_underlying(Errno::e_notsup);
        break;

    case ENOTTY:
        code = std::to_underlying(Errno::e_notty);
        break;

    case ENXIO:
        code = std::to_underlying(Errno::e_nxio);
        break;

    case EOVERFLOW:
        code = std::to_underlying(Errno::e_overflow);
        break;

    case EOWNERDEAD:
        code = std::to_underlying(Errno::e_ownerdead);
        break;

    case EPERM:
        code = std::to_underlying(Errno::e_perm);
        break;

    case EPIPE:
        code = std::to_underlying(Errno::e_pipe);
        break;

    case EPROTO:
        code = std::to_underlying(Errno::e_proto);
        break;

    case EPROTONOSUPPORT:
        code = std::to_underlying(Errno::e_protonosupport);
        break;

    case EPROTOTYPE:
        code = std::to_underlying(Errno::e_prototype);
        break;

    case EROFS:
        code = std::to_underlying(Errno::e_rofs);
        break;

    case ESPIPE:
        code = std::to_underlying(Errno::e_spipe);
        break;

    case ESRCH:
        code = std::to_underlying(Errno::e_srch);
        break;

    case ESTALE:
        code = std::to_underlying(Errno::e_stale);
        break;

    case ETIMEDOUT:
        code = std::to_underlying(Errno::e_timedout);
        break;

    case ETXTBSY:
        code = std::to_underlying(Errno::e_txtbsy);
        break;

    case EXDEV:
        code = std::to_underlying(Errno::e_xdev);
        break;

    default:
        if (error == EOPNOTSUPP)
            code = std::to_underlying(Errno::e_notsup);

        if (error == EWOULDBLOCK)
            code = std::to_underlying(Errno::e_again);

        break;
    }

    return code;
}

#define mem_as(mem, as) *reinterpret_cast<as *>(&mem)

inline auto wasi_args_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_args_sizes_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_environ_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto
wasi_environ_sizes_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_clock_res_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_clock_time_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    assert(args->size == 3);
    assert(results->size == 1);
    __wasi_clockid_t clock_id = static_cast<__wasi_clockid_t>(wasm_get_from_val<WASM_I32>(args->data[0]));
    int64_t precision         = wasm_get_from_val<WASM_I64>(args->data[1]);
    int32_t time_ptr          = wasm_get_from_val<WASM_I32>(args->data[2]);
    UNUSED(precision);
    byte_t *mem     = wasm_memory_data(g_memory);
    size_t mem_size = wasm_memory_data_size(g_memory);

    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);

    auto target = reinterpret_cast<__wasi_timestamp_t *>(&mem[time_ptr]);

    if (!wasm_check_pointer(time_ptr, sizeof(__wasi_timestamp_t), mem_size)) {
        results->data[0].of.i32 = std::to_underlying(Errno::e_2big);
        goto out;
    }

    if (!magic_enum::enum_contains<ClockID>(clock_id)) {
        results->data[0].of.i32 = std::to_underlying(Errno::e_inval);
        goto out;
    }

    timespec ts;
    if (clock_gettime(static_cast<clockid_t>(clock_id), &ts) < 0) {
        results->data[0].of.i32 = convert_errno(errno);
        goto out;
    }

    *target = timespec_to_nanoseconds(&ts);
    fmt::print("setting mem {:#Lx} to {:#Lx}\n", time_ptr, *target);

out:
    wasm_print_func_called(__func__, args, results);
    return nullptr;
    // return wasm_not_implemented_trap();
}

inline auto wasi_fd_advise_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_close_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_datasync_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_fdstat_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto
wasi_fd_fdstat_set_flags_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_filestat_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto
wasi_fd_filestat_set_size_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto
wasi_fd_filestat_set_times_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_pread_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_prestat_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto
wasi_fd_prestat_dir_name_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_pwrite_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_read_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_readdir_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_seek_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_sync_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_tell_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_fd_write_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto
wasi_path_create_directory_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto
wasi_path_filestat_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto
wasi_path_filestat_set_times_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_path_link_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_path_open_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_path_readlink_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto
wasi_path_remove_directory_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_path_rename_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_path_symlink_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto
wasi_path_unlink_file_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_poll_oneoff_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_proc_exit_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_sched_yield_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_random_get_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_sock_accept_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_sock_recv_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_sock_send_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_sock_shutdown_stub(void *env, const wasm_val_vec_t *args, wasm_val_vec_t *results) -> wasm_trap_t * {
    UNUSED(env);
    static bool printed = false;
    wasm_wasi_fix_return_type(&results->data[0].kind, &printed);
    results->data[0].of.i32 = std::to_underlying(Errno::e_success);
    wasm_print_func_called(__func__, args, results);
    return wasm_not_implemented_trap();
}

inline auto wasi_get_stub_from_str(const std::string str) -> wasm_func_callback_with_env_t {
    // clang-format off
    if (str.find("wasi_snapshot_preview1::args_get(i32, i32) -> (i32)") != std::string::npos)
        return wasi_args_get_stub;
    else if (str.find("wasi_snapshot_preview1::args_sizes_get(i32, i32) -> (i32)") != std::string::npos)
        return wasi_args_sizes_get_stub;
    else if (str.find("wasi_snapshot_preview1::environ_get(i32, i32) -> (i32)") != std::string::npos)
        return wasi_environ_get_stub;
    else if (str.find("wasi_snapshot_preview1::environ_sizes_get(i32, i32) -> (i32)") != std::string::npos)
        return wasi_environ_sizes_get_stub;
    else if (str.find("wasi_snapshot_preview1::clock_res_get(i32, i32) -> (i32)") != std::string::npos)
        return wasi_clock_res_get_stub;
    else if (str.find("wasi_snapshot_preview1::clock_time_get(i32, i64, i32) -> (i32)") != std::string::npos)
        return wasi_clock_time_get_stub;
    else if (str.find("wasi_snapshot_preview1::fd_advise(i32, i64, i64, i32) -> (i32)") != std::string::npos)
        return wasi_fd_advise_stub;
    else if (str.find("wasi_snapshot_preview1::fd_close(i32) -> (i32)") != std::string::npos)
        return wasi_fd_close_stub;
    else if (str.find("wasi_snapshot_preview1::fd_datasync(i32) -> (i32)") != std::string::npos)
        return wasi_fd_datasync_stub;
    else if (str.find("wasi_snapshot_preview1::fd_fdstat_get(i32, i32) -> (i32)") != std::string::npos)
        return wasi_fd_fdstat_get_stub;
    else if (str.find("wasi_snapshot_preview1::fd_fdstat_set_flags(i32, i32) -> (i32)") != std::string::npos)
        return wasi_fd_fdstat_set_flags_stub;
    else if (str.find("wasi_snapshot_preview1::fd_filestat_get(i32, i32) -> (i32)") != std::string::npos)
        return wasi_fd_filestat_get_stub;
    else if (str.find("wasi_snapshot_preview1::fd_filestat_set_size(i32, i64) -> (i32)") != std::string::npos)
        return wasi_fd_filestat_set_size_stub;
    else if (str.find("wasi_snapshot_preview1::fd_filestat_set_times(i32, i64, i64, i32) -> (i32)") != std::string::npos)
        return wasi_fd_filestat_set_times_stub;
    else if (str.find("wasi_snapshot_preview1::fd_pread(i32, i32, i32, i64, i32) -> (i32)") != std::string::npos)
        return wasi_fd_pread_stub;
    else if (str.find("wasi_snapshot_preview1::fd_prestat_get(i32, i32) -> (i32)") != std::string::npos)
        return wasi_fd_prestat_get_stub;
    else if (str.find("wasi_snapshot_preview1::fd_prestat_dir_name(i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_fd_prestat_dir_name_stub;
    else if (str.find("wasi_snapshot_preview1::fd_pwrite(i32, i32, i32, i64, i32) -> (i32)") != std::string::npos)
        return wasi_fd_pwrite_stub;
    else if (str.find("wasi_snapshot_preview1::fd_read(i32, i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_fd_read_stub;
    else if (str.find("wasi_snapshot_preview1::fd_readdir(i32, i32, i32, i64, i32) -> (i32)") != std::string::npos)
        return wasi_fd_readdir_stub;
    else if (str.find("wasi_snapshot_preview1::fd_seek(i32, i64, i32, i32) -> (i32)") != std::string::npos)
        return wasi_fd_seek_stub;
    else if (str.find("wasi_snapshot_preview1::fd_sync(i32) -> (i32)") != std::string::npos)
        return wasi_fd_sync_stub;
    else if (str.find("wasi_snapshot_preview1::fd_tell(i32, i32) -> (i32)") != std::string::npos)
        return wasi_fd_tell_stub;
    else if (str.find("wasi_snapshot_preview1::fd_write(i32, i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_fd_write_stub;
    else if (str.find("wasi_snapshot_preview1::path_create_directory(i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_path_create_directory_stub;
    else if (str.find("wasi_snapshot_preview1::path_filestat_get(i32, i32, i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_path_filestat_get_stub;
    else if (str.find("wasi_snapshot_preview1::path_filestat_set_times(i32, i32, i32, i32, i64, i64, i32) -> (i32)") != std::string::npos)
        return wasi_path_filestat_set_times_stub;
    else if (str.find("wasi_snapshot_preview1::path_link(i32, i32, i32, i32, i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_path_link_stub;
    else if (str.find("wasi_snapshot_preview1::path_open(i32, i32, i32, i32, i32, i64, i64, i32, i32) -> (i32)") != std::string::npos)
        return wasi_path_open_stub;
    else if (str.find("wasi_snapshot_preview1::path_readlink(i32, i32, i32, i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_path_readlink_stub;
    else if (str.find("wasi_snapshot_preview1::path_remove_directory(i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_path_remove_directory_stub;
    else if (str.find("wasi_snapshot_preview1::path_rename(i32, i32, i32, i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_path_rename_stub;
    else if (str.find("wasi_snapshot_preview1::path_symlink(i32, i32, i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_path_symlink_stub;
    else if (str.find("wasi_snapshot_preview1::path_unlink_file(i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_path_unlink_file_stub;
    else if (str.find("wasi_snapshot_preview1::poll_oneoff(i32, i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_poll_oneoff_stub;
    else if (str.find("wasi_snapshot_preview1::proc_exit(i32) -> ()") != std::string::npos)
        return wasi_proc_exit_stub;
    else if (str.find("wasi_snapshot_preview1::sched_yield() -> (i32)") != std::string::npos)
        return wasi_sched_yield_stub;
    else if (str.find("wasi_snapshot_preview1::random_get(i32, i32) -> (i32)") != std::string::npos)
        return wasi_random_get_stub;
    else if (str.find("wasi_snapshot_preview1::sock_accept(i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_sock_accept_stub;
    else if (str.find("wasi_snapshot_preview1::sock_recv(i32, i32, i32, i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_sock_recv_stub;
    else if (str.find("wasi_snapshot_preview1::sock_send(i32, i32, i32, i32, i32) -> (i32)") != std::string::npos)
        return wasi_sock_send_stub;
    else if (str.find("wasi_snapshot_preview1::sock_shutdown(i32, i32) -> (i32)") != std::string::npos)
        return wasi_sock_shutdown_stub;
    // clang-format on

    return nullptr;
}
} // namespace tss
// clang-format off

/**
 * Read command-line argument data.
 * The size of the array should match that returned by `args_sizes_get`.
 * Each argument is expected to be `\0` terminated.
 */
__wasi_errno_t __wasi_args_get(
    uint8_t * * argv,
    uint8_t * argv_buf
) __attribute__((__warn_unused_result__));

/**
 * Return command-line argument data sizes.
 * @return
 * Returns the number of arguments and the size of the argument string
 * data, or an error.
 */
__wasi_errno_t __wasi_args_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
) __attribute__((__warn_unused_result__));

/**
 * Read environment variable data.
 * The sizes of the buffers should match that returned by `environ_sizes_get`.
 * Key/value pairs are expected to be joined with `=`s, and terminated with `\0`s.
 */
__wasi_errno_t __wasi_environ_get(
    uint8_t * * environ,
    uint8_t * environ_buf
) __attribute__((__warn_unused_result__));

/**
 * Return environment variable data sizes.
 * @return
 * Returns the number of environment variable arguments and the size of the
 * environment variable data.
 */
__wasi_errno_t __wasi_environ_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
) __attribute__((__warn_unused_result__));

/**
 * Return the resolution of a clock.
 * Implementations are required to provide a non-zero value for supported clocks. For unsupported clocks,
 * return `errno::inval`.
 * Note: This is similar to `clock_getres` in POSIX.
 * @return
 * The resolution of the clock, or an error if one happened.
 */
__wasi_errno_t __wasi_clock_res_get(
    /**
     * The clock for which to return the resolution.
     */
    __wasi_clockid_t id,
    __wasi_timestamp_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Return the time value of a clock.
 * Note: This is similar to `clock_gettime` in POSIX.
 * @return
 * The time value of the clock.
 */
__wasi_errno_t __wasi_clock_time_get(
    /**
     * The clock for which to return the time.
     */
    __wasi_clockid_t id,
    /**
     * The maximum lag (exclusive) that the returned time value may have, compared to its actual value.
     */
    __wasi_timestamp_t precision,
    __wasi_timestamp_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Provide file advisory information on a file descriptor.
 * Note: This is similar to `posix_fadvise` in POSIX.
 */
__wasi_errno_t __wasi_fd_advise(
    __wasi_fd_t fd,
    /**
     * The offset within the file to which the advisory applies.
     */
    __wasi_filesize_t offset,
    /**
     * The length of the region to which the advisory applies.
     */
    __wasi_filesize_t len,
    /**
     * The advice.
     */
    __wasi_advice_t advice
) __attribute__((__warn_unused_result__));

/**
 * Force the allocation of space in a file.
 * Note: This is similar to `posix_fallocate` in POSIX.
 */
__wasi_errno_t __wasi_fd_allocate(
    __wasi_fd_t fd,
    /**
     * The offset at which to start the allocation.
     */
    __wasi_filesize_t offset,
    /**
     * The length of the area that is allocated.
     */
    __wasi_filesize_t len
) __attribute__((__warn_unused_result__));

/**
 * Close a file descriptor.
 * Note: This is similar to `close` in POSIX.
 */
__wasi_errno_t __wasi_fd_close(
    __wasi_fd_t fd
) __attribute__((__warn_unused_result__));

/**
 * Synchronize the data of a file to disk.
 * Note: This is similar to `fdatasync` in POSIX.
 */
__wasi_errno_t __wasi_fd_datasync(
    __wasi_fd_t fd
) __attribute__((__warn_unused_result__));

/**
 * Get the attributes of a file descriptor.
 * Note: This returns similar flags to `fsync(fd, F_GETFL)` in POSIX, as well as additional fields.
 * @return
 * The buffer where the file descriptor's attributes are stored.
 */
__wasi_errno_t __wasi_fd_fdstat_get(
    __wasi_fd_t fd,
    __wasi_fdstat_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Adjust the flags associated with a file descriptor.
 * Note: This is similar to `fcntl(fd, F_SETFL, flags)` in POSIX.
 */
__wasi_errno_t __wasi_fd_fdstat_set_flags(
    __wasi_fd_t fd,
    /**
     * The desired values of the file descriptor flags.
     */
    __wasi_fdflags_t flags
) __attribute__((__warn_unused_result__));

/**
 * Adjust the rights associated with a file descriptor.
 * This can only be used to remove rights, and returns `errno::notcapable` if called in a way that would attempt to add rights
 */
__wasi_errno_t __wasi_fd_fdstat_set_rights(
    __wasi_fd_t fd,
    /**
     * The desired rights of the file descriptor.
     */
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting
) __attribute__((__warn_unused_result__));

/**
 * Return the attributes of an open file.
 * @return
 * The buffer where the file's attributes are stored.
 */
__wasi_errno_t __wasi_fd_filestat_get(
    __wasi_fd_t fd,
    __wasi_filestat_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Adjust the size of an open file. If this increases the file's size, the extra bytes are filled with zeros.
 * Note: This is similar to `ftruncate` in POSIX.
 */
__wasi_errno_t __wasi_fd_filestat_set_size(
    __wasi_fd_t fd,
    /**
     * The desired file size.
     */
    __wasi_filesize_t size
) __attribute__((__warn_unused_result__));

/**
 * Adjust the timestamps of an open file or directory.
 * Note: This is similar to `futimens` in POSIX.
 */
__wasi_errno_t __wasi_fd_filestat_set_times(
    __wasi_fd_t fd,
    /**
     * The desired values of the data access timestamp.
     */
    __wasi_timestamp_t atim,
    /**
     * The desired values of the data modification timestamp.
     */
    __wasi_timestamp_t mtim,
    /**
     * A bitmask indicating which timestamps to adjust.
     */
    __wasi_fstflags_t fst_flags
) __attribute__((__warn_unused_result__));

/**
 * Read from a file descriptor, without using and updating the file descriptor's offset.
 * Note: This is similar to `preadv` in POSIX.
 * @return
 * The number of bytes read.
 */
__wasi_errno_t __wasi_fd_pread(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors in which to store data.
     */
    const __wasi_iovec_t *iovs,
    /**
     * The length of the array pointed to by `iovs`.
     */
    size_t iovs_len,
    /**
     * The offset within the file at which to read.
     */
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Return a description of the given preopened file descriptor.
 * @return
 * The buffer where the description is stored.
 */
__wasi_errno_t __wasi_fd_prestat_get(
    __wasi_fd_t fd,
    __wasi_prestat_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Return a description of the given preopened file descriptor.
 */
__wasi_errno_t __wasi_fd_prestat_dir_name(
    __wasi_fd_t fd,
    /**
     * A buffer into which to write the preopened directory name.
     */
    uint8_t * path,
    __wasi_size_t path_len
) __attribute__((__warn_unused_result__));

/**
 * Write to a file descriptor, without using and updating the file descriptor's offset.
 * Note: This is similar to `pwritev` in POSIX.
 * @return
 * The number of bytes written.
 */
__wasi_errno_t __wasi_fd_pwrite(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors from which to retrieve data.
     */
    const __wasi_ciovec_t *iovs,
    /**
     * The length of the array pointed to by `iovs`.
     */
    size_t iovs_len,
    /**
     * The offset within the file at which to write.
     */
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Read from a file descriptor.
 * Note: This is similar to `readv` in POSIX.
 * @return
 * The number of bytes read.
 */
__wasi_errno_t __wasi_fd_read(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors to which to store data.
     */
    const __wasi_iovec_t *iovs,
    /**
     * The length of the array pointed to by `iovs`.
     */
    size_t iovs_len,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Read directory entries from a directory.
 * When successful, the contents of the output buffer consist of a sequence of
 * directory entries. Each directory entry consists of a `dirent` object,
 * followed by `dirent::d_namlen` bytes holding the name of the directory
 * entry.
 * This function fills the output buffer as much as possible, potentially
 * truncating the last directory entry. This allows the caller to grow its
 * read buffer size in case it's too small to fit a single large directory
 * entry, or skip the oversized directory entry.
 * @return
 * The number of bytes stored in the read buffer. If less than the size of the read buffer, the end of the directory has been reached.
 */
__wasi_errno_t __wasi_fd_readdir(
    __wasi_fd_t fd,
    /**
     * The buffer where directory entries are stored
     */
    uint8_t * buf,
    __wasi_size_t buf_len,
    /**
     * The location within the directory to start reading
     */
    __wasi_dircookie_t cookie,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Atomically replace a file descriptor by renumbering another file descriptor.
 * Due to the strong focus on thread safety, this environment does not provide
 * a mechanism to duplicate or renumber a file descriptor to an arbitrary
 * number, like `dup2()`. This would be prone to race conditions, as an actual
 * file descriptor with the same number could be allocated by a different
 * thread at the same time.
 * This function provides a way to atomically renumber file descriptors, which
 * would disappear if `dup2()` were to be removed entirely.
 */
__wasi_errno_t __wasi_fd_renumber(
    __wasi_fd_t fd,
    /**
     * The file descriptor to overwrite.
     */
    __wasi_fd_t to
) __attribute__((__warn_unused_result__));

/**
 * Move the offset of a file descriptor.
 * Note: This is similar to `lseek` in POSIX.
 * @return
 * The new offset of the file descriptor, relative to the start of the file.
 */
__wasi_errno_t __wasi_fd_seek(
    __wasi_fd_t fd,
    /**
     * The number of bytes to move.
     */
    __wasi_filedelta_t offset,
    /**
     * The base from which the offset is relative.
     */
    __wasi_whence_t whence,
    __wasi_filesize_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Synchronize the data and metadata of a file to disk.
 * Note: This is similar to `fsync` in POSIX.
 */
__wasi_errno_t __wasi_fd_sync(
    __wasi_fd_t fd
) __attribute__((__warn_unused_result__));

/**
 * Return the current offset of a file descriptor.
 * Note: This is similar to `lseek(fd, 0, SEEK_CUR)` in POSIX.
 * @return
 * The current offset of the file descriptor, relative to the start of the file.
 */
__wasi_errno_t __wasi_fd_tell(
    __wasi_fd_t fd,
    __wasi_filesize_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Write to a file descriptor.
 * Note: This is similar to `writev` in POSIX.
 */
__wasi_errno_t __wasi_fd_write(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors from which to retrieve data.
     */
    const __wasi_ciovec_t *iovs,
    /**
     * The length of the array pointed to by `iovs`.
     */
    size_t iovs_len,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Create a directory.
 * Note: This is similar to `mkdirat` in POSIX.
 */
__wasi_errno_t __wasi_path_create_directory(
    __wasi_fd_t fd,
    /**
     * The path at which to create the directory.
     */
    const char *path
) __attribute__((__warn_unused_result__));

/**
 * Return the attributes of a file or directory.
 * Note: This is similar to `stat` in POSIX.
 * @return
 * The buffer where the file's attributes are stored.
 */
__wasi_errno_t __wasi_path_filestat_get(
    __wasi_fd_t fd,
    /**
     * Flags determining the method of how the path is resolved.
     */
    __wasi_lookupflags_t flags,
    /**
     * The path of the file or directory to inspect.
     */
    const char *path,
    __wasi_filestat_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Adjust the timestamps of a file or directory.
 * Note: This is similar to `utimensat` in POSIX.
 */
__wasi_errno_t __wasi_path_filestat_set_times(
    __wasi_fd_t fd,
    /**
     * Flags determining the method of how the path is resolved.
     */
    __wasi_lookupflags_t flags,
    /**
     * The path of the file or directory to operate on.
     */
    const char *path,
    /**
     * The desired values of the data access timestamp.
     */
    __wasi_timestamp_t atim,
    /**
     * The desired values of the data modification timestamp.
     */
    __wasi_timestamp_t mtim,
    /**
     * A bitmask indicating which timestamps to adjust.
     */
    __wasi_fstflags_t fst_flags
) __attribute__((__warn_unused_result__));

/**
 * Create a hard link.
 * Note: This is similar to `linkat` in POSIX.
 */
__wasi_errno_t __wasi_path_link(
    __wasi_fd_t old_fd,
    /**
     * Flags determining the method of how the path is resolved.
     */
    __wasi_lookupflags_t old_flags,
    /**
     * The source path from which to link.
     */
    const char *old_path,
    /**
     * The working directory at which the resolution of the new path starts.
     */
    __wasi_fd_t new_fd,
    /**
     * The destination path at which to create the hard link.
     */
    const char *new_path
) __attribute__((__warn_unused_result__));

/**
 * Open a file or directory.
 * The returned file descriptor is not guaranteed to be the lowest-numbered
 * file descriptor not currently open; it is randomized to prevent
 * applications from depending on making assumptions about indexes, since this
 * is error-prone in multi-threaded contexts. The returned file descriptor is
 * guaranteed to be less than 2**31.
 * Note: This is similar to `openat` in POSIX.
 * @return
 * The file descriptor of the file that has been opened.
 */
__wasi_errno_t __wasi_path_open(
    __wasi_fd_t fd,
    /**
     * Flags determining the method of how the path is resolved.
     */
    __wasi_lookupflags_t dirflags,
    /**
     * The relative path of the file or directory to open, relative to the
     * `path_open::fd` directory.
     */
    const char *path,
    /**
     * The method by which to open the file.
     */
    __wasi_oflags_t oflags,
    /**
     * The initial rights of the newly created file descriptor. The
     * implementation is allowed to return a file descriptor with fewer rights
     * than specified, if and only if those rights do not apply to the type of
     * file being opened.
     * The *base* rights are rights that will apply to operations using the file
     * descriptor itself, while the *inheriting* rights are rights that apply to
     * file descriptors derived from it.
     */
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting,
    __wasi_fdflags_t fdflags,
    __wasi_fd_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Read the contents of a symbolic link.
 * Note: This is similar to `readlinkat` in POSIX.
 * @return
 * The number of bytes placed in the buffer.
 */
__wasi_errno_t __wasi_path_readlink(
    __wasi_fd_t fd,
    /**
     * The path of the symbolic link from which to read.
     */
    const char *path,
    /**
     * The buffer to which to write the contents of the symbolic link.
     */
    uint8_t * buf,
    __wasi_size_t buf_len,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Remove a directory.
 * Return `errno::notempty` if the directory is not empty.
 * Note: This is similar to `unlinkat(fd, path, AT_REMOVEDIR)` in POSIX.
 */
__wasi_errno_t __wasi_path_remove_directory(
    __wasi_fd_t fd,
    /**
     * The path to a directory to remove.
     */
    const char *path
) __attribute__((__warn_unused_result__));

/**
 * Rename a file or directory.
 * Note: This is similar to `renameat` in POSIX.
 */
__wasi_errno_t __wasi_path_rename(
    __wasi_fd_t fd,
    /**
     * The source path of the file or directory to rename.
     */
    const char *old_path,
    /**
     * The working directory at which the resolution of the new path starts.
     */
    __wasi_fd_t new_fd,
    /**
     * The destination path to which to rename the file or directory.
     */
    const char *new_path
) __attribute__((__warn_unused_result__));

/**
 * Create a symbolic link.
 * Note: This is similar to `symlinkat` in POSIX.
 */
__wasi_errno_t __wasi_path_symlink(
    /**
     * The contents of the symbolic link.
     */
    const char *old_path,
    __wasi_fd_t fd,
    /**
     * The destination path at which to create the symbolic link.
     */
    const char *new_path
) __attribute__((__warn_unused_result__));

/**
 * Unlink a file.
 * Return `errno::isdir` if the path refers to a directory.
 * Note: This is similar to `unlinkat(fd, path, 0)` in POSIX.
 */
__wasi_errno_t __wasi_path_unlink_file(
    __wasi_fd_t fd,
    /**
     * The path to a file to unlink.
     */
    const char *path
) __attribute__((__warn_unused_result__));

/**
 * Concurrently poll for the occurrence of a set of events.
 * @return
 * The number of events stored.
 */
__wasi_errno_t __wasi_poll_oneoff(
    /**
     * The events to which to subscribe.
     */
    const __wasi_subscription_t * in,
    /**
     * The events that have occurred.
     */
    __wasi_event_t * out,
    /**
     * Both the number of subscriptions and events.
     */
    __wasi_size_t nsubscriptions,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Terminate the process normally. An exit code of 0 indicates successful
 * termination of the program. The meanings of other values is dependent on
 * the environment.
 */
[[noreturn]] void __wasi_proc_exit(
    /**
     * The exit code returned by the process.
     */
    __wasi_exitcode_t rval
);
/**
 * Temporarily yield execution of the calling thread.
 * Note: This is similar to `sched_yield` in POSIX.
 */
__wasi_errno_t __wasi_sched_yield(
    void
) __attribute__((__warn_unused_result__));

/**
 * Write high-quality random data into a buffer.
 * This function blocks when the implementation is unable to immediately
 * provide sufficient high-quality random data.
 * This function may execute slowly, so when large mounts of random data are
 * required, it's advisable to use this function to seed a pseudo-random
 * number generator, rather than to provide the random data directly.
 */
__wasi_errno_t __wasi_random_get(
    /**
     * The buffer to fill with random data.
     */
    uint8_t * buf,
    __wasi_size_t buf_len
) __attribute__((__warn_unused_result__));

/**
 * Accept a new incoming connection.
 * Note: This is similar to `accept` in POSIX.
 * @return
 * New socket connection
 */
__wasi_errno_t __wasi_sock_accept(
    /**
     * The listening socket.
     */
    __wasi_fd_t fd,
    /**
     * The desired values of the file descriptor flags.
     */
    __wasi_fdflags_t flags,
    __wasi_fd_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Receive a message from a socket.
 * Note: This is similar to `recv` in POSIX, though it also supports reading
 * the data into multiple buffers in the manner of `readv`.
 * @return
 * Number of bytes stored in ri_data and message flags.
 */
__wasi_errno_t __wasi_sock_recv(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors to which to store data.
     */
    const __wasi_iovec_t *ri_data,
    /**
     * The length of the array pointed to by `ri_data`.
     */
    size_t ri_data_len,
    /**
     * Message flags.
     */
    __wasi_riflags_t ri_flags,
    __wasi_size_t *retptr0,
    __wasi_roflags_t *retptr1
) __attribute__((__warn_unused_result__));

/**
 * Send a message on a socket.
 * Note: This is similar to `send` in POSIX, though it also supports writing
 * the data from multiple buffers in the manner of `writev`.
 * @return
 * Number of bytes transmitted.
 */
__wasi_errno_t __wasi_sock_send(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors to which to retrieve data
     */
    const __wasi_ciovec_t *si_data,
    /**
     * The length of the array pointed to by `si_data`.
     */
    size_t si_data_len,
    /**
     * Message flags.
     */
    __wasi_siflags_t si_flags,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));

/**
 * Shut down socket send and receive channels.
 * Note: This is similar to `shutdown` in POSIX.
 */
__wasi_errno_t __wasi_sock_shutdown(
    __wasi_fd_t fd,
    /**
     * Which channels on the socket to shut down.
     */
    __wasi_sdflags_t how
) __attribute__((__warn_unused_result__));

/** @} */

// clang-format on
