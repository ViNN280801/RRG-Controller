# Stop script execution on first error
$ErrorActionPreference = "Stop"

# Define paths
$LIBRRG_PATH = "build\c_api\src\rrg\librrg.dll"
$LIBRRG_RESOURCES_PATH = "ui\resources\librrg.dll"
$LIBRELAY_PATH = "build\c_api\src\relay\librelay.dll"
$LIBRELAY_RESOURCES_PATH = "ui\resources\librelay.dll"

$RRG_LIVE_TEST_PATH = "build\c_api\rrg_live_test.exe"
$RELAY_LIVE_TEST_PATH = "build\c_api\relay_live_test.exe"

# Create build directory
Write-Host "Creating build directory..."
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Navigate to build directory
Set-Location -Path "build"

# Run CMake
Write-Host "Running CMake..."
cmake .. 
if ($LASTEXITCODE -ne 0) { 
    Write-Host "CMake failed. Exiting..." 
    exit 1 
}

# Run make (or use `cmake --build .` for cross-platform compatibility)
Write-Host "Building project..."
cmake --build . 
if ($LASTEXITCODE -ne 0) { 
    Write-Host "Build failed. Exiting..." 
    exit 1 
}

# Return to root directory
Set-Location -Path ".."

# Copy built libraries to the appropriate locations
Write-Host "Copying built DLLs..."
Copy-Item -Path $LIBRRG_PATH -Destination $LIBRRG_RESOURCES_PATH -Force
Copy-Item -Path $LIBRRG_PATH -Destination ".\librrg.dll" -Force

Copy-Item -Path $LIBRELAY_PATH -Destination $LIBRELAY_RESOURCES_PATH -Force
Copy-Item -Path $LIBRELAY_PATH -Destination ".\librelay.dll" -Force

# Copy executable tests
Write-Host "Copying test executables..."
Copy-Item -Path $RRG_LIVE_TEST_PATH -Destination ".\rrg_live_test.exe" -Force
Copy-Item -Path $RELAY_LIVE_TEST_PATH -Destination ".\relay_live_test.exe" -Force

# Remove build directory
Write-Host "Removing build directory..."
Remove-Item -Recurse -Force "build"

Write-Host "Build process completed successfully!"
