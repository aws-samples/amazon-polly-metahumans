SET EXITCODE=0

SET SCRIPT_DIR=%~dp0
SET SDK_REPO_DIR=%SCRIPT_DIR%aws-sdk-cpp\
SET SDK_BUILD_DIR=%SDK_REPO_DIR%_build\
SET SDK_INSTALL_DIR=%SDK_REPO_DIR%_install\
SET MODULE_WIN64_DIR=%SCRIPT_DIR%Win64\

@REM Find and run VsDevCmd.bat to add the msbuild command to Path
SET VS_INSTALLPATH=
IF EXIST "%programfiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
    FOR /F "tokens=* USEBACKQ" %%F IN (
        `"%programfiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -version 16.0 -property installationPath`
    ) DO (
        SET VS_INSTALLPATH=%%F
    )
)
CALL "%VS_INSTALLPATH%\Common7\Tools\VsDevCmd.bat"

@REM Clone the repo
IF NOT EXIST "%SDK_REPO_DIR%" (
    PUSHD "%SCRIPT_DIR%"
    git clone -c core.longpaths=true --recurse-submodules https://github.com/aws/aws-sdk-cpp.git
    IF %ERRORLEVEL% NEQ 0 ( POPD & GOTO FAILED )
    POPD

    PUSHD "%SDK_REPO_DIR%"
    IF %ERRORLEVEL% NEQ 0 ( POPD & GOTO FAILED )
    POPD
)

@REM Create build and install directories where we will build and install the SDK to
IF EXIST "%SDK_BUILD_DIR%" ( RMDIR /Q /S "%SDK_BUILD_DIR%" )
MKDIR "%SDK_BUILD_DIR%"
IF EXIST "%SDK_INSTALL_DIR%" ( RMDIR /Q /S "%SDK_INSTALL_DIR%" )
MKDIR "%SDK_INSTALL_DIR%"

@REM Build and install the SDK
PUSHD "%SDK_BUILD_DIR%"
cmake .. -G "Visual Studio 16 2019" -DBUILD_ONLY=polly -DCUSTOM_MEMORY_MANAGEMENT=ON -DCMAKE_INSTALL_PREFIX="%SDK_INSTALL_DIR%" 
IF %ERRORLEVEL% NEQ 0 ( POPD & GOTO FAILED )
msbuild ALL_BUILD.vcxproj /p:Configuration=Release /p:DebugSymbols=true 
IF %ERRORLEVEL% NEQ 0 ( POPD & GOTO FAILED )
msbuild INSTALL.vcxproj /p:Configuration=Release /p:DebugSymbols=true 
IF %ERRORLEVEL% NEQ 0 ( POPD & GOTO FAILED )
POPD

@REM Remove any previous builds from the platform specific directory
IF EXIST "%MODULE_WIN64_DIR%" ( RMDIR /Q /S "%MODULE_WIN64_DIR%" )
IF %ERRORLEVEL% NEQ 0 ( GOTO FAILED )
MKDIR "%MODULE_WIN64_DIR%"
IF %ERRORLEVEL% NEQ 0 ( GOTO FAILED )

@REM Copy the new build to the platform specific directory
xcopy "%SDK_INSTALL_DIR%include" "%MODULE_WIN64_DIR%include" /E/H/C/I
MKDIR "%MODULE_WIN64_DIR%"bin
MKDIR "%MODULE_WIN64_DIR%"lib
xcopy "%SDK_INSTALL_DIR%"bin\*.dll "%MODULE_WIN64_DIR%bin" /E/H/C/I
xcopy "%SDK_BUILD_DIR%"bin\Release\*.pdb "%MODULE_WIN64_DIR%bin" /E/H/C/I
@REM Some of the libs are built to the bin folder for some weird reason
xcopy "%SDK_INSTALL_DIR%"bin\*.lib "%MODULE_WIN64_DIR%lib"
xcopy "%SDK_INSTALL_DIR%"lib\*.lib "%MODULE_WIN64_DIR%lib"

@REM Remove the cloned repo, build, and install directory.
RMDIR /Q /S "%SDK_REPO_DIR%"

GOTO SUCCESS

:FAILED
set EXITCODE=1
:SUCCESS
EXIT /B %EXITCODE%