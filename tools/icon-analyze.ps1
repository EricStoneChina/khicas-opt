# icon-analyze.ps1 — 像素级对比分析图标(2048 风格 vs KhiCAS 当前图标)
# 用法: pwsh -File icon-analyze.ps1
Add-Type -AssemblyName System.Drawing

function Analyze-Img([string]$path, [string]$label) {
    $bmp = [System.Drawing.Bitmap]::FromFile($path)
    $w = $bmp.Width; $h = $bmp.Height
    Write-Host "`n=== $label ==="
    Write-Host "  文件: $path"
    Write-Host "  尺寸: ${w}x${h}"

    # 四角背景色
    $c = @($bmp.GetPixel(0,0), $bmp.GetPixel($w-1,0), $bmp.GetPixel(0,$h-1), $bmp.GetPixel($w-1,$h-1))
    $bgR = ($c | Measure-Object -Property R -Average).Average
    $bgG = ($c | Measure-Object -Property G -Average).Average
    $bgB = ($c | Measure-Object -Property B -Average).Average
    Write-Host ("  四角平均背景色: RGB({0:N0},{1:N0},{2:N0})" -f $bgR, $bgG, $bgB)

    # 扫描:背景判定(距背景 > 40 视为主体),统计主色 + 边界框
    $minX = $w; $maxX = -1; $minY = $h; $maxY = -1
    $colors = @{}
    $subject = 0
    for ($y = 0; $y -lt $h; $y++) {
        for ($x = 0; $x -lt $w; $x++) {
            $p = $bmp.GetPixel($x, $y)
            $dist = [Math]::Abs($p.R - $bgR) + [Math]::Abs($p.G - $bgG) + [Math]::Abs($p.B - $bgB)
            if ($dist -gt 60) {
                $subject++
                if ($x -lt $minX) { $minX = $x }; if ($x -gt $maxX) { $maxX = $x }
                if ($y -lt $minY) { $minY = $y }; if ($y -gt $maxY) { $maxY = $y }
                $key = "{0},{1},{2}" -f ([Math]::Round($p.R/32)*32), ([Math]::Round($p.G/32)*32), ([Math]::Round($p.B/32)*32)
                if ($colors.ContainsKey($key)) { $colors[$key]++ } else { $colors[$key] = 1 }
            }
        }
    }
    Write-Host "  主体像素数: $subject"
    if ($subject -gt 0) {
        $marginL = $minX; $marginT = $minY; $marginR = $w - 1 - $maxX; $marginB = $h - 1 - $maxY
        Write-Host ("  主体边界框: x[{0}..{1}] y[{2}..{3}]" -f $minX, $maxX, $minY, $maxY)
        Write-Host ("  边距 左{0} 上{1} 右{2} 下{3} px" -f $marginL, $marginT, $marginR, $marginB)
        Write-Host ("  最小边距: {0}px  (规范要求 >=3px)" -f ([Math]::Min($marginL, [Math]::Min($marginT, [Math]::Min($marginR, $marginB)))))
        $top = $colors.GetEnumerator() | Sort-Object Value -Descending | Select-Object -First 6
        Write-Host ("  主色(量化32): {0}" -f (($top | ForEach-Object { "rgb($($_.Key))x$($_.Value)" }) -join "  "))
        $cx = ($minX + $maxX) / 2; $cy = ($minY + $maxY) / 2
        Write-Host ("  主体中心: ({0:N0},{1:N0})  画布中心: (46,32)" -f $cx, $cy)
    }
    $bmp.Dispose()
}

$ref = 'D:\DataCherryStudio\Data\Agents\system\2026-09-09\bb43ed99-aecc-467f-91fd-77bff21182b7\build\2048'
$khi = 'D:\GitHub\khicas-opt\giacbf'

Analyze-Img "$ref\unselected.bmp" "2048 未选中(unselected)"
Analyze-Img "$ref\selected.bmp"   "2048 选中(selected)"
Analyze-Img "$khi\khicasio.png"   "KhiCAS 未选中(khicasio.png)"
Analyze-Img "$khi\khicasio1.png"  "KhiCAS 选中(khicasio1.png)"
