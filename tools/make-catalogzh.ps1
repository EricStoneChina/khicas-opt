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

# PowerShell property lookup is case-insensitive; catalog commands are not.
function Get-ExactProperty($object, [string]$name) {
    foreach ($property in $object.PSObject.Properties) {
        if ([string]::Equals($property.Name, $name, [StringComparison]::Ordinal)) {
            return $property
        }
    }
    return $null
}

function Get-TranslationProperty($name) {
    $property = Get-ExactProperty $zhHelp $name
    if ($property -eq $null -and $zhHelp.caseSensitive -ne $null) {
        $property = Get-ExactProperty $zhHelp.caseSensitive $name
    }
    return $property
}

# --- 1. ram_filename ---
$oldName = 'const char ram_filename[]="\\\\fls0\\khicas50.8c2";'
$newName = 'const char ram_filename[]="\\\\fls0\\khicaszh.8c2";'
if (-not $src.Contains($oldName)) { throw "ram_filename line not found in catalogen.cpp" }
$src = $src.Replace($oldName, $newName)

# --- 1b. enable the Chinese UI strings for this build ---
$oldFlag = 'int zh_ui_enabled=0; /* Chinese UI strings off for the English build */'
$newFlag = 'int zh_ui_enabled=1; /* Chinese UI strings on (see zhui.cc) */'
if (-not $src.Contains($oldFlag)) { throw "zh_ui_enabled line not found in catalogen.cpp" }
$src = $src.Replace($oldFlag, $newFlag)

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
    [void]$sb.Append('""')   # close+reopen: a following hex digit must not merge into the escape
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

# --- 3. patch completeCat entries that have a Chinese translation ---
# Entry layout: {"name", insert|0, "desc"|0, "example"|0, "example2"|0, CATEGORY},
#   desc     -> Chinese howto (0x01 marker + GB18030 bytes)
#   example  -> "#<ex1>"  (the '#' makes the help box show/insert it verbatim
#               instead of wrapping it as name+example+")")
#   example2 -> "#<ex2>"  (or 0)
$zhHelpPath = 'D:\GitHub\khicas-opt\tools\help-zh.json'
$zhHelp = [System.IO.File]::ReadAllText($zhHelpPath, [System.Text.Encoding]::UTF8) | ConvertFrom-Json

function To-HashCString([string]$text) {
    $sb = New-Object System.Text.StringBuilder
    [void]$sb.Append('"#')
    foreach ($ch in $text.ToCharArray()) {
        if ($ch -eq '"') { [void]$sb.Append('\"') }
        elseif ($ch -eq [char]0x5C) { [void]$sb.Append('\\') }
        else { [void]$sb.Append($ch) }
    }
    [void]$sb.Append('"')
    return $sb.ToString()
}

function To-MarkedGB18030([string]$text) {
    $enc = [System.Text.Encoding]::GetEncoding('GB18030')
    $sb = New-Object System.Text.StringBuilder
    [void]$sb.Append('"')
    [void]$sb.Append('\x01')
    [void]$sb.Append('""')   # close+reopen: a following hex digit must not merge into the escape
    foreach ($ch in $text.ToCharArray()) {
        $cb = $enc.GetBytes([string]$ch)
        if ($cb.Length -gt 2) { $cb = @([byte][char]'?') }
        foreach ($b in $cb) {
            if ($b -ge 0x80) {
                [void]$sb.Append('""'); [void]$sb.Append('\x'); [void]$sb.Append($b.ToString('x2')); [void]$sb.Append('""')
            } elseif ($b -eq 0x22) { [void]$sb.Append('\"')
            } elseif ($b -eq 0x5C) { [void]$sb.Append('\\')
            } else { [void]$sb.Append([char]$b) }
        }
    }
    [void]$sb.Append('"')
    return $sb.ToString()
}

$catRe = New-Object System.Text.RegularExpressions.Regex('^(?<indent>\s*)\{(?<name>"(?:[^"\\]|\\.)*")\s*,\s*(?<insert>0|"(?:[^"\\]|\\.)*")\s*,\s*(?<desc>0|"(?:[^"\\]|\\.)*")\s*,\s*(?<ex1>0|"(?:[^"\\]|\\.)*")\s*,\s*(?<ex2>0|"(?:[^"\\]|\\.)*")\s*,\s*(?<cat>[^}]*)\}\s*,\s*$')
$lines = $src -split "`n"
$patched = 0
$out2 = foreach ($l in $lines) {
    $m = $catRe.Match($l)
    if (-not $m.Success) { $l; continue }
    $nm = $m.Groups['name'].Value.Trim('"')
    $base = $nm
    $pi = $base.IndexOf('(')
    if ($pi -gt 0) { $base = $base.Substring(0, $pi) }
    $prop = Get-TranslationProperty $nm
    if ($prop -eq $null) { $prop = Get-TranslationProperty $base }
    if ($prop -eq $null -or $prop.Value -is [string]) { $l; continue }
    $val = $prop.Value
    if (-not $val.howto -or -not $val.ex1) { $l; continue }
    $descLit = To-MarkedGB18030 $val.howto
    $ex1Lit = To-HashCString $val.ex1
    $ex2Lit = if ($val.ex2) { To-HashCString $val.ex2 } else { '0' }
    $patched++
    '{0}{{{1}, {2}, {3}, {4}, {5}, {6}}},' -f $m.Groups['indent'].Value, $m.Groups['name'].Value, $m.Groups['insert'].Value, $descLit, $ex1Lit, $ex2Lit, $m.Groups['cat'].Value
}
$src = $out2 -join "`n"

# --- 4. Chinese catalog category names ---
$catPath = 'D:\GitHub\khicas-opt\tools\ui-cat-zh.txt'
$catText = [System.IO.File]::ReadAllText($catPath, [System.Text.Encoding]::UTF8)
$catMap = @{}
foreach ($ln in ($catText -split "`r?`n")) {
    if ($ln -eq '') { continue }
    $kv = $ln -split '=', 2
    if ($kv.Count -eq 2) { $catMap[$kv[0].Trim()] = $kv[1] }
}
$catRe = New-Object System.Text.RegularExpressions.Regex('^(?<head>\s*menuitems\[(?<cat>CAT_CATEGORY_\w+)\]\.text\s*=\s*).*$')
$lines = $src -split "`n"
$catPatched = 0
$out3 = foreach ($l in $lines) {
    $m = $catRe.Match($l)
    if (-not $m.Success) { $l; continue }
    $cat = $m.Groups['cat'].Value
    if (-not $catMap.ContainsKey($cat)) { $l; continue }
    $catPatched++
    $m.Groups['head'].Value + '(char*)' + (To-MarkedGB18030 $catMap[$cat]) + ';'
}
$src = $out3 -join "`n"

[System.IO.File]::WriteAllText($outPath, $src, $latin1)
Write-Host "catalogzh.cpp regenerated: $($src.Length) chars, $patched catalog entries translated, $catPatched categories translated"
