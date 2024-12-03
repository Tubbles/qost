#pragma once

#include "inc.hh"

// clang-format off

static_assert(alignof(int8_t) == 1, "non-wasi data layout");
static_assert(alignof(uint8_t) == 1, "non-wasi data layout");
static_assert(alignof(int16_t) == 2, "non-wasi data layout");
static_assert(alignof(uint16_t) == 2, "non-wasi data layout");
static_assert(alignof(int32_t) == 4, "non-wasi data layout");
static_assert(alignof(uint32_t) == 4, "non-wasi data layout");
static_assert(alignof(int64_t) == 8, "non-wasi data layout");
static_assert(alignof(uint64_t) == 8, "non-wasi data layout");
// static_assert(alignof(void*) == 4, "non-wasi data layout");

#define __WASI_DIRCOOKIE_START (UINT64_C(0))
typedef uint32_t __wasi_size_t;

static_assert(sizeof(__wasi_size_t) == 4, "witx calculated size");
static_assert(alignof(__wasi_size_t) == 4, "witx calculated align");

/**
 * Non-negative file size or length of a region within a file.
 */
typedef uint64_t __wasi_filesize_t;

static_assert(sizeof(__wasi_filesize_t) == 8, "witx calculated size");
static_assert(alignof(__wasi_filesize_t) == 8, "witx calculated align");

/**
 * Timestamp in nanoseconds.
 */
typedef uint64_t __wasi_timestamp_t;

static_assert(sizeof(__wasi_timestamp_t) == 8, "witx calculated size");
static_assert(alignof(__wasi_timestamp_t) == 8, "witx calculated align");

/**
 * Identifiers for clocks.
 */
typedef uint32_t __wasi_clockid_t;

// clang-format on
enum class ClockID : __wasi_clockid_t {
    realtime,  // The clock measuring real time. Time value zero corresponds with 1970-01-01T00:00:00Z.
    monotonic, // The store-wide monotonic clock, which is defined as a clock measuring real time, whose value cannot be
               // adjusted and which cannot have negative clock jumps. The epoch of this clock is undefined. The
               // absolute time value of this clock therefore has no meaning.
    process_cputime_id, // The CPU-time clock associated with the current process.
    thread_cputime_id,  // The CPU-time clock associated with the current thread.
};
// clang-format off

static_assert(sizeof(__wasi_clockid_t) == 4, "witx calculated size");
static_assert(alignof(__wasi_clockid_t) == 4, "witx calculated align");

/**
 * Error codes returned by functions.
 * Not all of these error codes are returned by the functions provided by this
 * API; some are used in higher-level library layers, and others are provided
 * merely for alignment with POSIX.
 */
typedef uint16_t __wasi_errno_t;

// clang-format on
enum class Errno : __wasi_errno_t {
    e_success        = 0,  // No error occurred. System call completed successfully.
    e_2big           = 1,  // Argument list too long.
    e_acces          = 2,  // Permission denied.
    e_addrinuse      = 3,  // Address in use.
    e_addrnotavail   = 4,  // Address not available.
    e_afnosupport    = 5,  // Address family not supported.
    e_again          = 6,  // Resource unavailable, or operation would block.
    e_already        = 7,  // Connection already in progress.
    e_badf           = 8,  // Bad file descriptor.
    e_badmsg         = 9,  // Bad message.
    e_busy           = 10, // Device or resource busy.
    e_canceled       = 11, // Operation canceled.
    e_child          = 12, // No child processes.
    e_connaborted    = 13, // Connection aborted.
    e_connrefused    = 14, // Connection refused.
    e_connreset      = 15, // Connection reset.
    e_deadlk         = 16, // Resource deadlock would occur.
    e_destaddrreq    = 17, // Destination address required.
    e_dom            = 18, // Mathematics argument out of domain of function.
    e_dquot          = 19, // Reserved.
    e_exist          = 20, // File exists.
    e_fault          = 21, // Bad address.
    e_fbig           = 22, // File too large.
    e_hostunreach    = 23, // Host is unreachable.
    e_idrm           = 24, // Identifier removed.
    e_ilseq          = 25, // Illegal byte sequence.
    e_inprogress     = 26, // Operation in progress.
    e_intr           = 27, // Interrupted function.
    e_inval          = 28, // Invalid argument.
    e_io             = 29, // I/O error.
    e_isconn         = 30, // Socket is connected.
    e_isdir          = 31, // Is a directory.
    e_loop           = 32, // Too many levels of symbolic links.
    e_mfile          = 33, // File descriptor value too large.
    e_mlink          = 34, // Too many links.
    e_msgsize        = 35, // Message too large.
    e_multihop       = 36, // Reserved.
    e_nametoolong    = 37, // Filename too long.
    e_netdown        = 38, // Network is down.
    e_netreset       = 39, // Connection aborted by network.
    e_netunreach     = 40, // Network unreachable.
    e_nfile          = 41, // Too many files open in system.
    e_nobufs         = 42, // No buffer space available.
    e_nodev          = 43, // No such device.
    e_noent          = 44, // No such file or directory.
    e_noexec         = 45, // Executable file format error.
    e_nolck          = 46, // No locks available.
    e_nolink         = 47, // Reserved.
    e_nomem          = 48, // Not enough space.
    e_nomsg          = 49, // No message of the desired type.
    e_noprotoopt     = 50, // Protocol not available.
    e_nospc          = 51, // No space left on device.
    e_nosys          = 52, // Function not supported.
    e_notconn        = 53, // The socket is not connected.
    e_notdir         = 54, // Not a directory or a symbolic link to a directory.
    e_notempty       = 55, // Directory not empty.
    e_notrecoverable = 56, // State not recoverable.
    e_notsock        = 57, // Not a socket.
    e_notsup         = 58, // Not supported, or operation not supported on socket.
    e_notty          = 59, // Inappropriate I/O control operation.
    e_nxio           = 60, // No such device or address.
    e_overflow       = 61, // Value too large to be stored in data type.
    e_ownerdead      = 62, // Previous owner died.
    e_perm           = 63, // Operation not permitted.
    e_pipe           = 64, // Broken pipe.
    e_proto          = 65, // Protocol error.
    e_protonosupport = 66, // Protocol not supported.
    e_prototype      = 67, // Protocol wrong type for socket.
    e_range          = 68, // Result too large.
    e_rofs           = 69, // Read-only file system.
    e_spipe          = 70, // Invalid seek.
    e_srch           = 71, // No such process.
    e_stale          = 72, // Reserved.
    e_timedout       = 73, // Connection timed out.
    e_txtbsy         = 74, // Text file busy.
    e_xdev           = 75, // Cross-device link.
    e_notcapable     = 76, // Extension: Capabilities insufficient.
};
// clang-format off

static_assert(sizeof(__wasi_errno_t) == 2, "witx calculated size");
static_assert(alignof(__wasi_errno_t) == 2, "witx calculated align");

/**
 * File descriptor rights, determining which actions may be performed.
 */
typedef uint64_t __wasi_rights_t;

// clang-format on
enum class Rights : __wasi_rights_t {
    fd_datasync = 1 << 0, // The right to invoke `fd_datasync`. If `path_open` is set, includes the right to invoke
                          // `path_open` with `fdflags::dsync`.
    fd_read = 1 << 1, // The right to invoke `fd_read` and `sock_recv`. If `rights::fd_seek` is set, includes the right
                      // to invoke `fd_pread`.
    fd_seek             = 1 << 2, // The right to invoke `fd_seek`. This flag implies `rights::fd_tell`.
    fd_fdstat_set_flags = 1 << 3, // The right to invoke `fd_fdstat_set_flags`.
    fd_sync = 1 << 4,  // The right to invoke `fd_sync`. If `path_open` is set, includes the right to invoke `path_open`
                       // with `fdflags::rsync` and `fdflags::dsync`.
    fd_tell = 1 << 5,  // The right to invoke `fd_seek` in such a way that the file offset remains unaltered (i.e.,
                       // `whence::cur` with offset zero), or to invoke `fd_tell`.
    fd_write = 1 << 6, // The right to invoke `fd_write` and `sock_send`. If `rights::fd_seek` is set, includes the
                       // right to invoke `fd_pwrite`.
    fd_advise             = 1 << 7,  // The right to invoke `fd_advise`.
    fd_allocate           = 1 << 8,  // The right to invoke `fd_allocate`.
    path_create_directory = 1 << 9,  // The right to invoke `path_create_directory`.
    path_create_file      = 1 << 10, // If `path_open` is set, the right to invoke `path_open` with `oflags::creat`.
    path_link_source   = 1 << 11, // The right to invoke `path_link` with the file descriptor as the source directory.
    path_link_target   = 1 << 12, // The right to invoke `path_link` with the file descriptor as the target directory.
    path_open          = 1 << 13, // The right to invoke `path_open`.
    fd_readdir         = 1 << 14, // The right to invoke `fd_readdir`.
    path_readlink      = 1 << 15, // The right to invoke `path_readlink`.
    path_rename_source = 1 << 16, // The right to invoke `path_rename` with the file descriptor as the source directory.
    path_rename_target = 1 << 17, // The right to invoke `path_rename` with the file descriptor as the target directory.
    path_filestat_get  = 1 << 18, // The right to invoke `path_filestat_get`.
    path_filestat_set_size = 1 << 19,  // The right to change a file's size (there is no `path_filestat_set_size`). If
                                       // `path_open` is set, includes the right to invoke `path_open` with
                                       // `oflags::trunc`.
    path_filestat_set_times = 1 << 20, // The right to invoke `path_filestat_set_times`.
    fd_filestat_get         = 1 << 21, // The right to invoke `fd_filestat_get`.
    fd_filestat_set_size    = 1 << 22, // The right to invoke `fd_filestat_set_size`.
    fd_filestat_set_times   = 1 << 23, // The right to invoke `fd_filestat_set_times`.
    path_symlink            = 1 << 24, // The right to invoke `path_symlink`.
    path_remove_directory   = 1 << 25, // The right to invoke `path_remove_directory`.
    path_unlink_file        = 1 << 26, // The right to invoke `path_unlink_file`.
    poll_fd_readwrite = 1 << 27, // If `rights::fd_read` is set, includes the right to invoke `poll_oneoff` to subscribe
                                 // to `eventtype::fd_read`. If `rights::fd_write` is set, includes the right to invoke
                                 // `poll_oneoff` to subscribe to `eventtype::fd_write`.
    sock_shutdown = 1 << 28,     // The right to invoke `sock_shutdown`.
    sock_accept   = 1 << 29,     // The right to invoke `sock_accept`.
};
// clang-format off

/**
 * A file descriptor handle.
 */
typedef int __wasi_fd_t;

static_assert(sizeof(__wasi_fd_t) == 4, "witx calculated size");
static_assert(alignof(__wasi_fd_t) == 4, "witx calculated align");

/**
 * A region of memory for scatter/gather reads.
 */
typedef struct __wasi_iovec_t {
    /**
     * The address of the buffer to be filled.
     */
    uint8_t * buf;

    /**
     * The length of the buffer to be filled.
     */
    __wasi_size_t buf_len;

} __wasi_iovec_t;

// static_assert(sizeof(__wasi_iovec_t) == 8, "witx calculated size");
// static_assert(alignof(__wasi_iovec_t) == 4, "witx calculated align");
// static_assert(offsetof(__wasi_iovec_t, buf) == 0, "witx calculated offset");
// static_assert(offsetof(__wasi_iovec_t, buf_len) == 4, "witx calculated offset");

/**
 * A region of memory for scatter/gather writes.
 */
typedef struct __wasi_ciovec_t {
    /**
     * The address of the buffer to be written.
     */
    const uint8_t * buf;

    /**
     * The length of the buffer to be written.
     */
    __wasi_size_t buf_len;

} __wasi_ciovec_t;

// static_assert(sizeof(__wasi_ciovec_t) == 8, "witx calculated size");
// static_assert(alignof(__wasi_ciovec_t) == 4, "witx calculated align");
// static_assert(offsetof(__wasi_ciovec_t, buf) == 0, "witx calculated offset");
// static_assert(offsetof(__wasi_ciovec_t, buf_len) == 4, "witx calculated offset");

/**
 * Relative offset within a file.
 */
typedef int64_t __wasi_filedelta_t;

static_assert(sizeof(__wasi_filedelta_t) == 8, "witx calculated size");
static_assert(alignof(__wasi_filedelta_t) == 8, "witx calculated align");

/**
 * The position relative to which to set the offset of the file descriptor.
 */
typedef uint8_t __wasi_whence_t;

// clang-format on
enum class Whence : __wasi_whence_t {
    set = 0, // Seek relative to start-of-file.
    cur = 1, // Seek relative to current position.
    end = 2, // Seek relative to end-of-file.
};
// clang-format off

static_assert(sizeof(__wasi_whence_t) == 1, "witx calculated size");
static_assert(alignof(__wasi_whence_t) == 1, "witx calculated align");

/**
 * A reference to the offset of a directory entry.
 *
 * The value 0 signifies the start of the directory.
 */
typedef uint64_t __wasi_dircookie_t;

static_assert(sizeof(__wasi_dircookie_t) == 8, "witx calculated size");
static_assert(alignof(__wasi_dircookie_t) == 8, "witx calculated align");

/**
 * The type for the `dirent::d_namlen` field of `dirent` struct.
 */
typedef uint32_t __wasi_dirnamlen_t;

static_assert(sizeof(__wasi_dirnamlen_t) == 4, "witx calculated size");
static_assert(alignof(__wasi_dirnamlen_t) == 4, "witx calculated align");

/**
 * File serial number that is unique within its file system.
 */
typedef uint64_t __wasi_inode_t;

static_assert(sizeof(__wasi_inode_t) == 8, "witx calculated size");
static_assert(alignof(__wasi_inode_t) == 8, "witx calculated align");

/**
 * The type of a file descriptor or file.
 */
typedef uint8_t __wasi_filetype_t;

// clang-format on
enum class Filetype : __wasi_filetype_t {
    unknown = 0, // The type of the file descriptor or file is unknown or is different from any of the other types
                 // specified.
    block_device     = 1, // The file descriptor or file refers to a block device inode.
    character_device = 2, // The file descriptor or file refers to a character device inode.
    directory        = 3, // The file descriptor or file refers to a directory inode.
    regular_file     = 4, // The file descriptor or file refers to a regular file inode.
    socket_dgram     = 5, // The file descriptor or file refers to a datagram socket.
    socket_stream    = 6, // The file descriptor or file refers to a byte-stream socket.
    symbolic_link    = 7, // The file refers to a symbolic link inode.
};
// clang-format off

static_assert(sizeof(__wasi_filetype_t) == 1, "witx calculated size");
static_assert(alignof(__wasi_filetype_t) == 1, "witx calculated align");

/**
 * A directory entry.
 */
typedef struct __wasi_dirent_t {
    /**
     * The offset of the next directory entry stored in this directory.
     */
    __wasi_dircookie_t d_next;

    /**
     * The serial number of the file referred to by this directory entry.
     */
    __wasi_inode_t d_ino;

    /**
     * The length of the name of the directory entry.
     */
    __wasi_dirnamlen_t d_namlen;

    /**
     * The type of the file referred to by this directory entry.
     */
    __wasi_filetype_t d_type;

} __wasi_dirent_t;

static_assert(sizeof(__wasi_dirent_t) == 24, "witx calculated size");
static_assert(alignof(__wasi_dirent_t) == 8, "witx calculated align");
static_assert(offsetof(__wasi_dirent_t, d_next) == 0, "witx calculated offset");
static_assert(offsetof(__wasi_dirent_t, d_ino) == 8, "witx calculated offset");
static_assert(offsetof(__wasi_dirent_t, d_namlen) == 16, "witx calculated offset");
static_assert(offsetof(__wasi_dirent_t, d_type) == 20, "witx calculated offset");

/**
 * File or memory access pattern advisory information.
 */
typedef uint8_t __wasi_advice_t;

// clang-format on
enum class Advice : __wasi_advice_t {
    normal     = 0, // The application has no advice to give on its behavior with respect to the specified data.
    sequential = 1, // The application expects to access the specified data sequentially from lower offsets to higher
                    // offsets.
    random   = 2,   // The application expects to access the specified data in a random order.
    willneed = 3,   // The application expects to access the specified data in the near future.
    dontneed = 4,   // The application expects that it will not access the specified data in the near future.
    noreuse  = 5,   // The application expects to access the specified data once and then not reuse it thereafter.
};
// clang-format off


static_assert(sizeof(__wasi_advice_t) == 1, "witx calculated size");
static_assert(alignof(__wasi_advice_t) == 1, "witx calculated align");

/**
 * File descriptor flags.
 */
typedef __wasi_errno_t __wasi_fdflags_t;

// clang-format on
enum class FDFlags : __wasi_fdflags_t {
    append = 1 << 0, // Append mode: Data written to the file is always appended to the file's end.
    dsync  = 1 << 1, // Write according to synchronized I/O data integrity completion. Only the data stored in the file
                     // is synchronized.
    nonblock = 1 << 2, // Non-blocking mode.
    rsync    = 1 << 3, // Synchronized read I/O operations.
    sync = 1 << 4, // Write according to synchronized I/O file integrity completion. In addition to synchronizing the
                   // data stored in the file, the implementation may also synchronously update the file's metadata.
};
// clang-format off


/**
 * File descriptor attributes.
 */
typedef struct __wasi_fdstat_t {
    /**
     * File type.
     */
    __wasi_filetype_t fs_filetype;

    /**
     * File descriptor flags.
     */
    __wasi_fdflags_t fs_flags;

    /**
     * Rights that apply to this file descriptor.
     */
    __wasi_rights_t fs_rights_base;

    /**
     * Maximum set of rights that may be installed on new file descriptors that
     * are created through this file descriptor, e.g., through `path_open`.
     */
    __wasi_rights_t fs_rights_inheriting;

} __wasi_fdstat_t;

static_assert(sizeof(__wasi_fdstat_t) == 24, "witx calculated size");
static_assert(alignof(__wasi_fdstat_t) == 8, "witx calculated align");
static_assert(offsetof(__wasi_fdstat_t, fs_filetype) == 0, "witx calculated offset");
static_assert(offsetof(__wasi_fdstat_t, fs_flags) == 2, "witx calculated offset");
static_assert(offsetof(__wasi_fdstat_t, fs_rights_base) == 8, "witx calculated offset");
static_assert(offsetof(__wasi_fdstat_t, fs_rights_inheriting) == 16, "witx calculated offset");

/**
 * Identifier for a device containing a file system. Can be used in combination
 * with `inode` to uniquely identify a file or directory in the filesystem.
 */
typedef uint64_t __wasi_device_t;

static_assert(sizeof(__wasi_device_t) == 8, "witx calculated size");
static_assert(alignof(__wasi_device_t) == 8, "witx calculated align");

/**
 * Which file time attributes to adjust.
 */
typedef __wasi_errno_t __wasi_fstflags_t;

// clang-format on
enum class FStFlags : __wasi_fstflags_t {
    atim     = 1 << 0, // Adjust the last data access timestamp to the value stored in `filestat::atim`.
    atim_now = 1 << 1, // Adjust the last data access timestamp to the time of clock `clockid::realtime`.
    mtim     = 1 << 2, // Adjust the last data modification timestamp to the value stored in `filestat::mtim`.
    mtim_now = 1 << 3, // Adjust the last data modification timestamp to the time of clock `clockid::realtime`.
};
// clang-format off

/**
 * Flags determining the method of how paths are resolved.
 */
typedef uint32_t __wasi_lookupflags_t;

// clang-format on
enum class LookupFlags : __wasi_lookupflags_t {
    symlink_follow = 1 << 0, // As long as the resolved path corresponds to a symbolic link, it is expanded.
};
// clang-format off

/**
 * Open flags used by `path_open`.
 */
typedef __wasi_errno_t __wasi_oflags_t;

// clang-format on
enum class OFlags : __wasi_oflags_t {
    creat     = 1 << 0, // Create file if it does not exist.
    directory = 1 << 1, // Fail if not a directory.
    excl      = 1 << 2, // Fail if file already exists.
    trunc     = 1 << 3, // Truncate file to size 0.
};
// clang-format off

/**
 * Number of hard links to an inode.
 */
typedef uint64_t __wasi_linkcount_t;

static_assert(sizeof(__wasi_linkcount_t) == 8, "witx calculated size");
static_assert(alignof(__wasi_linkcount_t) == 8, "witx calculated align");

/**
 * File attributes.
 */
typedef struct __wasi_filestat_t {
    /**
     * Device ID of device containing the file.
     */
    __wasi_device_t dev;

    /**
     * File serial number.
     */
    __wasi_inode_t ino;

    /**
     * File type.
     */
    __wasi_filetype_t filetype;

    /**
     * Number of hard links to the file.
     */
    __wasi_linkcount_t nlink;

    /**
     * For regular files, the file size in bytes. For symbolic links, the length in bytes of the pathname contained in the symbolic link.
     */
    __wasi_filesize_t size;

    /**
     * Last data access timestamp.
     */
    __wasi_timestamp_t atim;

    /**
     * Last data modification timestamp.
     */
    __wasi_timestamp_t mtim;

    /**
     * Last file status change timestamp.
     */
    __wasi_timestamp_t ctim;

} __wasi_filestat_t;

static_assert(sizeof(__wasi_filestat_t) == 64, "witx calculated size");
static_assert(alignof(__wasi_filestat_t) == 8, "witx calculated align");
static_assert(offsetof(__wasi_filestat_t, dev) == 0, "witx calculated offset");
static_assert(offsetof(__wasi_filestat_t, ino) == 8, "witx calculated offset");
static_assert(offsetof(__wasi_filestat_t, filetype) == 16, "witx calculated offset");
static_assert(offsetof(__wasi_filestat_t, nlink) == 24, "witx calculated offset");
static_assert(offsetof(__wasi_filestat_t, size) == 32, "witx calculated offset");
static_assert(offsetof(__wasi_filestat_t, atim) == 40, "witx calculated offset");
static_assert(offsetof(__wasi_filestat_t, mtim) == 48, "witx calculated offset");
static_assert(offsetof(__wasi_filestat_t, ctim) == 56, "witx calculated offset");

/**
 * User-provided value that may be attached to objects that is retained when
 * extracted from the implementation.
 */
typedef uint64_t __wasi_userdata_t;

static_assert(sizeof(__wasi_userdata_t) == 8, "witx calculated size");
static_assert(alignof(__wasi_userdata_t) == 8, "witx calculated align");

/**
 * Type of a subscription to an event or its occurrence.
 */
typedef uint8_t __wasi_eventtype_t;

// clang-format on
enum class EventType : __wasi_eventtype_t {
    clock = 0, // The time value of clock `subscription_clock::id` has reached timestamp `subscription_clock::timeout`.
    fd_read = 1,  // File descriptor `subscription_fd_readwrite::file_descriptor` has data available for reading. This
                  // event always triggers for regular files.
    fd_write = 2, // File descriptor `subscription_fd_readwrite::file_descriptor` has capacity available for writing.
                  // This event always triggers for regular files.
};
// clang-format off


static_assert(sizeof(__wasi_eventtype_t) == 1, "witx calculated size");
static_assert(alignof(__wasi_eventtype_t) == 1, "witx calculated align");

/**
 * The state of the file descriptor subscribed to with
 * `eventtype::fd_read` or `eventtype::fd_write`.
 */
typedef __wasi_errno_t __wasi_eventrwflags_t;

// clang-format on
enum class EventRWFlags : __wasi_eventrwflags_t {
    fd_readwrite_hangup = 1 << 0, // The peer of this socket has closed or disconnected.
};
// clang-format off

/**
 * The contents of an `event` when type is `eventtype::fd_read` or
 * `eventtype::fd_write`.
 */
typedef struct __wasi_event_fd_readwrite_t {
    /**
     * The number of bytes available for reading or writing.
     */
    __wasi_filesize_t nbytes;

    /**
     * The state of the file descriptor.
     */
    __wasi_eventrwflags_t flags;

} __wasi_event_fd_readwrite_t;

static_assert(sizeof(__wasi_event_fd_readwrite_t) == 16, "witx calculated size");
static_assert(alignof(__wasi_event_fd_readwrite_t) == 8, "witx calculated align");
static_assert(offsetof(__wasi_event_fd_readwrite_t, nbytes) == 0, "witx calculated offset");
static_assert(offsetof(__wasi_event_fd_readwrite_t, flags) == 8, "witx calculated offset");

/**
 * An event that occurred.
 */
typedef struct __wasi_event_t {
    /**
     * User-provided value that got attached to `subscription::userdata`.
     */
    __wasi_userdata_t userdata;

    /**
     * If non-zero, an error that occurred while processing the subscription request.
     */
    __wasi_errno_t error;

    /**
     * The type of event that occured
     */
    __wasi_eventtype_t type;

    /**
     * The contents of the event, if it is an `eventtype::fd_read` or
     * `eventtype::fd_write`. `eventtype::clock` events ignore this field.
     */
    __wasi_event_fd_readwrite_t fd_readwrite;

} __wasi_event_t;

static_assert(sizeof(__wasi_event_t) == 32, "witx calculated size");
static_assert(alignof(__wasi_event_t) == 8, "witx calculated align");
static_assert(offsetof(__wasi_event_t, userdata) == 0, "witx calculated offset");
static_assert(offsetof(__wasi_event_t, error) == 8, "witx calculated offset");
static_assert(offsetof(__wasi_event_t, type) == 10, "witx calculated offset");
static_assert(offsetof(__wasi_event_t, fd_readwrite) == 16, "witx calculated offset");

/**
 * Flags determining how to interpret the timestamp provided in
 * `subscription_clock::timeout`.
 */
typedef __wasi_errno_t __wasi_subclockflags_t;

// clang-format on
enum class SubClockFlags : __wasi_subclockflags_t {
    subscription_clock_abstime = 1 << 0, // If set, treat the timestamp provided in `subscription_clock::timeout` as an
                                         // absolute timestamp of clock `subscription_clock::id`. If clear, treat the
                                         // timestamp provided in `subscription_clock::timeout` relative to the current
                                         // time value of clock `subscription_clock::id`.
};
// clang-format off


/**
 * The contents of a `subscription` when type is `eventtype::clock`.
 */
typedef struct __wasi_subscription_clock_t {
    /**
     * The clock against which to compare the timestamp.
     */
    __wasi_clockid_t id;

    /**
     * The absolute or relative timestamp.
     */
    __wasi_timestamp_t timeout;

    /**
     * The amount of time that the implementation may wait additionally
     * to coalesce with other events.
     */
    __wasi_timestamp_t precision;

    /**
     * Flags specifying whether the timeout is absolute or relative
     */
    __wasi_subclockflags_t flags;

} __wasi_subscription_clock_t;

static_assert(sizeof(__wasi_subscription_clock_t) == 32, "witx calculated size");
static_assert(alignof(__wasi_subscription_clock_t) == 8, "witx calculated align");
static_assert(offsetof(__wasi_subscription_clock_t, id) == 0, "witx calculated offset");
static_assert(offsetof(__wasi_subscription_clock_t, timeout) == 8, "witx calculated offset");
static_assert(offsetof(__wasi_subscription_clock_t, precision) == 16, "witx calculated offset");
static_assert(offsetof(__wasi_subscription_clock_t, flags) == 24, "witx calculated offset");

/**
 * The contents of a `subscription` when type is type is
 * `eventtype::fd_read` or `eventtype::fd_write`.
 */
typedef struct __wasi_subscription_fd_readwrite_t {
    /**
     * The file descriptor on which to wait for it to become ready for reading or writing.
     */
    __wasi_fd_t file_descriptor;

} __wasi_subscription_fd_readwrite_t;

static_assert(sizeof(__wasi_subscription_fd_readwrite_t) == 4, "witx calculated size");
static_assert(alignof(__wasi_subscription_fd_readwrite_t) == 4, "witx calculated align");
static_assert(offsetof(__wasi_subscription_fd_readwrite_t, file_descriptor) == 0, "witx calculated offset");

/**
 * The contents of a `subscription`.
 */
typedef union __wasi_subscription_u_u_t {
    __wasi_subscription_clock_t clock;
    __wasi_subscription_fd_readwrite_t fd_read;
    __wasi_subscription_fd_readwrite_t fd_write;
} __wasi_subscription_u_u_t;
typedef struct __wasi_subscription_u_t {
    uint8_t tag;
    __wasi_subscription_u_u_t u;
} __wasi_subscription_u_t;

static_assert(sizeof(__wasi_subscription_u_t) == 40, "witx calculated size");
static_assert(alignof(__wasi_subscription_u_t) == 8, "witx calculated align");

/**
 * Subscription to an event.
 */
typedef struct __wasi_subscription_t {
    /**
     * User-provided value that is attached to the subscription in the
     * implementation and returned through `event::userdata`.
     */
    __wasi_userdata_t userdata;

    /**
     * The type of the event to which to subscribe, and its contents
     */
    __wasi_subscription_u_t u;

} __wasi_subscription_t;

static_assert(sizeof(__wasi_subscription_t) == 48, "witx calculated size");
static_assert(alignof(__wasi_subscription_t) == 8, "witx calculated align");
static_assert(offsetof(__wasi_subscription_t, userdata) == 0, "witx calculated offset");
static_assert(offsetof(__wasi_subscription_t, u) == 8, "witx calculated offset");

/**
 * Exit code generated by a process when exiting.
 */
typedef uint32_t __wasi_exitcode_t;

static_assert(sizeof(__wasi_exitcode_t) == 4, "witx calculated size");
static_assert(alignof(__wasi_exitcode_t) == 4, "witx calculated align");

/**
 * Flags provided to `sock_recv`.
 */
typedef __wasi_errno_t __wasi_riflags_t;

// clang-format on
enum class RiFlags : __wasi_riflags_t {
    recv_peek    = 1 << 0, // Returns the message without removing it from the socket's receive queue.
    recv_waitall = 1 << 1, // On byte-stream sockets, block until the full amount of data can be returned.
};
// clang-format off

/**
 * Flags returned by `sock_recv`.
 */
typedef __wasi_errno_t __wasi_roflags_t;

// clang-format on
enum class RoFlags : __wasi_roflags_t {
    recv_data_truncated = 1 << 0, // Returned by `sock_recv`: Message data has been truncated.
};
// clang-format off

/**
 * Flags provided to `sock_send`. As there are currently no flags
 * defined, it must be set to zero.
 */
typedef __wasi_errno_t __wasi_siflags_t;

static_assert(sizeof(__wasi_siflags_t) == 2, "witx calculated size");
static_assert(alignof(__wasi_siflags_t) == 2, "witx calculated align");

/**
 * Which channels on a socket to shut down.
 */
typedef uint8_t __wasi_sdflags_t;

// clang-format on
enum class SdFlags : __wasi_sdflags_t {
    rd = 1 << 0, // Disables further receive operations.
    wr = 1 << 1, // Disables further send operations.
};
// clang-format off

/**
 * Identifiers for preopened capabilities.
 */
typedef uint8_t __wasi_preopentype_t;

// clang-format on
enum class PreopenType : __wasi_preopentype_t {
    dir = 0, // A pre-opened directory.
};
// clang-format off

static_assert(sizeof(__wasi_preopentype_t) == 1, "witx calculated size");
static_assert(alignof(__wasi_preopentype_t) == 1, "witx calculated align");

/**
 * The contents of a $prestat when type is `preopentype::dir`.
 */
typedef struct __wasi_prestat_dir_t {
    /**
     * The length of the directory name for use with `fd_prestat_dir_name`.
     */
    __wasi_size_t pr_name_len;

} __wasi_prestat_dir_t;

static_assert(sizeof(__wasi_prestat_dir_t) == 4, "witx calculated size");
static_assert(alignof(__wasi_prestat_dir_t) == 4, "witx calculated align");
static_assert(offsetof(__wasi_prestat_dir_t, pr_name_len) == 0, "witx calculated offset");

/**
 * Information about a pre-opened capability.
 */
typedef union __wasi_prestat_u_t {
    __wasi_prestat_dir_t dir;
} __wasi_prestat_u_t;
typedef struct __wasi_prestat_t {
    uint8_t tag;
    __wasi_prestat_u_t u;
} __wasi_prestat_t;

static_assert(sizeof(__wasi_prestat_t) == 8, "witx calculated size");
static_assert(alignof(__wasi_prestat_t) == 4, "witx calculated align");
