#!/usr/bin/env bash
# -*- coding: utf-8 -*-

my_dir="$(dirname "$(realpath "$0")")"

"${my_dir}/docker.sh" "${my_dir}/tools/build-system/build.py"
