#!/usr/bin/env bash
set -euo pipefail

APP_ID="biz.lostsidedead.MasterX"
BRANCH="master"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MANIFEST="$PROJECT_DIR/biz.lostsidedead.MasterX.yaml"
WORK_ROOT="${WORK_ROOT:-/tmp/masterx-flatpak}"
STATE_DIR="${STATE_DIR:-$WORK_ROOT/state}"
BUILD_DIR="${BUILD_DIR:-$WORK_ROOT/build}"
REPO_DIR="${REPO_DIR:-$PROJECT_DIR/repo}"
BUNDLE_FILE="${BUNDLE_FILE:-$PROJECT_DIR/MasterX.flatpak}"
ARCH="${ARCH:-$(flatpak --default-arch)}"
FORCE_CLEAN=1
BUILD_BUNDLE=1

log() {
  printf "[build-flatpak] %s\n" "$*"
}

fail() {
  printf "[build-flatpak] ERROR: %s\n" "$*" >&2
  exit 1
}

usage() {
  cat <<EOF
Usage: $0 [options]

Build the MasterX Flatpak, export a local repo, and create a bundle.

Options:
  --work-root DIR     Working root for temporary build state (default: $WORK_ROOT)
  --state-dir DIR     flatpak-builder state dir (default: WORK_ROOT/state)
  --build-dir DIR     flatpak-builder build dir (default: WORK_ROOT/build)
  --repo-dir DIR      Exported Flatpak repo directory (default: $REPO_DIR)
  --bundle FILE       Output .flatpak bundle path (default: $BUNDLE_FILE)
  --arch ARCH         Target architecture (default: $ARCH)
  --branch NAME       Flatpak branch/ref name (default: $BRANCH)
  --no-clean          Reuse previous build and state directories
  --no-bundle         Skip generating the .flatpak bundle
  -h, --help          Show this help text

Environment overrides:
  WORK_ROOT, STATE_DIR, BUILD_DIR, REPO_DIR, BUNDLE_FILE, ARCH
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --work-root)
      [[ $# -ge 2 ]] || fail "Missing value for $1"
      WORK_ROOT="$2"
      STATE_DIR="$WORK_ROOT/state"
      BUILD_DIR="$WORK_ROOT/build"
      shift 2
      ;;
    --state-dir)
      [[ $# -ge 2 ]] || fail "Missing value for $1"
      STATE_DIR="$2"
      shift 2
      ;;
    --build-dir)
      [[ $# -ge 2 ]] || fail "Missing value for $1"
      BUILD_DIR="$2"
      shift 2
      ;;
    --repo-dir)
      [[ $# -ge 2 ]] || fail "Missing value for $1"
      REPO_DIR="$2"
      shift 2
      ;;
    --bundle)
      [[ $# -ge 2 ]] || fail "Missing value for $1"
      BUNDLE_FILE="$2"
      shift 2
      ;;
    --arch)
      [[ $# -ge 2 ]] || fail "Missing value for $1"
      ARCH="$2"
      shift 2
      ;;
    --branch)
      [[ $# -ge 2 ]] || fail "Missing value for $1"
      BRANCH="$2"
      shift 2
      ;;
    --no-clean)
      FORCE_CLEAN=0
      shift
      ;;
    --no-bundle)
      BUILD_BUNDLE=0
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      usage
      fail "Unknown argument: $1"
      ;;
  esac
done

command -v flatpak >/dev/null 2>&1 || fail "flatpak is not installed"
command -v flatpak-builder >/dev/null 2>&1 || fail "flatpak-builder is not installed"
[[ -f "$MANIFEST" ]] || fail "Manifest not found: $MANIFEST"

mkdir -p "$STATE_DIR" "$REPO_DIR"
mkdir -p "$(dirname "$BUNDLE_FILE")"

if [[ "$FORCE_CLEAN" -eq 1 ]]; then
  log "Cleaning build directories"
  rm -rf "$BUILD_DIR"
fi

log "Manifest: $MANIFEST"
log "State dir: $STATE_DIR"
log "Build dir: $BUILD_DIR"
log "Repo dir: $REPO_DIR"
if [[ "$BUILD_BUNDLE" -eq 1 ]]; then
  log "Bundle: $BUNDLE_FILE"
fi

flatpak-builder \
  --state-dir="$STATE_DIR" \
  --disable-rofiles-fuse \
  $(if [[ "$FORCE_CLEAN" -eq 1 ]]; then printf '%s' '--force-clean'; fi) \
  --repo="$REPO_DIR" \
  "$BUILD_DIR" \
  "$MANIFEST"

log "Updating repo metadata"
flatpak build-update-repo --generate-static-deltas "$REPO_DIR"

if [[ "$BUILD_BUNDLE" -eq 1 ]]; then
  log "Creating bundle"
  rm -f "$BUNDLE_FILE"
  flatpak build-bundle "$REPO_DIR" "$BUNDLE_FILE" "$APP_ID" "$BRANCH" --arch="$ARCH"
fi

log "Build complete"
log "Install repo build with: flatpak install --user "$REPO_DIR" $APP_ID"
if [[ "$BUILD_BUNDLE" -eq 1 ]]; then
  log "Install bundle with: flatpak install --user "$BUNDLE_FILE""
fi