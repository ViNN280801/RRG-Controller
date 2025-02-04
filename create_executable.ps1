# Stop script execution on first error
$ErrorActionPreference = "Stop"

# Define the output name
$APP_NAME = "rrg_relay"

# Define paths
$MAIN_SCRIPT = ".\ui\main.py"
$DIST_DIR = ".\dist"
$BUILD_DIR = ".\build"

# Clean previous builds
Write-Host "Cleaning previous builds..."
if (Test-Path $DIST_DIR) { Remove-Item -Recurse -Force $DIST_DIR }
if (Test-Path $BUILD_DIR) { Remove-Item -Recurse -Force $BUILD_DIR }

# Create executable with PyInstaller
Write-Host "Building executable..."
pyinstaller --noconfirm `
            --onedir `
            --console `
            --clean `
            --log-level "INFO" `
            --name $APP_NAME `
            --add-data "ui;ui\" `
            --add-data "c_api;c_api\" `
            --add-data "ui/config;config\" `
            --add-binary "ui/resources/librelay.dll;ui/resources/" `
            --add-binary "ui/resources/librrg.dll;ui/resources/" `
            --paths "ui" `
            $MAIN_SCRIPT

# Copy DLLs into the dist folder (if needed)
Write-Host "Copying DLL files to the dist folder..."
Copy-Item -Path ".\ui\resources\*" -Destination "$DIST_DIR\$APP_NAME\" -Force

Write-Host "Build completed. Executable is in $DIST_DIR\$APP_NAME"
