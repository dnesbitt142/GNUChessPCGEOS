"""Shared configuration for the direct Linux-hosted PC/GEOS build."""
from pathlib import Path
import os

ROOT = Path(__file__).resolve().parents[1]

def required_dir(name):
    value = os.environ.get(name)
    if not value:
        raise SystemExit(f"Set {name} to the extracted toolchain directory. See README.md.")
    path = Path(value).expanduser().resolve()
    if not path.is_dir():
        raise SystemExit(f"{name} is not a directory: {path}")
    return path

def tool(path):
    if not path.is_file() or not os.access(path, os.X_OK):
        raise SystemExit(f"Executable not found: {path}")
    return path
