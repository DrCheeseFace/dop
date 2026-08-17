#!/bin/sh
set -e

for arg in "$@"; do
    case "$arg" in
        (debug|build-debug|build-test-debug|test-debug)
            export BUILD_TYPE="debug"
            ;;
    esac
done

. ./config.sh

exec ${MAKE:-make} "$@"
