#!/bin/bash

set -e

ROOT="$(dirname "$0")"

echo "🔨 building..."
cmake -B "$ROOT/build" -S "$ROOT" > /dev/null 2>&1
cmake --build "$ROOT/build" > /dev/null 2>&1
echo "✅ build done"

echo "🚀 starting server on port 4221..."
exec "$ROOT/build/http-server" "$@"