#!/bin/bash

# Ensure script stops on first error
set -e

# Define the output name
APP_NAME="rrg_relay"

# Define paths
MAIN_SCRIPT="./ui/main.py"
DIST_DIR="./dist"
BUILD_DIR="./build"

# Clean previous builds
rm -rf "$DIST_DIR" "$BUILD_DIR"

# Create executable with PyInstaller
pyinstaller --noconfirm \
            --onedir \
            --console \
            --clean \
            --log-level "INFO" \
            --name "$APP_NAME" \
            --add-data "ui:ui/" \
            --add-data "c_api:c_api/" \
            --add-data "ui/config:config/" \
            --add-binary "ui/resources/librelay.so:ui/resources/" \
            --add-binary "ui/resources/librrg.so:ui/resources/" \
            --paths "ui" \
            "$MAIN_SCRIPT"

cp -v ui/resources/* $DIST_DIR/$APP_NAME
echo "Build completed. Executable is in $DIST_DIR/$APP_NAME"
