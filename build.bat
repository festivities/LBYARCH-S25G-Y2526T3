@echo off
nasm -f win64 DeCaJVallJMP2.asm -o DeCaJVallJMP2.obj || exit /b 1
gcc DeCaJVallJMP2.c DeCaJVallJMP2.obj -o app.exe || exit /b 1
echo Build finished. Run with: app.exe   (or: app.exe ^< sample.txt)
