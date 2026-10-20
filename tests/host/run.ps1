# Host test runner (local Windows): MSVC via vcvars, tests against fs_std.
# Usage: tests\host\run.ps1 [doctest args...]
param([Parameter(ValueFromRemainingArguments = $true)] [string[]] $TestArgs)
$ErrorActionPreference = "Stop"
$here = $PSScriptRoot
$root = (Resolve-Path (Join-Path $here "..\..")).Path
$build = Join-Path $here "build"
New-Item -ItemType Directory -Force $build | Out-Null

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) { throw "Visual Studio C++ toolchain not found (install VS with C++ workload)" }
$vcvars = Join-Path $vs "VC\Auxiliary\Build\vcvars64.bat"

$engineNames = "settings", "model", "quickstart", "sha256", "store", "fs_std"
$engine = $engineNames | ForEach-Object { Join-Path $root "external_components\curling_engine\$_.cpp" }
$tests = (Get-ChildItem (Join-Path $here "test_*.cpp")).FullName
$missing = @($engine + $tests) | Where-Object { -not (Test-Path $_) }
if ($missing) { throw "missing sources: $($missing -join ', ')" }
$sources = (@($engine) + $tests | ForEach-Object { '"' + $_ + '"' }) -join " "

Push-Location $build
try {
    $cmd = "call `"$vcvars`" >nul 2>&1 && cl /nologo /std:c++20 /utf-8 /EHsc /W3 /D_CRT_SECURE_NO_WARNINGS /I`"$here`" $sources /Fe:host_tests.exe"
    cmd /c $cmd
    if ($LASTEXITCODE -ne 0) { throw "compile failed (exit $LASTEXITCODE)" }
    & (Join-Path $build "host_tests.exe") @TestArgs
    exit $LASTEXITCODE
} finally { Pop-Location }
