#!/usr/bin/env bash

my_dir="$(dirname "$(realpath "$0")")"
cw_dir="$(pwd)"

: "${home_dir:="${my_dir}/.docker-home"}"
: "${image_name:="qost-builder"}"
: "${image_tag:=latest}"
: "${image:="${image_name}:${image_tag}"}"
: "${verbose:=false}"

trace() {
    if [[ "${verbose}" == true ]]; then
        (
            set -x
            "$@"
        )
    else
        "$@"
    fi
}

if [[ "$1" == "build" ]]; then
    trace podman build -t "${image_name}" .
    exit 0
fi

if [[ ! -d "${home_dir}" ]]; then
    mkdir -p "${home_dir}"
fi

# Global variables possibly set outside this script
# shellcheck disable=2154
trace podman run \
    --rm \
    --tty \
    --interactive \
    --security-opt label=disable \
    --env HOME="${my_dir}/.docker-home" \
    --userns keep-id \
    --group-add sudo \
    --volume "${my_dir}":"${my_dir}" \
    --workdir "${cw_dir}" \
    "${image_name}" "$@"
