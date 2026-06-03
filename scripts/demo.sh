#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${repo_root}/build"

make -C "${repo_root}" >/dev/null

echo "$ ranedge-sim --ticks 10"
"${build_dir}/ranedge-sim" --ticks 10

echo
echo "$ ranedge-sim --ticks 8 --metrics | tail"
"${build_dir}/ranedge-sim" --ticks 8 --metrics | tail

echo
echo "$ ranedge-sim --ticks 8 --otel-logs | tail -n 3"
"${build_dir}/ranedge-sim" --ticks 8 --otel-logs | tail -n 3

echo
echo "$ ranedge-sim --ticks 4 --json"
"${build_dir}/ranedge-sim" --ticks 4 --json
