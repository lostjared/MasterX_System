#!/usr/bin/env bash
set -euo pipefail

BIN_PATH="/usr/local/bin/MasterX"
APP_NAME="MasterX_System"

if ! command -v distrobox-export >/dev/null 2>&1; then
    echo "Error: distrobox-export is not available in this container."
    exit 1
fi

echo "Removing exported desktop launcher..."
distrobox-export --app "$APP_NAME" --delete || true

echo "Removing exported binary..."
distrobox-export --bin "$BIN_PATH" --delete || true

echo "Done. Exported MasterX launcher and binary were removed (if they existed)."
