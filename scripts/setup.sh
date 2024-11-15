#!/usr/bin/env bash
# -*- coding: utf-8 -*-

set -eEo pipefail

root_dir="$(git rev-parse --show-toplevel)"
script_dir="${root_dir}/scripts"
source "${script_dir}/common.source"
modules_dir="${root_dir}/src/modules"

args=("$@")
pristine=false
verbose=false
test=false

if [[ " ${args[*]} " =~ [[:space:]]"--verbose"[[:space:]] ]]; then
    verbose=true
fi

if [[ " ${args[*]} " =~ [[:space:]]"--pristine"[[:space:]] ]]; then
    pristine=true
fi

if [[ " ${args[*]} " =~ [[:space:]]"--test"[[:space:]] ]]; then
    test=true
fi

git -C "${root_dir}" submodule update --init --recursive

make_args=(
    "--no-print-directory"
    "-O"
    "-j"
)

if [[ ${verbose} == false ]]; then
    make_args+=("-s")
fi

module="doctest"
if [[ " ${args[*]} " =~ [[:space:]]${module}[[:space:]] ]]; then
    (
        echo "${module}: nothing to build"
    )
fi

module="fmt"
if [[ " ${args[*]} " =~ [[:space:]]${module}[[:space:]] ]]; then
    (
        echo "Setting up ${module}"
        cd "${modules_dir}/${module}"
        if [[ ${pristine} == true ]]; then
            rm -fr "build"
        fi

        if [[ ! -d "build" ]]; then
            mkdir -p "build"
            step cmake -B "build"
        fi

        step make -C "build" "${make_args[@]}"

        if [[ ${test} == true ]]; then
            step make -C "build" "${make_args[@]}" test
        fi
    )
fi

module="lua"
if [[ " ${args[*]} " =~ [[:space:]]${module}[[:space:]] ]]; then
    (
        echo "Setting up ${module}"
        cd "${modules_dir}/${module}"
        if [[ ${pristine} == true ]]; then
            step make clean
        fi

        echo "Building ${module}"
        step make "${make_args[@]}"
        echo "Building ${module} done"

        if [[ ${test} == true ]]; then
            cd testes/libs
            step make "${make_args[@]}"
            cd "${modules_dir}/${module}"

            step bash ./all | tee log
            result="$(tail -n3 log | head -n 1 | tr -d '\r')"
            rm log
            if [[ "${result}" != "    final OK!!!!" ]]; then false; fi
        fi
    )
fi

module="wasmer"
if [[ " ${args[*]} " =~ [[:space:]]${module}[[:space:]] ]]; then
    (
        echo "Setting up ${module}"
        mkdir -p "${modules_dir}/${module}"
        cd "${modules_dir}/${module}"

        if [[ ${pristine} == true ]]; then
            rm -fr wasmer-linux-amd64*
        fi

        if [[ ! -d "wasmer-linux-amd64" ]]; then
            if [[ ! -r "wasmer-linux-amd64.tar.gz" ]]; then
                wget https://github.com/wasmerio/wasmer/releases/download/v5.0.1/wasmer-linux-amd64.tar.gz
            fi
            mkdir -p wasmer-linux-amd64
            tar -C wasmer-linux-amd64/ -zxvf wasmer-linux-amd64.tar.gz
        fi
    )
fi

module="wren"
if [[ " ${args[*]} " =~ [[:space:]]${module}[[:space:]] ]]; then
    (
        echo "Setting up ${module}"
        cd "${modules_dir}/${module}"
        if [[ ${pristine} == true ]]; then
            step make -C projects/make clean
        fi

        step make -C projects/make "${make_args[@]}"

        if [[ ${test} == true ]]; then
            :
            # step python3 "util/test.py"
        fi
    )
fi
