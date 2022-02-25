#!/bin/bash
# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

set -o errexit
set -o pipefail
set -o nounset

# meson_init.sh configures OpenTitan's Meson build system. Rather than calling
# Meson directly, this script should be used instead, since it handles things
# that Meson does not out of the box, such as:
# - The RISC-V toolchain.
# - Idempotence.
# - Building for multiple device platforms.
#
# This script's semantics for creating build directories are as follows:
# - By default, this script will create any missing build directories and
#   configure them. It is idempotent: if there is no work to be done, nothing
#   will change and this script will exit successfully.
# - Passing -r will reconfigure existing build directories. This should be used
#   when editing meson.build files.
# - Passing -f will erase existing build directories. This should be used when
#   existing configuration is completely broken, or a clean build is desired
#   (neither of these should be necessary).
# - Passing -A will explicitly disable idempotence, and cause the script to exit
#   if a build directory already exists. This should be used only in CI, where
#   such error-checking is desireable.
#
# Note that only the first option above is actually guaranteed to be idempotent.

. util/build_consts.sh

echo "Detected \$REPO_TOP at $REPO_TOP."
echo "Object directory set at $OBJ_DIR."
echo "Binary directory set at $BIN_DIR."
echo "OpenTitan version: $OT_VERSION"
echo

function usage() {
  cat << USAGE
Configure Meson build targets.

Usage: $0 [-r|-f|-A|-K|-c] [-T PATH] [-t FILE]

  -A: Assert that no build dirs exist when running this command.
  -c: Enable coverage (requires clang).
  -f: Force a reconfiguration by removing existing build dirs.
  -K: Keep include search paths as generated by Meson.
  -r: Reconfigure build dirs, if they exist.
  -t FILE: Configure Meson with toolchain configuration FILE

USAGE
}

readonly DEFAULT_RISCV_TOOLS=/tools/riscv
TOOLCHAIN_PATH="${TOOLCHAIN_PATH:-$DEFAULT_RISCV_TOOLS}"

FLAGS_assert=false
FLAGS_force=false
FLAGS_reconfigure=""
FLAGS_keep_includes=false
FLAGS_specified_toolchain_file=false
FLAGS_coverage=false
ARG_toolchain_file="${TOOLCHAIN_PATH}/meson-riscv32-unknown-elf-clang.txt"
# `getopts` usage
# - The initial colon in the optstring is to suppress the default error
#   handling.
# - The remaining options are specified in alphabetical order, and the case
#   statement should match this order.
# - Only options that take an argument should have a following colon.
# - The case statement contains two additional cases:
#   - when `$flag` = `?`, this is an unexpected option.
#   - when `$flag` = `:`, this is the case that a flag which requires an
#     argument is not provided one. In both cases, `$OPTARG` contains the
#     relevant parsed option.
# - After option parsing is finished, we `shift` by `$OPTIND - 1` so that the
#   remaining (unprocessed) arguments are in `$@` (and $1, $2, $3 etc.).
while getopts ':AcfKrt:T:' flag; do
  case "${flag}" in
    A) FLAGS_assert=true;;
    c) FLAGS_coverage=true;;
    f) FLAGS_force=true;;
    K) FLAGS_keep_includes=true;;
    r) FLAGS_reconfigure="--reconfigure";;
    t) FLAGS_specified_toolchain_file=true
       ARG_toolchain_file="${OPTARG}";;
    \?) echo "Unexpected option: -${OPTARG}" >&2
        usage
        exit 1
        ;;
    :) echo "Option -${OPTARG} requires a path argument" >&2
       usage
       exit 1
       ;;
    *) echo "Internal Error: Unhandled option: -${flag}" >&2
       exit 1
       ;;
  esac
done
shift $((OPTIND - 1))

# We do not accept additional arguments.
if [[ "$#" -gt 0 ]]; then
  echo "Unexpected arguments:" "$@" >&2
  exit 1
fi

if [[ ! -n "$(command -v meson)" ]]; then
  echo "Unable to find meson. Please install meson before running this command." >&2
  exit 1
fi

if [[ ! -n "$(command -v ninja)" ]]; then
  echo "Unable to find ninja. Please install ninja before running this command." >&2
  exit 1
fi

if [[ "${FLAGS_force}" == true ]]; then
  rm -rf "$OBJ_DIR"
  rm -rf "$BIN_DIR"
fi

if [[ "${FLAGS_assert}" == true ]]; then
  if [[ -e "$OBJ_DIR" ]]; then
    echo "Object directory at $OBJ_DIR already exists. Aborting." >&2
    exit 1
  fi
  if [[ -e "$BIN_DIR" ]]; then
    echo "Binary directory at $BIN_DIR already exists. Aborting." >&2
    exit 1
  fi
fi

reconf="${FLAGS_reconfigure}"

if [[ ! -d "$OBJ_DIR" ]]; then
  echo "Output directory does not exist at $OBJ_DIR; creating." >&2
  mkdir -p "$OBJ_DIR"
  reconf=""
elif [[ -z "$reconf" ]]; then
  echo "Output directory already exists at $OBJ_DIR; skipping." >&2
fi

if [[ -f "${ARG_toolchain_file}" ]]; then
  echo "Using meson toolchain file at $ARG_toolchain_file." >&2
else
  if [[ "${FLAGS_specified_toolchain_file}" == true ]]; then
    echo "Unable to find meson toolchain file at $ARG_toolchain_file. Aborting." >&2
    exit 1
  else
    cross_file="$OBJ_DIR/toolchain-configured.txt"
    cp toolchain.txt "$cross_file"
    perl -pi -e "s#$DEFAULT_RISCV_TOOLS#$TOOLCHAIN_PATH#g" "$cross_file"
    touch -r toolchain.txt "$cross_file"
    echo "Set up toolchain file at $cross_file." >&2
    ARG_toolchain_file="${cross_file}"
  fi
fi

mkdir -p "$BIN_DIR"
set -x
meson $reconf \
  -Dot_version="$OT_VERSION" \
  -Dbin_dir="$BIN_DIR" \
  -Dkeep_includes="$FLAGS_keep_includes" \
  -Dcoverage="$FLAGS_coverage" \
  --cross-file="$ARG_toolchain_file" \
  --cross-file="meson-config.txt" \
  "$OBJ_DIR"
