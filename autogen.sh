#!/bin/bash
mkdir -p scripts/m4
autoreconf --install || echo "autoreconf failed." && exit 1
