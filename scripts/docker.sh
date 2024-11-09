#!/usr/bin/env bash

set -eEo pipefail

my_dir="$(dirname "$(realpath "${0}")")"
root_dir="$(cd "${my_dir}" && git rev-parse --show-toplevel)"
script_dir="${root_dir}/scripts"
source "${script_dir}/common.source"

: "${home_dir:="${root_dir}/.docker-home"}"
: "${image_name:="qost-builder"}"
: "${image_tag:="latest"}"
: "${image:="${image_name}:${image_tag}"}"
: "${verbose:=false}"

if [[ "$1" == "build" ]]; then
    step podman build -t "${image_name}" "${script_dir}"
    exit 0
fi

if [[ ! -d "${home_dir}" ]]; then
    mkdir -p "${home_dir}"
fi

step podman run \
    --rm \
    --tty \
    --interactive \
    --security-opt label=disable \
    --env HOME="${home_dir}" \
    --userns keep-id \
    --group-add sudo \
    --volume "${root_dir}":"${root_dir}" \
    --workdir "$(pwd)" \
    "${image_name}" "$@"
