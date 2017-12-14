@echo off
setlocal
set VS_VERSION=15 2017
set TOOLSET_VERSION=
rem RelWithDebInfo configuration do many problems in solution, allow creating pdb for Release configuration
set CMAKE_PARAMS=-D BENCHMARK_ENABLE_LTO:BOOL=ON -D CMAKE_CONFIGURATION_TYPES:STRING=Debug;Release -D CMAKE_CXX_FLAGS_RELEASE:STRING="/MD /Zi /O2 /D NDEBUG" -D CMAKE_C_FLAGS_RELEASE:STRING="/MD /Zi /O2 /D NDEBUG" -D CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING="/DEBUG /INCREMENTAL:NO" -D CMAKE_SHARED_LINKER_FLAGS_RELEASE:STRING="/DEBUG /INCREMENTAL:NO"
rem set CMAKE_OUTPUT_DIRS_WIN32=-D CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:STRING=%~dp0\_result\Win32_Debug\lib\ -D CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG:STRING=%~dp0\_result\Win32_Debug\lib -D CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG:STRING=%~dp0\_result\Win32_Debug\bin^
rem  -D CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:STRING=%~dp0\_result\Win32_Release\lib\ -D CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE:STRING=%~dp0\_result\Win32_Release\lib -D CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:STRING=%~dp0\_result\Win32_Release\bin
rem set CMAKE_OUTPUT_DIRS_X64=-D CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:STRING=%~dp0\_result\x64_Debug\lib\ -D CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG:STRING=%~dp0\_result\x64_Debug\lib -D CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG:STRING=%~dp0\_result\x64_Debug\bin^
rem  -D CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:STRING=%~dp0\_result\x64_Release\lib\ -D CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE:STRING=%~dp0\_result\x64_Release\lib -D CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:STRING=%~dp0\_result\x64_Release\bin

mkdir _build
cd _build

rem mkdir Win32
rem cd Win32
rem cmake -G "Visual Studio %VS_VERSION%" %TOOLSET_VERSION% %CMAKE_PARAMS% ..\..
rem cmake --build . --config "Debug"
rem cmake --build . --config "Release"
rem cd ..

rem mkdir x64
rem cd x64
cmake -G "Visual Studio %VS_VERSION% Win64" %TOOLSET_VERSION% %CMAKE_PARAMS% %CMAKE_OUTPUT_DIRS_X64% ..
cmake --build . --config "Debug"
rem cmake --build . --config "Release"
rem cd ..

cd ..