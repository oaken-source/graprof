#!/bin/bash
mkdir -p m4
autoreconf --install || echo "autoreconf failed." && exit 1
