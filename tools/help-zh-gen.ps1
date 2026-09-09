# help-zh-gen.ps1 - generate static_helpzh.h (Chinese help, GB18030 bytes escaped)
# Input : giacbf/static_help.h (UTF-8 master) + tools/help-zh.json (translations)
# Output: giacbf/static_helpzh.h (pure-ASCII C source; all non-ASCII -> GB18030 \xNN)
# ASCII-only script body (PS 5.1 reads scripts as ANSI). All file IO explicit UTF-8.
$ErrorActionPreference = 'Stop'

$masterPath = 'D:\GitHub\khicas-opt\giacbf\static_help.h'
$jsonPath   = 'D:\GitHub\khicas-opt\tools\help-zh.json'
$outPath    = 'D:\GitHub\khicas-opt\giacbf\static_helpzh.h'

$utf8 = New-Object System.Text.UTF8Encoding($false)   # no BOM
# master is read as Latin-1 (1 byte == 1 char) so byte-wise C-literal parsing is safe
$latin1 = [System.Text.Encoding]::GetEncoding(28591)
$master = [System.IO.File]::ReadAllText($masterPath, $latin1)
$jsonText = [System.IO.File]::ReadAllText($jsonPath, [System.Text.Encoding]::UTF8)
$translations = $jsonText | ConvertFrom-Json

# resolve a C string literal into raw bytes (escape-resolved); $null for '0'
function Decode-CStringBytes([string]$lit) {
    if ($lit -eq '0') { return $null }
    $inner = $lit.Substring(1, $lit.Length - 2)
    $ms = New-Object System.IO.MemoryStream
    $i = 0
    while ($i -lt $inner.Length) {
        $ch = $inner[$i]
        if ($ch -eq [char]0x5C) { # backslash
            $nx = $inner[$i + 1]
            if ($nx -eq 'x') {
                $hex = $inner.Substring($i + 2, 2)
                $ms.WriteByte([byte][Convert]::ToInt32($hex, 16))
                $i += 4; continue
            }
            if ($nx -eq 'n') { $ms.WriteByte(10); $i += 2; continue }
            if ($nx -eq 't') { $ms.WriteByte(9); $i += 2; continue }
            $ms.WriteByte([byte][int][char]$nx); $i += 2; continue
        }
        # ASCII byte (master is UTF-8; chars < 0x80 are ASCII)
        $ms.WriteByte([byte][int][char]$ch); $i++
    }
    return $ms.ToArray()
}

# raw bytes -> PS string via UTF-8 (master is UTF-8)
function From-UTF8Bytes([byte[]]$bytes) {
    if ($bytes -eq $null) { return '' }
    return ([System.Text.Encoding]::UTF8.GetString($bytes))
}

# PS string -> GB18030 bytes -> escaped C literal, prefixed with \x01 marker.
# The 0x01 marker tells the render layer "this string is GB18030" (see
# giacbf/khicas_gb18030.h). Other strings keep their raw bytes untouched.
$script:fourByteChars = 0
function To-MarkedGB18030([string]$text) {
    $enc = [System.Text.Encoding]::GetEncoding('GB18030')
    $bytes = New-Object System.Collections.Generic.List[byte]
    foreach ($ch in $text.ToCharArray()) {
        $cb = $enc.GetBytes([string]$ch)
        if ($cb.Length -gt 2) {
            $script:fourByteChars++
            $bytes.Add([byte][char]'?')
        } else {
            foreach ($b in $cb) { $bytes.Add($b) }
        }
    }
    $sb = New-Object System.Text.StringBuilder
    [void]$sb.Append('"')
    [void]$sb.Append('\x01')
    foreach ($b in $bytes) {
        if ($b -ge 0x80) {
            # content-level ""\xNN"" -> caller quotes yield "ascii""\xNN""ascii"
            [void]$sb.Append('""')
            [void]$sb.Append('\x')
            [void]$sb.Append($b.ToString('x2'))
            [void]$sb.Append('""')
        } elseif ($b -eq 0x22) { [void]$sb.Append('\"')
        } elseif ($b -eq 0x5C) { [void]$sb.Append('\\')
        } else { [void]$sb.Append([char]$b) }
    }
    [void]$sb.Append('"')
    return $sb.ToString()
}

$linePat = '^\{(?<name>"(?:[^"\\]|\\.)*"|0),\{(?<h0>(?:"(?:[^"\\]|\\.)*"|0)),(?<h1>(?:"(?:[^"\\]|\\.)*"|0)),(?<h2>(?:"(?:[^"\\]|\\.)*"|0)),(?<h3>(?:"(?:[^"\\]|\\.)*"|0)),(?<h4>(?:"(?:[^"\\]|\\.)*"|0))\},(?<syntax>(?:"(?:[^"\\]|\\.)*"|0)),(?<related>(?:"(?:[^"\\]|\\.)*"|0)),(?<examples>(?:"(?:[^"\\]|\\.)*"|0))\},?$'
$re = New-Object System.Text.RegularExpressions.Regex($linePat)

$out = New-Object System.Text.StringBuilder
$total = 0; $translated = 0; $skipped = 0

foreach ($line in $master -split "`n") {
    $line = $line.TrimEnd("`r", "`n").Trim()
    if ($line -eq '') { continue }
    $m = $re.Match($line)
    if (-not $m.Success) { $skipped++; continue }
    $total++

    $nameLit = $m.Groups['name'].Value
    $h1      = $m.Groups['h1'].Value
    $syntax  = $m.Groups['syntax'].Value
    $related = $m.Groups['related'].Value
    $examples = $m.Groups['examples'].Value

    # decode cmd name for lookup
    $cmdBytes = Decode-CStringBytes $nameLit
    $cmd = ''
    if ($cmdBytes -ne $null) { $cmd = From-UTF8Bytes $cmdBytes }
    $zh = $null
    if ($cmd -ne '') {
        $prop = $translations.PSObject.Properties[$cmd]
        if ($prop -ne $null) { $zh = $prop.Value }
    }

    if ($zh) {
        $howto = To-MarkedGB18030 $zh
        $translated++
    } else {
        # untranslated: keep the master's raw literal byte-for-byte
        # (identical rendering to the English build)
        $howto = $h1
    }

    [void]$out.Append('{')
    [void]$out.Append($nameLit); [void]$out.Append(',')
    [void]$out.Append($howto); [void]$out.Append(',')
    [void]$out.Append($syntax); [void]$out.Append(',')
    [void]$out.Append($related); [void]$out.Append(',')
    [void]$out.Append($examples)
    [void]$out.Append('},')
    [void]$out.Append("`n")
}

[System.IO.File]::WriteAllText($outPath, $out.ToString(), $utf8)
Write-Host "done: $total entries, $translated translated, $skipped skipped, $script:fourByteChars 4-byte-chars->'?'"
Write-Host "out: $outPath"
