# script to create a zip archive that can be used as user template in Visual Studio
# https://learn.microsoft.com/en-us/visualstudio/ide/how-to-locate-and-organize-project-and-item-templates?view=vs-2022
# usage: CreateVSTemplateArchive.ps1
# optional: copy or symlink the archive to C:\Users\<username>\Documents\Visual Studio 2022\Templates\ProjectTemplates\<archive>.zip

$srcpaths = $(".\metadata\*", ".\BOF-Template\*")
$dst = ".\BOF-Template-v410.zip"

function ReplaceTextInFile {
    param (
        [string]$file,
        [string]$pattern,
        [string]$replacement
    )
    $content = Get-Content -Path $file
    $content = $content -replace $pattern, $replacement
    $content | Set-Content -Path $file
}

$tempDir = New-Item -ItemType Directory -Path ".\temp" -Force
if ($null -eq $tempDir) {
    Write-Host "[!] failed to create temporary directory." -ForegroundColor Red
    exit
}

# move project files to temporary directory
foreach ($file in $srcpaths) {
    Copy-Item -Recurse -Path $file -Destination $tempDir.FullName -Force
}

# rename BOF-Template\base to BOF-Template\BASE
Rename-Item -Path "$tempDir\base" -NewName "base_tmp" -Force
Rename-Item -Path "$tempDir\base_tmp" -NewName "BASE" -Force

# replace static guid and project name in vcxproj file
ReplaceTextInFile "$tempDir\BOF-Template.vcxproj" "{58ebee7c-b0cf-49e3-ae82-e60750c03613}" '{$guid1$}'
ReplaceTextInFile "$tempDir\BOF-Template.vcxproj" "<RootNamespace>BOFTemplate</RootNamespace>" '<RootNamespace>$safeprojectname$</RootNamespace>'

# create archive
Compress-Archive -Path "$($tempDir.FullName)\*" -DestinationPath $dst -Force
if (-not (Test-Path $dst)) {
    Write-Host "[!] failed to create archive." -ForegroundColor Red
    exit
} else {
    Remove-Item -Path $tempDir.FullName -Recurse -Force
    Write-Host "[*] Created archive: $dst" -ForegroundColor Green
}
