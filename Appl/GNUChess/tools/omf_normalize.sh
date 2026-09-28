#!/bin/sh
# Linux/Unix launcher. PC/GEOS already requires Perl.
set -eu
SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
exec perl "$SCRIPT_DIR/omf_normalize.pl" "$@"
