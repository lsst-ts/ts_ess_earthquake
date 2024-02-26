#!/bin/bash

# Build lib330
make CFLAGS=-fPIC LDFLAGS=-lm -C ../earthworm/src/libsrc/lib330 -f makefile.unix

# Build libmseed
make -C ../earthworm/src/libsrc/util/libmseed
