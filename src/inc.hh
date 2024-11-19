#pragma once

// #include "doctest/doctest.h"
// #include "wren.hpp"
#include "fmt/format.h"
#include "wasmer.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "luaconf.h"
#include "lualib.h"
}

#define UNUSED(x) ((void)x)
#define ARRLEN(x) (sizeof(x) / sizeof(*(x)))

#include "wasi_stubs.hh"
