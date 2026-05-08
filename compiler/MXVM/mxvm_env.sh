export MXVM_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo "set path: $MXVM_DIR"
export PATH="$MXVM_DIR:$PATH"
