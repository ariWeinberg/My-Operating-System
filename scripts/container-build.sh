#!/bin/sh
set -eu

readonly source_dir=/src
readonly work_dir=/work/source
readonly epoch="${SOURCE_DATE_EPOCH:-1686528000}"

if [ ! -f "${source_dir}/makefile" ]; then
    echo "error: mount the repository at /src" >&2
    exit 2
fi

rm -rf "${work_dir}"
mkdir -p "${work_dir}"
cp -a "${source_dir}/." "${work_dir}/"
rm -rf     "${work_dir}/.git"     "${work_dir}/build"     "${work_dir}/out"     "${work_dir}/kernel/build"     "${work_dir}/qemu.log"

# Git does not preserve mtimes. Normalize every input before mtools copies files
# into FAT directory entries, then freeze wall-clock reads during image creation.
find "${work_dir}" -exec touch -h -d "@${epoch}" {} +

cd "${work_dir}"
faketime -f "@${epoch}" make clean
faketime -f "@${epoch}" make

test "$(stat -c %s out/disk.img)" -eq 16777216
sha256sum out/disk.img > out/SHA256SUMS

{
    echo "source_date_epoch=${epoch}"
    echo "disk_size_bytes=$(stat -c %s out/disk.img)"
    echo "disk_sha256=$(sha256sum out/disk.img | awk '{print $1}')"
    echo
    echo "[tools]"
    nasm -v
    gcc --version | sed -n '1p'
    ld --version | sed -n '1p'
    objcopy --version | sed -n '1p'
    mkfs.fat --version 2>&1 | sed -n '1p'
    mcopy -V 2>&1 | sed -n '1p'
    make --version | sed -n '1p'
    echo
    echo "[packages]"
    dpkg-query -W -f='${Package}=${Version}\n' | LC_ALL=C sort
} > out/build-manifest.txt

mkdir -p "${source_dir}/out"
cp out/disk.img out/SHA256SUMS out/build-manifest.txt "${source_dir}/out/"
