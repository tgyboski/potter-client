@echo off
setlocal

:: Verificar se o Ninja está instalado
where ninja >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Erro: Ninja nao encontrado no PATH
    echo Por favor, instale o Ninja e adicione ao PATH
    echo Voce pode baixar em: https://github.com/ninja-build/ninja/releases
    pause
    exit /b 1
)

:: Verificar se o CMake está instalado
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Erro: CMake nao encontrado no PATH
    echo Por favor, instale o CMake e adicione ao PATH
    pause
    exit /b 1
)

:: Configurar variáveis de ambiente do Visual Studio
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_PATH=%%i"
    )
)

if defined VS_PATH (
    if exist "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat" (
        call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
    ) else (
        echo Erro: vcvars64.bat nao encontrado em %VS_PATH%
        pause
        exit /b 1
    )
) else (
    echo Erro: Visual Studio com suporte a C++ nao encontrado
    echo Por favor, instale o Visual Studio 2022 com suporte a C++
    pause
    exit /b 1
)

:: Limpar diretório build se existir
if exist "build" (
    echo Limpando diretorio build...
    rmdir /s /q build
)

:: Criar diretório de build
echo Criando diretorio build...
mkdir build
cd build

:: Configurar o projeto com CMake
echo Configurando projeto com CMake...
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=ninja

if %ERRORLEVEL% neq 0 (
    echo Erro ao configurar o projeto com CMake
    cd ..
    pause
    exit /b 1
)

:: Buildar o projeto
echo Buildando o projeto...
cmake --build . --config Release

if %ERRORLEVEL% neq 0 (
    echo Erro ao buildar o projeto
    cd ..
    pause
    exit /b 1
)

:: Voltar para o diretório raiz
cd ..

echo Build concluido com sucesso!
pause 