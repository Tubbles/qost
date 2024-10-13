#!/usr/bin/env bash
# -*- coding: utf-8 -*-

set -e

usage="$0 [command...]

Facilitates building, testing, and running this project.

[command...] can be zero or more of (in any order):
* generate  - Only generate the ninja script and compile commands database, do not compile
* help      - Print this help text and exit
* pristine  - Clean the build directory before build
* release   - Build with release configuration instead of debug configuration
* execute   - Run the resulting executable after a successful build
* verbose   - Print verbose information to the terminal"

my_dir="$(dirname "$(realpath "$0")")"
project="$(basename "${my_dir}")"

build=true
clean=false
debug=true
generate=false
run=false
verbose=false

ninja_gen_args=()
ninja_args=()

# Check args
while (($# > 0)); do
    if [[ "$1" =~ ^ge?n?e?r?a?t?e?$ ]]; then
        build=false
        generate=true
    elif [[ "$1" =~ ^pr?i?s?t?i?n?e?$ ]]; then
        clean=true
    elif [[ "$1" =~ ^re?l?e?a?s?e?$ ]]; then
        debug=false
    elif [[ "$1" =~ ^ex?e?c?u?t?e?$ ]]; then
        run=true
    elif [[ "$1" =~ ^ve?r?b?o?s?e?$ ]]; then
        verbose=true
    elif [[ "$1" =~ ^he?l?p?$ ]]; then
        echo -e "Usage:\n${usage}"
        exit 0
    else
        echo -e "\nError: Unkown argument given: $1\n"
        echo -e "Usage:\n${usage}"
        exit 1
    fi
    shift
done

step() {
    if [[ "${verbose}" == true ]]; then
        (
            export verbose
            set -x
            "$@"
        )
    else
        "$@"
    fi
}

if [[ "${debug}" == true ]]; then
    build_type="debug"
else
    build_type="release"
fi

build_dir="${my_dir}/output/${build_type}"

ninja_gen_args+=(
    "-i"
    "${my_dir}/ng-config.toml"
    "-o"
    "${build_dir}/build.ninja"
)

ninja_gen_args+=(
    "--profile"
    "${build_type}"
)

if [[ "${clean}" == true ]]; then
    rm -fr "${build_dir}"
fi

if [[ ! -f "${build_dir}/build.ninja" ]]; then
    generate=true
fi

mkdir -p "${build_dir}"

if [[ "${verbose}" == true ]]; then
    ninja_gen_args+=("--verbose")
    ninja_args+=("--verbose")
fi

if [[ "${generate}" == true ]]; then
    step "${my_dir}/docker.sh" "${my_dir}/scripts/generate_ninja.py" "${ninja_gen_args[@]}"
    step "${my_dir}/docker.sh" ninja -C "${build_dir}" -t compdb >"${build_dir}/compile_commands.json"

    if [[ "${verbose}" == true ]]; then
        cat "${build_dir}/build.ninja"
    fi
fi

if [[ "${build}" == true ]]; then
    step "${my_dir}/docker.sh" ninja -C "${build_dir}" "${ninja_args[@]}" "${project}"
fi

if [[ "${run}" == true ]]; then
    if [[ "${verbose}" == true ]]; then
        echo "Running project"
    fi
    "${build_dir}/${project}"
fi
