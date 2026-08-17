#!/bin/sh
set -e

export BUILD_TYPE=${BUILD_TYPE:-release}

export CC=${CC:-clang}
export CSTANDARD=${CSTANDARD:-c99}

export LLVM_CONFIG=${LLVM_CONFIG:-/home/tharun/Projects/llvm/build/bin/llvm-config}

if ! command -v "$LLVM_CONFIG" >/dev/null 2>&1; then
    echo "Error: llvm-config not found at '$LLVM_CONFIG'." >&2
    exit 1
fi

LLVM_CFLAGS=$("$LLVM_CONFIG" --cflags)
LLVM_LDFLAGS=$("$LLVM_CONFIG" --ldflags)
LLVM_LIBS=$("$LLVM_CONFIG" --libs core irreader)
LLVM_SYSLIBS=$("$LLVM_CONFIG" --system-libs)
LLVM_RPATH="-Wl,-rpath,$("$LLVM_CONFIG" --libdir)"

export INCLUDES="-Iinclude -Isrc/mr_utils/include"
export LDLIBS="-lm $LLVM_LIBS $LLVM_SYSLIBS"

SANITIZERS="-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer"

WARNINGS="-Wall -Wextra -Werror -Wpedantic -pedantic-errors \
 -Wpointer-arith -Wcast-align -Wwrite-strings \
 -Wstrict-prototypes \
 -Wswitch-default -Wunreachable-code \
 -Wbad-function-cast -Wcast-qual -Wundef \
 -Wshadow -Wfloat-equal -Wformat=2 \
 -Wredundant-decls -Wnested-externs \
 -Wnull-dereference \
 -fcolor-diagnostics"

export BACKTRACE=${BACKTRACE:-}

if [ "$BUILD_TYPE" = "debug" ]; then
    export CFLAGS="${CFLAGS:-} -O0 -g -DDEBUG -DMRD_DEBUG_ONLY_CALLED_AND_ERR $BACKTRACE $INCLUDES $WARNINGS $SANITIZERS $LLVM_CFLAGS"
    export LDFLAGS="${LDFLAGS:-} $LLVM_LDFLAGS $LLVM_RPATH -rdynamic $SANITIZERS"
else
    export CFLAGS="${CFLAGS:-} -O2 -flto $WARNINGS $INCLUDES $LLVM_CFLAGS"
    export LDFLAGS="${LDFLAGS:-} $LLVM_LDFLAGS $LLVM_RPATH -flto"
fi

export BUILD_DIR="build"
export OBJ_DIR="$BUILD_DIR/$BUILD_TYPE"

export TARGET_MAIN="$OBJ_DIR/main.out"
export TARGET_TEST="$OBJ_DIR/test.out"
export TARGET_SPACERS="$OBJ_DIR/spacers"
