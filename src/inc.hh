#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "fmt/format.h"
#include "fmt/ranges.h"
#include "magic_enum/magic_enum.hpp"
#include "wasmer.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "luaconf.h"
#include "lualib.h"
}

#ifdef UNIT_TEST
#include "doctest/doctest.h"
#endif

#define UNUSED(x) ((void)x)
#define ARRLEN(x) (sizeof(x) / sizeof(*(x)))

// template <class Enum>
// using tul = std::to_underlying;

#include "wasi_types.hh"

#include "wasi_stubs.hh"
