#!/bin/sh
set -eu

readonly image="${ARIOS_TOOLCHAIN_IMAGE:-arios-toolchain:bookworm-20230612}"
readonly repo_dir="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"

command -v docker >/dev/null 2>&1 || {
    echo "error: Docker is required" >&2
    exit 127
}

docker build --pull=false --tag "${image}" "${repo_dir}"
docker run --rm     --network none     --user "$(id -u):$(id -g)"     --volume "${repo_dir}:/src"     "${image}"
