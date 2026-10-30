param(
    [string]$BuildDir = (Join-Path $PSScriptRoot 'build'),
    [string]$OutputDir = (Join-Path $PSScriptRoot 'packages'),
    [string]$NsisRoot = 'C:\Program Files (x86)\NSIS',
    [switch]$SkipBuild,
    [switch]$Validation,
    [switch]$Candidate,
    [switch]$Refresh
)
$ErrorActionPreference = 'Stop'
if (!(Test-Path (Join-Path $NsisRoot 'makensis.exe'))) { throw 'NSIS not found; no dependencies will be installed.' }
if (!$SkipBuild) {
    $env:LW_BUILD_DIR = $BuildDir
    & (Join-Path $PSScriptRoot 'build.cmd')
    if ($LASTEXITCODE) { throw 'Release build or tests failed.' }
}
$Version = (Get-Content -LiteralPath (Join-Path $BuildDir 'version.txt') -Raw).Trim()
if ($Version -notmatch '^\d+\.\d+\.\d+$') { throw 'Invalid CMake version metadata.' }
$BuildDir = [IO.Path]::GetFullPath($BuildDir)
$OutputDir = [IO.Path]::GetFullPath($OutputDir)
$projectRoot = [IO.Path]::GetFullPath($PSScriptRoot)
if ($BuildDir -eq $projectRoot -or $BuildDir -eq [IO.Path]::GetPathRoot($BuildDir)) { throw 'Unsafe build directory.' }
$statusFile = Join-Path $PSScriptRoot 'RELEASE-STATUS.md'
if (!(Test-Path -LiteralPath $statusFile)) { throw 'Missing release status.' }
if (!$Candidate -and (Get-Content -LiteralPath $statusFile -Raw) -match 'NO-GO') {
    throw 'Binary release is blocked. Use -Candidate only for local verification; see RELEASE-STATUS.md.'
}
$suffix = if ($Candidate) { '-candidate' } else { '' }
if ($Validation) { $suffix += '-validation' }
$output = Join-Path $OutputDir "LuckyWheel-$Version-Windows-x64-Setup$suffix.exe"
$portable = Join-Path $OutputDir "LuckyWheel-$Version-Windows-x64-Portable$suffix.zip"
$sourceZip = Join-Path $OutputDir "LuckyWheel-$Version-source.zip"
$hashFile = Join-Path $OutputDir 'SHA256SUMS.txt'
foreach ($target in @($output,$portable,$sourceZip,$hashFile)) {
    if (Test-Path -LiteralPath $target) {
        if (!$Refresh) { throw "Refusing to overwrite existing deliverable: $target" }
        if ([IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($target)) -ne $OutputDir) { throw 'Unsafe refresh path.' }
        Remove-Item -LiteralPath $target -Force
    }
}
# Source is delivered physically inside both Portable and Setup, not by future promise.
$thirdPartySource = Join-Path $OutputDir "LuckyWheel-$Version-third-party-sources.zip"
if (!(Test-Path -LiteralPath $thirdPartySource)) { throw 'Missing corresponding third-party source ZIP.' }
Add-Type -AssemblyName System.IO.Compression
$sourceArchive = [IO.Compression.ZipFile]::OpenRead($thirdPartySource)
try {
    $provenance = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'licenses\source-provenance.json') -Raw | ConvertFrom-Json
    foreach ($source in $provenance.sources) {
        $entry = $sourceArchive.GetEntry('sources/' + $source.file)
        if (!$entry) { throw "Missing source archive: $($source.file)" }
        $stream = $entry.Open()
        $sha = [Security.Cryptography.SHA256]::Create()
        try { $actual = [Convert]::ToHexString($sha.ComputeHash($stream)).ToLowerInvariant() }
        finally { $stream.Dispose(); $sha.Dispose() }
        if ($actual -ne $source.sha256) { throw "Source archive hash mismatch: $($source.file)" }
    }
    foreach ($file in @('SOURCE-DELIVERY.md','source-provenance.json')) {
        $entry = $sourceArchive.GetEntry($file)
        if (!$entry) { throw "Missing source metadata: $file" }
        $reader = [IO.StreamReader]::new($entry.Open())
        try { $inside = $reader.ReadToEnd() } finally { $reader.Dispose() }
        if ($inside -ne (Get-Content -LiteralPath (Join-Path $PSScriptRoot "licenses\$file") -Raw)) { throw "Stale source metadata: $file" }
    }
    foreach ($file in Get-ChildItem -LiteralPath (Join-Path $PSScriptRoot 'licenses\build-info') -File) {
        $entry = $sourceArchive.GetEntry('build-info/' + $file.Name)
        if (!$entry) { throw "Missing build information: $($file.Name)" }
        $reader = [IO.StreamReader]::new($entry.Open())
        try { $inside = $reader.ReadToEnd() } finally { $reader.Dispose() }
        if ($inside -ne (Get-Content -LiteralPath $file.FullName -Raw)) { throw "Stale build information: $($file.Name)" }
    }
} finally { $sourceArchive.Dispose() }
$payload = Join-Path $BuildDir 'dist'
if (![IO.Directory]::Exists($payload)) { throw 'Missing deployed payload.' }
$builtExe = Join-Path $BuildDir 'LuckyWheel.exe'
if (!(Test-Path -LiteralPath $builtExe) -or
    (Get-FileHash -LiteralPath $builtExe -Algorithm SHA256).Hash -ne
    (Get-FileHash -LiteralPath (Join-Path $payload 'LuckyWheel.exe') -Algorithm SHA256).Hash) {
    throw 'Payload executable does not match the current build. Redeploy before packaging.'
}
$termsBytes = [IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'licenses\END-USER-TERMS.txt'))
if ($termsBytes.Length -lt 3 -or $termsBytes[0] -ne 239 -or $termsBytes[1] -ne 187 -or $termsBytes[2] -ne 191) {
    throw 'NSIS license text must have a UTF-8 BOM.'
}
$exeVersion = (Get-Item -LiteralPath (Join-Path $payload 'LuckyWheel.exe')).VersionInfo.ProductVersion
if ($exeVersion -ne $Version) { throw "Payload version $exeVersion differs from CMake version $Version." }
foreach ($required in @('LuckyWheel.exe','Qt6Core.dll','Qt6Sql.dll','msvcp140.dll','vcruntime140.dll','vcruntime140_1.dll','platforms\qwindows.dll','sqldrivers\qsqlite.dll')) {
    if (!(Test-Path (Join-Path $payload $required))) { throw "Missing deployed dependency: $required" }
}
foreach ($license in @('LGPL-3.0.txt','GPL-3.0.txt','LGPL-2.1.txt','THIRD-PARTY-NOTICES.txt','Qt-Translations-BSD-3-Clause.txt','SOURCE-DELIVERY.md','source-provenance.json','Microsoft-SDK-LICENSE.rtf','Microsoft-SDK-THIRD-PARTY-NOTICES.rtf','MICROSOFT-REDIST.md','END-USER-TERMS.txt','Microsoft-VS2026-Community-LICENSE.txt')) {
    if (!(Test-Path (Join-Path $PSScriptRoot "licenses\$license"))) { throw "Missing license file: $license" }
}
if (Test-Path (Join-Path $payload 'Qt6Pdf.dll')) { throw 'Unused Qt PDF module found in payload; rebuild with build.cmd.' }
# Replace rather than merge so a second run cannot nest licenses\licenses or keep removed files.
$payloadSources = Join-Path $payload 'sources'
New-Item -ItemType Directory -Force $payloadSources | Out-Null
Copy-Item -LiteralPath $thirdPartySource -Destination (Join-Path $payloadSources ([IO.Path]::GetFileName($thirdPartySource))) -Force
$payloadLicenses = [IO.Path]::GetFullPath((Join-Path $payload 'licenses'))
if ([IO.Path]::GetDirectoryName($payloadLicenses) -ne $payload -or ((Get-Item -LiteralPath $payloadLicenses -ErrorAction SilentlyContinue).Attributes -band [IO.FileAttributes]::ReparsePoint)) { throw 'Unsafe licenses directory.' }
if (Test-Path $payloadLicenses) { Remove-Item -LiteralPath $payloadLicenses -Recurse -Force }
Copy-Item (Join-Path $PSScriptRoot 'licenses') $payloadLicenses -Recurse -Force
Copy-Item (Join-Path $PSScriptRoot 'THIRD-PARTY.md') $payload -Force
Copy-Item $statusFile $payload -Force
Copy-Item (Join-Path $PSScriptRoot 'LICENSE') (Join-Path $payload 'LICENSE.txt') -Force
New-Item -ItemType Directory -Force $OutputDir | Out-Null
$manifest = Join-Path $BuildDir 'uninstall-files.nsh'
$lines = @(Get-ChildItem -LiteralPath $payload -File -Recurse | ForEach-Object {
    $relative = $_.FullName.Substring($payload.Length).TrimStart('\')
    'Delete "$INSTDIR\' + $relative + '"'
})
$lines += @(Get-ChildItem -LiteralPath $payload -Directory -Recurse | Sort-Object { $_.FullName.Length } -Descending | ForEach-Object {
    $relative = $_.FullName.Substring($payload.Length).TrimStart('\')
    'RMDir "$INSTDIR\' + $relative + '"'
})
$lines | Set-Content -LiteralPath $manifest -Encoding utf8BOM
$nsisArgs = @("/DPAYLOAD=$payload", "/DOUTPUT=$output", "/DREMOVE_MANIFEST=$manifest", "/DVERSION=$Version")
if ($Validation) { $nsisArgs += '/DVALIDATION' }
& (Join-Path $NsisRoot 'makensis.exe') @nsisArgs (Join-Path $PSScriptRoot 'installer\LuckyWheel.nsi')
if ($LASTEXITCODE) { throw 'NSIS packaging failed.' }
Compress-Archive -Path (Join-Path $payload '*') -DestinationPath $portable
# Source archive uses an allowlist rather than recursively collecting the workspace.
$rootFiles = @('.gitignore','.clang-format','CMakeLists.txt','CMakePresets.json','build.cmd','package.ps1',
    'LICENSE','README.md','BUILD.md','INSTALL.md','THIRD-PARTY.md','RELEASE-STATUS.md','RELEASE-NOTES-v2.0.8.md')
$sourceFiles = @($rootFiles | ForEach-Object { Get-Item -LiteralPath (Join-Path $PSScriptRoot $_) })
$sourceFiles += @(Get-ChildItem -LiteralPath $PSScriptRoot -Filter 'CHANGELOG-v*.md' -File)
foreach ($dir in @('src','tests','assets','examples','installer','licenses')) {
    $sourceFiles += @(Get-ChildItem -LiteralPath (Join-Path $PSScriptRoot $dir) -Recurse -File)
}
Add-Type -AssemblyName System.IO.Compression
$archive = [IO.Compression.ZipFile]::Open($sourceZip, [IO.Compression.ZipArchiveMode]::Create)
try {
    foreach ($file in $sourceFiles | Sort-Object FullName -Unique) {
        $relative = [IO.Path]::GetRelativePath($PSScriptRoot, $file.FullName).Replace('\','/')
        if ($relative.StartsWith('../') -or $file.Extension -match '^\.(sqlite|db|key|pem|pfx|p12|log)$') {
            throw "Unexpected source file: $relative"
        }
        [IO.Compression.ZipFileExtensions]::CreateEntryFromFile($archive, $file.FullName, $relative,
            [IO.Compression.CompressionLevel]::Optimal) | Out-Null
    }
} finally { $archive.Dispose() }
$deliverables = @($output,$portable,$sourceZip)
$thirdPartySource = Join-Path $OutputDir "LuckyWheel-$Version-third-party-sources.zip"
if (Test-Path -LiteralPath $thirdPartySource) { $deliverables += $thirdPartySource }
$hashes = @(Get-FileHash -Algorithm SHA256 -LiteralPath $deliverables)
$hashes | ForEach-Object { $_.Hash.ToLowerInvariant() + '  ' + [IO.Path]::GetFileName($_.Path) } |
    Set-Content -LiteralPath $hashFile -Encoding utf8
$hashes
