# icon-analyze2.ps1 - measure subject bbox excluding background (ASCII only)
Add-Type -AssemblyName System.Drawing

function Analyze-Subject([string]$path, [string]$label, [int]$bgDistThresh = 100) {
    $bmp = [System.Drawing.Bitmap]::FromFile($path)
    $w = $bmp.Width; $h = $bmp.Height
    $c = @($bmp.GetPixel(0,0), $bmp.GetPixel($w-1,0), $bmp.GetPixel(0,$h-1), $bmp.GetPixel($w-1,$h-1))
    $bgR = ($c | Measure-Object -Property R -Average).Average
    $bgG = ($c | Measure-Object -Property G -Average).Average
    $bgB = ($c | Measure-Object -Property B -Average).Average

    $minX = $w; $maxX = -1; $minY = $h; $maxY = -1
    $colors = @{}
    $subject = 0
    for ($y = 0; $y -lt $h; $y++) {
        for ($x = 0; $x -lt $w; $x++) {
            $p = $bmp.GetPixel($x, $y)
            $dist = [Math]::Abs($p.R - $bgR) + [Math]::Abs($p.G - $bgG) + [Math]::Abs($p.B - $bgB)
            if ($dist -gt $bgDistThresh) {
                $subject++
                if ($x -lt $minX) { $minX = $x }; if ($x -gt $maxX) { $maxX = $x }
                if ($y -lt $minY) { $minY = $y }; if ($y -gt $maxY) { $maxY = $y }
                $key = "{0},{1},{2}" -f ([Math]::Round($p.R/32)*32), ([Math]::Round($p.G/32)*32), ([Math]::Round($p.B/32)*32)
                if ($colors.ContainsKey($key)) { $colors[$key]++ } else { $colors[$key] = 1 }
            }
        }
    }
    Write-Host "=== $label ==="
    Write-Host ("  bg(4 corners): RGB({0:N0},{1:N0},{2:N0})" -f $bgR, $bgG, $bgB)
    if ($subject -eq 0) { Write-Host "  no subject found"; $bmp.Dispose(); return }
    $mL = $minX; $mT = $minY; $mR = $w - 1 - $maxX; $mB = $h - 1 - $maxY
    Write-Host ("  bbox: x[{0}..{1}] y[{2}..{3}]" -f $minX, $maxX, $minY, $maxY)
    Write-Host ("  margins L{0} T{1} R{2} B{3}  -> min {4}px" -f $mL, $mT, $mR, $mB, ([Math]::Min($mL, [Math]::Min($mT, [Math]::Min($mR, $mB)))))
    Write-Host ("  subject size {0}x{1} (canvas 92x64)" -f ($maxX-$minX+1), ($maxY-$minY+1))
    $top = $colors.GetEnumerator() | Sort-Object Value -Descending | Select-Object -First 5
    Write-Host ("  subject colors: {0}" -f (($top | ForEach-Object { "rgb($($_.Key))x$($_.Value)" }) -join "  "))
    $bmp.Dispose()
}

$ref = 'D:\DataCherryStudio\Data\Agents\system\2026-09-09\bb43ed99-aecc-467f-91fd-77bff21182b7\build\2048'
$khi = 'D:\GitHub\khicas-opt\giacbf'
Analyze-Subject "$ref\unselected.bmp" "2048 unselected digit"
Analyze-Subject "$ref\selected.bmp"   "2048 selected digit"
Analyze-Subject "$khi\khicasio.png"   "KhiCAS unselected chi (black bg)"
Analyze-Subject "$khi\khicasio1.png"  "KhiCAS selected chi (blue bg)"
