@echo off
echo ESP32-S3 Face - One-Click Installer (Windows)
echo ==============================================

REM Check if Docker is installed
docker --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Installing Docker Desktop...
    echo Please download and install: https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe
    echo After installation, restart and run this script again.
    pause
    exit /b 1
)

REM Check if git is installed
git --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Installing Git...
    winget install Git.Git
)

echo Cloning project...
git clone https://github.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C.git
cd g1-ESP32-S3-Touch-LCD-1.85C

echo Running setup...
run.bat

echo.
echo Done! Connect phone to 'ESP32_Face' Bluetooth and play music.
pause