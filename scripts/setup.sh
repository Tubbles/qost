#!/usr/bin/env bash
# -*- coding: utf-8 -*-

set -eEo pipefail

all_modules=(
    "doctest"
    "fmt"
    "lua"
    "wren"
)

usage="$0 [command... | module...]

Sets up all third party dependencies for this project

[command...] can be zero or more of (in any order):
* help      - Print this help text and exit
* pristine  - Clean all build directories before build
* test      - Also run all test suites
* verbose   - Print verbose information to the terminal

[module]... Build only the selected modules. Can be zero or more of (in any order):${all_modules[*]/#/\\n* }"

root_dir="$(git rev-parse --show-toplevel)"
script_dir="${root_dir}/scripts"
source "${script_dir}/common.source"
modules_dir="${root_dir}/src/modules"

clean=false
verbose=false
test=false
do_all_modules=true
selected_modules=()
make_args=(
    "-O"
    "-j"
)

# Check args
while (($# > 0)); do
    module_match=false
    for module in "${all_modules[@]}"; do
        if [[ "$1" == "${module}" ]]; then
            selected_modules+=("${module}")
            do_all_modules=false
            module_match=true
        fi
    done

    if [[ ${module_match} == true ]]; then
        shift
        continue
    fi

    if [[ "$1" =~ ^pr?i?s?t?i?n?e?$ ]]; then
        clean=true
    elif [[ "$1" =~ ^ve?r?b?o?s?e?$ ]]; then
        # We might wanna use verbose in the future
        # shellcheck disable=SC2034
        verbose=true
    elif [[ "$1" =~ ^te?s?t?$ ]]; then
        test=true
    elif [[ "$1" =~ ^he?l?p?$ ]] || [[ "$1" == "--help" ]] || [[ "$1" == "-h" ]]; then
        echo -e "Usage:\n${usage}"
        exit 0
    else
        echo -e "\nError: Unkown argument given: $1\n"
        echo -e "Usage:\n${usage}"
        exit 1
    fi
    shift
done

git -C "${root_dir}" submodule update --init --recursive

if [[ ${verbose} == false ]]; then
    make_args+=("-s")
fi

if [[ ${do_all_modules} == true ]]; then
    for module in "${all_modules[@]}"; do
        selected_modules+=("${module}")
    done
fi

for module in "${selected_modules[@]}"; do
    echo "Setting up ${module}"

    if [[ "${module}" == "doctest" ]]; then
        (
            echo "doctest: nothing to build"
        )
    elif [[ "${module}" == "fmt" ]]; then
        (
            cd "${modules_dir}/${module}"
            if [[ ${clean} == true ]]; then
                rm -fr "build"
            fi

            if [[ ! -d "build" ]]; then
                mkdir -p "build"
                docker_step cmake -B "build"
            fi

            docker_step make -C "build" "${make_args[@]}"

            if [[ ${test} == true ]]; then
                docker_step make -C "build" "${make_args[@]}" test
            fi
        )
    elif [[ "${module}" == "lua" ]]; then
        (
            cd "${modules_dir}/${module}"
            if [[ ${clean} == true ]]; then
                docker_step make clean
            fi

            docker_step make "${make_args[@]}"

            if [[ ${test} == true ]]; then
                cd testes/libs
                docker_step make "${make_args[@]}"
                cd "${modules_dir}/${module}"

                docker_step bash ./all | tee log
                result="$(tail -n3 log | head -n 1 | tr -d '\r')"
                rm log
                if [[ "${result}" != "    final OK!!!!" ]]; then false; fi
            fi
        )
    elif [[ "${module}" == "wren" ]]; then
        (
            cd "${modules_dir}/${module}"
            if [[ ${clean} == true ]]; then
                docker_step make -C projects/make clean
            fi

            docker_step make -C projects/make "${make_args[@]}"

            if [[ ${test} == true ]]; then
                :
                # docker_step python3 "util/test.py"
            fi
        )
    fi
done
