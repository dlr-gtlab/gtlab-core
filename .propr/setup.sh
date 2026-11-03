#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
# SPDX-License-Identifier: MPL-2.0+
set -euo pipefail

workspace="${PROPR_WORKSPACE:-$(pwd)}"
build_dir="${GTLAB_BUILD_DIR:-${workspace}/build}"
install_dir="${GTLAB_INSTALL_DIR:-${workspace}/install}"
cache_dir="${PROPR_CACHE_DIR:-/tmp/propr-setup-cache/opencode}"
hdf5_root="${cache_dir}/build_deps/hdf5-1.12.1-linux"
hdf5_archive="${cache_dir}/build_deps/hdf5-1.12.1-linux.tar.gz"
hdf5_dir="${hdf5_root}/share/cmake/hdf5"

if [[ ! -f "${workspace}/CMakeLists.txt" ]]; then
    echo "GTlab CMakeLists.txt not found in ${workspace}" >&2
    exit 1
fi

for tool in cmake ninja curl tar; do
    if ! command -v "${tool}" >/dev/null 2>&1; then
        echo "Required build tool '${tool}' is missing from the ProPR agent image" >&2
        exit 1
    fi
done

# Match the HDF5 version used by GTlab's Linux CI; Debian's libhdf5-dev can
# be older than the version required by GenH5.
if [[ ! -f "${hdf5_dir}/HDF5Config.cmake" ]]; then
    mkdir -p "${cache_dir}/build_deps"
    curl --fail --location --silent --show-error --retry 3 \
        https://gtlab.de/builddeps/hdf5-1.12.1-linux.tar.gz \
        --output "${hdf5_archive}"
    tar -xzf "${hdf5_archive}" -C "${cache_dir}/build_deps"
fi

cmake -S "${workspace}" -B "${build_dir}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_UNITTESTS=ON \
    -DBUILD_TESTMODULES=ON \
    -DCMAKE_INSTALL_PREFIX="${install_dir}" \
    -DHDF5_DIR="${hdf5_dir}"

cmake --build "${build_dir}" --parallel "${GTLAB_BUILD_JOBS:-2}"
unit_test_runner="${build_dir}/GTlabUnitTest"
if [[ ! -x "${unit_test_runner}" ]]; then
    echo "GTlab unit-test runner was not built: ${unit_test_runner}" >&2
    exit 1
fi
"${unit_test_runner}"
