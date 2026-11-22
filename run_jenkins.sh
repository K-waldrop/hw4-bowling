#!/bin/sh
set -eu
cd "$(dirname "$0")"
make
if ./a.out; then
    echo "Expected a missing-player-count error." >&2
    exit 1
else
    test "$?" -eq 1
fi
./a.out 2
./a.out 3
