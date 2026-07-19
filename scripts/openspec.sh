#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
node_home="$repo_root/.tools/node-v22.23.1-linux-x64"

if [[ ! -x "$node_home/bin/node" ]]; then
  echo "Missing project Node.js toolchain: $node_home" >&2
  exit 1
fi

export PATH="$node_home/bin:$PATH"
unset NODE_TLS_REJECT_UNAUTHORIZED
exec "$repo_root/node_modules/.bin/openspec" "$@"
