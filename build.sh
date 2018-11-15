#!/usr/bin/env bash

set -ex

mkdir -p build || true

build_simple_quic() {
    # Build into build dir
    cd build
    cmake ..
    make
    sudo make install
}

build_simple_quic
