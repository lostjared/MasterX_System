#!/usr/bin/env bash
set -euo pipefail

BIN_PATH="/usr/local/bin/MasterX"
DESKTOP_FILE="/usr/local/share/applications/MasterX_System.desktop"

if ! command -v distrobox-export >/dev/null 2>&1; then
    echo "Error: distrobox-export is not available in this container."
    exit 1
fi

if [[ ! -x "$BIN_PATH" ]]; then
    echo "Error: $BIN_PATH not found. Install first (example: cmake --install <build-dir>)."
    exit 1
fi

if [[ ! -f "$DESKTOP_FILE" ]]; then
    echo "Error: Desktop file not found in /usr/local/share/applications."
    echo "Expected: $DESKTOP_FILE"
    exit 1
fi

echo "Exporting desktop launcher..."
distrobox-export --app "$DESKTOP_FILE"

echo "Exporting binary..."
distrobox-export --bin "$BIN_PATH"

echo "Done. MasterX exported to host application menu and PATH integration (if supported)."
