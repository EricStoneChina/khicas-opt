# help-zh-gen.ps1 - generate giacbf/static_helpzh.h (Chinese help)
# Input : giacbf/static_help.h (UTF-8 master) + tools/help-zh.json (translations)
# Output: giacbf/static_helpzh.h (pure-ASCII C source)
#
# JSON schema (both forms accepted):
#   "cmd": "中文说明"                                        (howto only)
#   "cmd": { "howto": "中文说明+用法", "ex1": "...", "ex2": "..." }
#
# - howto becomes a GB18030 byte string prefixed with the 0x01 marker
# - ex1/ex2 replace the entry's examples field (plain CAS expressions, ';'-joined);
#   entries without a translation keep the master's raw bytes untouched
#
# ASCII-only script body (PS 5.1 reads scripts as ANSI). File IO is explicit.
$ErrorActionPreference = 'Stop'

$masterPath = 'D:\GitHub\khicas-opt\giacbf\static_help.h'
$jsonPath   = 'D:\GitHub\khicas-opt\tools\help-zh.json'
$outPath    = 'D:\GitHub\khicas-opt\giacbf\static_helpzh.h'

$utf8 = New-Object System.Text.UTF8Encoding($false)   # no BOM
# master read as Latin-1 (1 byte == 1 char) so byte-wise C-literal parsing is safe
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
        if ($ch -eq [char]0x5C) {
            $nx = $inner[$i + 1]
            if ($nx -eq 'x') {
                $ms.WriteByte([byte][Convert]::ToInt32($inner.Substring($i + 2, 2), 16)); $i += 4; continue
            }
            if ($nx -eq 'n') { $ms.WriteByte(10); $i += 2; continue }
            if ($nx -eq 't') { $ms.WriteByte(9); $i += 2; continue }
            $ms.WriteByte([byte][int][char]$nx); $i += 2; continue
        }
        $ms.WriteByte([byte][int][char]$ch); $i++
    }
    return $ms.ToArray()
}

function From-UTF8Bytes([byte[]]$bytes) {
    if ($bytes -eq $null) { return '' }
    return ([System.Text.Encoding]::UTF8.GetString($bytes))
}

# PS string -> GB18030 bytes -> C literal with the 0x01 marker (pure ASCII out)
# chars needing >2 GB18030 bytes (rare) fall back to '?'
$script:fourByteChars = 0
function To-MarkedGB18030([string]$text) {
    $enc = [System.Text.Encoding]::GetEncoding('GB18030')
    $bytes = New-Object System.Collections.Generic.List[byte]
    foreach ($ch in $text.ToCharArray()) {
        $cb = $enc.GetBytes([string]$ch)
        if ($cb.Length -gt 2) { $script:fourByteChars++; $bytes.Add([byte][char]'?') }
        else { foreach ($b in $cb) { $bytes.Add($b) } }
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

# plain ASCII C string (examples are CAS code)
function To-PlainCString([string]$text) {
    $sb = New-Object System.Text.StringBuilder
    [void]$sb.Append('"')
    foreach ($ch in $text.ToCharArray()) {
        if ($ch -eq '"') { [void]$sb.Append('\"') }
        elseif ($ch -eq [char]0x5C) { [void]$sb.Append('\\') }
        else { [void]$sb.Append($ch) }
    }
    [void]$sb.Append('"')
    return $sb.ToString()
}

# data field order in static_help.h is: name,{howto*5},syntax,EXAMPLES,RELATED
# (the struct in khelpfr.cc names the last two the other way round)
$linePat = '^\{(?<name>"(?:[^"\\]|\\.)*"|0),\{(?<h0>(?:"(?:[^"\\]|\\.)*"|0)),(?<h1>(?:"(?:[^"\\]|\\.)*"|0)),(?<h2>(?:"(?:[^"\\]|\\.)*"|0)),(?<h3>(?:"(?:[^"\\]|\\.)*"|0)),(?<h4>(?:"(?:[^"\\]|\\.)*"|0))\},(?<syntax>(?:"(?:[^"\\]|\\.)*"|0)),(?<examples>(?:"(?:[^"\\]|\\.)*"|0)),(?<related>(?:"(?:[^"\\]|\\.)*"|0))\},?$'
$re = New-Object System.Text.RegularExpressions.Regex($linePat)

$out = New-Object System.Text.StringBuilder
$total = 0; $translated = 0; $withEx = 0; $skipped = 0

foreach ($line in $master -split "`n") {
    $line = $line.TrimEnd("`r", "`n").Trim()
    if ($line -eq '') { continue }
    $m = $re.Match($line)
    if (-not $m.Success) { $skipped++; continue }
    $total++

    $nameLit  = $m.Groups['name'].Value
    $h1       = $m.Groups['h1'].Value
    $syntax   = $m.Groups['syntax'].Value
    $examples = $m.Groups['examples'].Value   # 4th data field = real examples
    $related  = $m.Groups['related'].Value    # 5th data field = real related

    $cmdBytes = Decode-CStringBytes $nameLit
    $cmd = ''
    if ($cmdBytes -ne $null) { $cmd = From-UTF8Bytes $cmdBytes }

    $zhHowto = $null; $zhEx1 = $null; $zhEx2 = $null
    if ($cmd -ne '') {
        $prop = $translations.PSObject.Properties[$cmd]
        if ($prop -ne $null) {
            $val = $prop.Value
            if ($val -is [string]) { $zhHowto = $val }
            else {
                if ($val.PSObject.Properties['howto']) { $zhHowto = $val.howto }
                if ($val.PSObject.Properties['ex1'])   { $zhEx1   = $val.ex1 }
                if ($val.PSObject.Properties['ex2'])   { $zhEx2   = $val.ex2 }
            }
        }
    }

    if ($zhHowto) { $howto = To-MarkedGB18030 $zhHowto; $translated++ }
    else { $howto = $h1 }

    $exField = $examples
    if ($zhEx1) {
        $ex = $zhEx1
        if ($zhEx2) { $ex = "$zhEx1;$zhEx2" }
        $exField = To-PlainCString $ex
        $withEx++
    }

    [void]$out.Append('{')
    [void]$out.Append($nameLit); [void]$out.Append(',')
    [void]$out.Append($howto); [void]$out.Append(',')
    [void]$out.Append($syntax); [void]$out.Append(',')
    [void]$out.Append($exField); [void]$out.Append(',')
    [void]$out.Append($related)
    [void]$out.Append('},')
    [void]$out.Append("`n")
}

# write as Latin-1: untranslated entries are copied byte-for-byte from the
# master (which is UTF-8), so a UTF-8 write would double-encode those bytes
[System.IO.File]::WriteAllText($outPath, $out.ToString(), $latin1)
Write-Host "static_helpzh.h: $total entries, $translated translated, $withEx with curated examples, $skipped skipped, $script:fourByteChars 4-byte-chars->'?'"
Write-Host "out: $outPath"
