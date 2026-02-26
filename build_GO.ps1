# ============================================================
# SCRIPT DE BUILD UNIVERSEL - COSMOUIT
# Détection automatique du compilateur
# ============================================================

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host " BUILD PORTABLE - CosmoUIT" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# ===== CONFIGURATION SFML =====
$SFML_ROOT = "C:\Users\hp\Downloads\SFML-2.6.1-windows-vc17-64-bit\SFML-2.6.1"

Write-Host "[0/8] Verification de SFML..." -ForegroundColor Yellow
Write-Host "  Chemin : $SFML_ROOT" -ForegroundColor Gray

if (-not (Test-Path $SFML_ROOT)) {
    Write-Host "[ERREUR] SFML non trouve" -ForegroundColor Red
    pause
    exit 1
}

Write-Host "[OK] SFML trouve" -ForegroundColor Green

# Détecter le type de DLLs
$hasDebugDlls = Test-Path "$SFML_ROOT\bin\sfml-graphics-d-2.dll"
$hasReleaseDlls = Test-Path "$SFML_ROOT\bin\sfml-graphics-2.dll"

if ($hasDebugDlls) {
    $buildType = "Debug"
    $dllSuffix = "-d-2.dll"
    Write-Host "[INFO] Build type : Debug" -ForegroundColor Cyan
} elseif ($hasReleaseDlls) {
    $buildType = "Release"
    $dllSuffix = "-2.dll"
    Write-Host "[INFO] Build type : Release" -ForegroundColor Cyan
} else {
    Write-Host "[ERREUR] Aucune DLL SFML trouvee" -ForegroundColor Red
    pause
    exit 1
}

Write-Host ""

# ===== ÉTAPE 1 : NETTOYAGE =====
Write-Host "[1/8] Nettoyage..." -ForegroundColor Yellow

if (Test-Path "build") {
    Remove-Item -Recurse -Force "build" -ErrorAction SilentlyContinue
}
if (Test-Path "SimulateurSolaire2D_Portable") {
    Remove-Item -Recurse -Force "SimulateurSolaire2D_Portable" -ErrorAction SilentlyContinue
}

Write-Host "[OK] Nettoyage termine" -ForegroundColor Green
Write-Host ""

# ===== ÉTAPE 2 : CRÉATION BUILD =====
Write-Host "[2/8] Creation du dossier build..." -ForegroundColor Yellow

New-Item -ItemType Directory -Path "build" -Force | Out-Null
Set-Location "build"

Write-Host "[OK] Dossier build cree" -ForegroundColor Green
Write-Host ""

# ===== ÉTAPE 3 : CONFIGURATION CMAKE (SANS GÉNÉRATEUR SPÉCIFIQUE) =====
Write-Host "[3/8] Configuration CMake ($buildType)..." -ForegroundColor Yellow

$cmakeConfigPath = "$SFML_ROOT\lib\cmake\SFML"

Write-Host "  SFML_DIR : $cmakeConfigPath" -ForegroundColor Gray
Write-Host "  CMAKE laisse detecter automatiquement le compilateur..." -ForegroundColor Gray
Write-Host ""

# Arguments CMake SANS -G (laisse CMake détecter)
$cmakeArgs = @(
    "..",
    "-DSFML_DIR=`"$cmakeConfigPath`"",
    "-DCMAKE_BUILD_TYPE=$buildType"
)

Write-Host "  Commande : cmake $($cmakeArgs -join ' ')" -ForegroundColor DarkGray
Write-Host ""
Write-Host "  Execution..." -ForegroundColor Yellow
Write-Host ""

# Exécuter CMake et afficher la sortie en temps réel
$process = Start-Process -FilePath "cmake" -ArgumentList $cmakeArgs -NoNewWindow -PassThru -Wait

if ($process.ExitCode -ne 0) {
    Write-Host ""
    Write-Host "[ERREUR] Configuration CMake echouee" -ForegroundColor Red
    Write-Host ""
    Write-Host "DIAGNOSTIC :" -ForegroundColor Yellow
    Write-Host "1. Verifiez que CMake est installe : cmake --version" -ForegroundColor Gray
    Write-Host "2. Verifiez qu'un compilateur C++ est installe (Visual Studio OU MinGW)" -ForegroundColor Gray
    Write-Host ""
    Write-Host "INSTALLER UN COMPILATEUR :" -ForegroundColor Yellow
    Write-Host "Option 1 : Visual Studio Community 2019+ (gratuit)" -ForegroundColor Gray
    Write-Host "  https://visualstudio.microsoft.com/downloads/" -ForegroundColor Cyan
  Write-Host "  Selectionnez : 'Desktop development with C++'" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Option 2 : MinGW-w64" -ForegroundColor Gray
    Write-Host "  https://www.mingw-w64.org/downloads/" -ForegroundColor Cyan
Write-Host ""
    Set-Location ..
    pause
    exit 1
}

Write-Host ""
Write-Host "[OK] Configuration CMake terminee" -ForegroundColor Green
Write-Host ""

# ===== ÉTAPE 4 : COMPILATION =====
Write-Host "[4/8] Compilation $buildType..." -ForegroundColor Yellow
Write-Host "Cela peut prendre 3-5 minutes..." -ForegroundColor Gray
Write-Host ""

$buildArgs = @("--build", ".", "--config", $buildType)
$buildProcess = Start-Process -FilePath "cmake" -ArgumentList $buildArgs -NoNewWindow -PassThru -Wait

if ($buildProcess.ExitCode -ne 0) {
    Write-Host ""
    Write-Host "[ERREUR] Compilation echouee" -ForegroundColor Red
    Write-Host ""
    Write-Host "Consultez les erreurs ci-dessus pour plus de details" -ForegroundColor Yellow
    Set-Location ..
  pause
    exit 1
}

Write-Host ""
Write-Host "[OK] Compilation terminee" -ForegroundColor Green
Write-Host ""

Set-Location ..

# ===== ÉTAPE 5 : CRÉATION PORTABLE =====
Write-Host "[5/8] Creation du dossier portable..." -ForegroundColor Yellow

New-Item -ItemType Directory -Path "SimulateurSolaire2D_Portable" -Force | Out-Null

Write-Host "[OK] Dossier cree" -ForegroundColor Green
Write-Host ""

# ===== ÉTAPE 6 : COPIE EXÉCUTABLE =====
Write-Host "[6/8] Recherche et copie de l'executable..." -ForegroundColor Yellow

# Chercher dans plusieurs emplacements
$exePaths = @(
"build\$buildType\CosmoUIT.exe",
    "build\Release\CosmoUIT.exe",
    "build\Debug\CosmoUIT.exe",
    "build\CosmoUIT.exe"
)

$exeCopied = $false
foreach ($exePath in $exePaths) {
    if (Test-Path $exePath) {
Copy-Item $exePath "SimulateurSolaire2D_Portable\" -Force
        Write-Host "  [OK] Trouve : $exePath" -ForegroundColor Green
        $exeCopied = $true
        break
    }
}

# Recherche récursive si non trouvé
if (-not $exeCopied) {
    Write-Host "  [INFO] Recherche recursive..." -ForegroundColor Yellow
    $foundExe = Get-ChildItem -Path "build" -Filter "CosmoUIT.exe" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($foundExe) {
        Copy-Item $foundExe.FullName "SimulateurSolaire2D_Portable\" -Force
        Write-Host "  [OK] Trouve : $($foundExe.FullName)" -ForegroundColor Green
$exeCopied = $true
    }
}

if (-not $exeCopied) {
    Write-Host "  [ERREUR] Executable CosmoUIT.exe non trouve" -ForegroundColor Red
    Write-Host ""
  Write-Host "Chemins essayes :" -ForegroundColor Yellow
    foreach ($path in $exePaths) {
        Write-Host "  - $path" -ForegroundColor Gray
    }
    pause
    exit 1
}

Write-Host ""

# ===== ÉTAPE 7 : COPIE DLLs ET RESSOURCES =====
Write-Host "[7/8] Copie des DLLs et ressources..." -ForegroundColor Yellow

# DLLs SFML
Write-Host "  - DLLs SFML..." -ForegroundColor Gray
$dlls = @(
    "sfml-graphics$dllSuffix",
 "sfml-window$dllSuffix",
    "sfml-system$dllSuffix"
)

$dllCount = 0
foreach ($dll in $dlls) {
  $srcPath = "$SFML_ROOT\bin\$dll"
    if (Test-Path $srcPath) {
 Copy-Item $srcPath "SimulateurSolaire2D_Portable\" -Force
        Write-Host "    [OK] $dll" -ForegroundColor Green
        $dllCount++
    } else {
        Write-Host "    [MANQUANT] $dll" -ForegroundColor Red
    }
}

# openal32.dll
$openalPath = "$SFML_ROOT\bin\openal32.dll"
if (Test-Path $openalPath) {
    Copy-Item $openalPath "SimulateurSolaire2D_Portable\" -Force
    Write-Host "    [OK] openal32.dll" -ForegroundColor Green
}

Write-Host ""

# Textures
Write-Host "  - Ressources..." -ForegroundColor Gray
if (Test-Path "textures") {
    Copy-Item -Path "textures" -Destination "SimulateurSolaire2D_Portable\textures" -Recurse -Force
    $textureCount = (Get-ChildItem "SimulateurSolaire2D_Portable\textures" -File -ErrorAction SilentlyContinue).Count
    Write-Host "    [OK] textures/ ($textureCount fichiers)" -ForegroundColor Green
} else {
    Write-Host "    [ATTENTION] Dossier textures non trouve" -ForegroundColor Yellow
}

# Police
if (Test-Path "font.ttf") {
    Copy-Item "font.ttf" "SimulateurSolaire2D_Portable\" -Force
    Write-Host "    [OK] font.ttf" -ForegroundColor Green
} else {
    Write-Host "    [ATTENTION] font.ttf non trouve" -ForegroundColor Yellow
}

Write-Host ""

# ===== ÉTAPE 8 : README ET LANCEUR =====
Write-Host "[8/8] Creation README et lanceur..." -ForegroundColor Yellow

$readme = @"
========================================
 COSMOUIT - SIMULATEUR SOLAIRE 2D
========================================

LANCEMENT :
  Double-cliquez sur CosmoUIT.exe
  OU lancez Lancer_CosmoUIT.bat

CONFIGURATION REQUISE :
  - Windows 7/8/10/11
  - 512 MB RAM minimum
  - Carte graphique compatible OpenGL 2.0+

CONTROLES :
  - Fleches directionnelles : Deplacer la camera
  - Molette souris : Zoom
  - Espace : Pause
  - Tab : Menu outils
  - Echap : Retour
  - 1-8 : Suivre une planete
  - 0 : Centrer sur le Soleil
  - H : Afficher les missions spatiales
  - Q : Quiz educatif

DEVELOPPEURS :
  - Universite Ibn Tofail
  - Equipe CosmoUIT 2025

VERSION : 2.0 Portable
BUILD TYPE : $buildType
========================================
"@

$readme | Out-File -FilePath "SimulateurSolaire2D_Portable\README.txt" -Encoding UTF8

$launcher = @"
@echo off
start CosmoUIT.exe
exit
"@

$launcher | Out-File -FilePath "SimulateurSolaire2D_Portable\Lancer_CosmoUIT.bat" -Encoding ASCII

Write-Host "  [OK] README.txt cree" -ForegroundColor Green
Write-Host "  [OK] Lancer_CosmoUIT.bat cree" -ForegroundColor Green

Write-Host ""

# ===== RÉSULTAT FINAL =====
Write-Host "========================================" -ForegroundColor Green
Write-Host " BUILD TERMINE AVEC SUCCES !" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Type de build : $buildType" -ForegroundColor Cyan
Write-Host "SFML : $SFML_ROOT" -ForegroundColor Cyan
Write-Host ""
Write-Host "Dossier cree : SimulateurSolaire2D_Portable\" -ForegroundColor Cyan
Write-Host ""

Write-Host "CONTENU DU DOSSIER PORTABLE :" -ForegroundColor Yellow
Get-ChildItem "SimulateurSolaire2D_Portable" | Format-Table Name, @{Label="Taille";Expression={
    if ($_.PSIsContainer) { "<DIR>" } 
    elseif ($_.Length -lt 1KB) { "$($_.Length) B" }
    elseif ($_.Length -lt 1MB) { "{0:N2} KB" -f ($_.Length / 1KB) }
    else { "{0:N2} MB" -f ($_.Length / 1MB) }
}} -AutoSize

Write-Host ""
Write-Host "========================================" -ForegroundColor Magenta
Write-Host " PROCHAINES ETAPES" -ForegroundColor Magenta
Write-Host "========================================" -ForegroundColor Magenta
Write-Host ""
Write-Host "1. TESTER l'application :" -ForegroundColor White
Write-Host "   cd SimulateurSolaire2D_Portable" -ForegroundColor Gray
Write-Host "   .\CosmoUIT.exe" -ForegroundColor Gray
Write-Host ""
Write-Host "2. COMPRESSER pour distribution :" -ForegroundColor White
Write-Host "   Compress-Archive -Path 'SimulateurSolaire2D_Portable' -DestinationPath 'CosmoUIT_v2.0.zip'" -ForegroundColor Gray
Write-Host ""
Write-Host "3. DISTRIBUER a votre encadrant" -ForegroundColor White
Write-Host "   Envoyez le fichier CosmoUIT_v2.0.zip" -ForegroundColor Gray
Write-Host ""

pause
