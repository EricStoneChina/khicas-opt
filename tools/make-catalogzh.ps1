# make-catalogzh.ps1 - regenerate giacbf/catalogzh.cpp from giacbf/catalogen.cpp
#
# catalogzh.cpp is the Chinese build's catalog object. Two differences:
#   1. ram_filename -> \\fls0\khicaszh.8c2  (so khicaszh loads khicaszh.ac2;
#      main.cc chk_ram2M only rewrites the .8c2 extension to .ac2/.882)
#   2. apropos_string -> Chinese About text. Every line is prefixed with the 0x01
#      GB18030 marker (textGUI's add() splits the string into one element per
#      line, and the marker is interpreted per element), non-ASCII bytes are
#      emitted as GB18030 \xNN escapes so the file stays pure-ASCII C source.
#
# Run after editing catalogen.cpp:  powershell -File tools/make-catalogzh.ps1
$ErrorActionPreference = 'Stop'

$srcPath = 'D:\GitHub\khicas-opt\giacbf\catalogen.cpp'
$outPath = 'D:\GitHub\khicas-opt\giacbf\catalogzh.cpp'
$latin1 = [System.Text.Encoding]::GetEncoding(28591)
$src = [System.IO.File]::ReadAllText($srcPath, $latin1)

# --- 1. ram_filename ---
$oldName = 'const char ram_filename[]="\\\\fls0\\khicas50.8c2";'
$newName = 'const char ram_filename[]="\\\\fls0\\khicaszh.8c2";'
if (-not $src.Contains($oldName)) { throw "ram_filename line not found in catalogen.cpp" }
$src = $src.Replace($oldName, $newName)

# --- 2. Chinese apropos_string ---
# NOTE: the Chinese lines live in a separate UTF-8 file. PowerShell 5.1 reads
# .ps1 files as ANSI, so Chinese literals inside this script would be mangled
# (and re-encoding them would silently yield UTF-8 bytes instead of GB18030).
$aboutPath = 'D:\GitHub\khicas-opt\tools\about-zh.txt'
$aboutText = [System.IO.File]::ReadAllText($aboutPath, [System.Text.Encoding]::UTF8)
$zhLines = $aboutText -split "`r?`n"
if ($zhLines[-1] -eq '') { $zhLines = $zhLines[0..($zhLines.Count-2)] }
$gb = [System.Text.Encoding]::GetEncoding('GB18030')
$sb = New-Object System.Text.StringBuilder
[void]$sb.Append('const char apropos_string[]="')
for ($i = 0; $i -lt $zhLines.Count; $i++) {
  [void]$sb.Append('\x01')
  foreach ($b in $gb.GetBytes($zhLines[$i])) {
    if ($b -ge 0x80) { [void]$sb.Append('\x'); [void]$sb.Append($b.ToString('x2')) }
    elseif ($b -eq 0x22) { [void]$sb.Append('\"') }
    elseif ($b -eq 0x5C) { [void]$sb.Append('\\') }
    else { [void]$sb.Append([char]$b) }
  }
  if ($i -lt $zhLines.Count - 1) { [void]$sb.Append('\n') }
}
[void]$sb.Append('";')
$newApropos = $sb.ToString()

$lines = $src -split "`n"
$found = $false
$out = foreach ($l in $lines) {
  if ($l.StartsWith('const char apropos_string[]=')) { $found = $true; $newApropos } else { $l }
}
if (-not $found) { throw "apropos_string line not found in catalogen.cpp" }
$src = $out -join "`n"

[System.IO.File]::WriteAllText($outPath, $src, $latin1)
Write-Host "catalogzh.cpp regenerated: $($src.Length) chars"
