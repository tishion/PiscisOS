#!/bin/bash

# Build script for Piscis OS
# Author: tishion (tishion#163.com)
# Date: 2016-03-26 11:58:38

RUN="$1"
echo "Run parameter: $RUN"

# Get the directory of the script and change to it
PROJ_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_ROOT="${PROJ_ROOT}/src"
SRC_APPS="${SRC_ROOT}/apps"
BUILD_DIR="${PROJ_ROOT}/build"
OUT_ROOT="${BUILD_DIR}/out"
IMG_ROOT="${BUILD_DIR}/image"
IMG_NAME=piscisos.img
IMG_PATH="${IMG_ROOT}/${IMG_NAME}"
OUT_APPS="${OUT_ROOT}/app"
BOCHS_SCRIPT="${IMG_ROOT}/bochsrc.bxrc"

cd "$PROJ_ROOT"

# Function to exit on error
exit_on_error() {
    if [ $? -ne 0 ]; then
        echo "Build failed!"
        exit 1
    fi
}

# Create output folders
echo "Creating output directories..."
mkdir -p "$OUT_ROOT"
mkdir -p "$OUT_APPS"
mkdir -p "$IMG_ROOT"

# Build bootsector file
echo "====== Building bootsector... ======"
fasm "${SRC_ROOT}/boot/bootsect.asm" -s "${OUT_ROOT}/bootsector.sym" "${OUT_ROOT}/bootsector"
exit_on_error
echo

# Build kernel file
echo "====== Building pkernel... ======"
fasm "${SRC_ROOT}/kernel/pkernel.asm" -s "${OUT_ROOT}/pkernel.sym" "${OUT_ROOT}/pkernel.bin"
exit_on_error
echo

# Build shell file
echo "====== Building shell... ======"
fasm "${SRC_ROOT}/shell/shell.asm" -s "${OUT_ROOT}/shell.sym" "${OUT_ROOT}/shell"
exit_on_error
echo

# Build apps
echo "====== Building applications... ======"
if [ -d "$SRC_APPS" ]; then
    find "$SRC_APPS" -name "*.asm" -type f | while read -r asm_file; do
        filename=$(basename "$asm_file" .asm)
        echo "+Building $asm_file"
        fasm "$asm_file" -s "${OUT_APPS}/${filename}.sym" "${OUT_APPS}/${filename}"
        if [ $? -ne 0 ]; then
            echo "Failed to build $asm_file"
            exit 1
        fi
    done
fi
echo

echo "====== Burning OS image... ======"

echo "+Creating image file with bootsector..."
mformat -f 1440 -v PiscisOSVOL -B "${OUT_ROOT}/bootsector" -C -i "$IMG_PATH" ::
exit_on_error

echo "+Copying pkernel.bin to image file system..."
mcopy -i "$IMG_PATH" "${OUT_ROOT}/pkernel.bin" ::
exit_on_error

echo "+Copying shell to image file system..."
mcopy -i "$IMG_PATH" "${OUT_ROOT}/shell" ::
exit_on_error

echo "+Creating bin folder in image file system..."
mmd -i "$IMG_PATH" ::bin
exit_on_error

echo "+Copying all applications to image file system..."
if [ -d "$OUT_APPS" ] && [ "$(find "$OUT_APPS" -maxdepth 1 -type f | wc -l)" -gt 0 ]; then
    mcopy -i "$IMG_PATH" "${OUT_APPS}"/* ::bin
    exit_on_error
fi

echo "Build and burn done successfully!"
echo "Output floppy image file: $IMG_PATH"

echo "+Creating bochs script..."
echo "floppya: type=1_44, 1_44=\"$IMG_NAME\", status=inserted, write_protected=1" > "$BOCHS_SCRIPT"
echo "Done!"

if [ "$RUN" = "-run" ]; then
    if command -v bochs >/dev/null 2>&1; then
        bochs -f "$BOCHS_SCRIPT"
    else
        echo "Bochs not found in PATH. Please install bochs or add it to PATH."
    fi
fi