# icon-generate.ps1 - generate 2048-style KhiCAS icons (92x64, 24bpp RGB PNG)
# unselected: white bg + orange-red chi + gray shadow (like 2048 unselected)
# selected  : blue gradient bg + white chi + highlight (like 2048 selected)
Add-Type -AssemblyName System.Drawing

$W = 92; $H = 64
$outDir = 'D:\GitHub\khicas-opt\tools\out'
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

# chi geometry (large, top-aligned, bottom margin for OS text zone)
$cx = 46.0
$y0 = 10.0
$y1 = 44.0
$halfX = 30.0   # horizontal half-span of the crossing lines
$penW = 9.0

function Add-Chi([System.Drawing.Graphics]$g, [System.Drawing.Pen]$pen, [float]$ox, [float]$oy) {
    # two crossing thick round-cap lines (clean, no flare arcs - avoids touching edges)
    $g.DrawLine($pen, $cx - $halfX + $ox, $y0 + $oy, $cx + $halfX + $ox, $y1 + $oy)
    $g.DrawLine($pen, $cx + $halfX + $ox, $y0 + $oy, $cx - $halfX + $ox, $y1 + $oy)
}

function New-Icon([string]$path, [bool]$selected) {
    $bmp = New-Object System.Drawing.Bitmap($W, $H, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality

    # background
    if ($selected) {
        $rect = New-Object System.Drawing.Rectangle(0, 0, $W, $H)
        $grad = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
            $rect,
            [System.Drawing.Color]::FromArgb(115, 195, 255),   # top: light sky blue
            [System.Drawing.Color]::FromArgb(28, 86, 178),     # bottom: deep blue
            90.0)
        $g.FillRectangle($grad, $rect)
        $grad.Dispose()
        # top highlight band
        $hl = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(225, 242, 255))
        $g.FillRectangle($hl, 0, 0, $W, 4)
        $hl.Dispose()
    } else {
        $g.Clear([System.Drawing.Color]::White)
    }

    # shadow (offset 2,2)
    $shadowCol = if ($selected) { [System.Drawing.Color]::FromArgb(24, 70, 140) } else { [System.Drawing.Color]::FromArgb(198, 198, 198) }
    $spen = New-Object System.Drawing.Pen($shadowCol, $penW)
    $spen.StartCap = [System.Drawing.Drawing2D.LineCap]::Round
    $spen.EndCap = [System.Drawing.Drawing2D.LineCap]::Round
    Add-Chi $g $spen 2.0 2.0
    $spen.Dispose()

    # subject chi
    $bodyCol = if ($selected) { [System.Drawing.Color]::White } else { [System.Drawing.Color]::FromArgb(226, 92, 48) }
    $pen = New-Object System.Drawing.Pen($bodyCol, $penW)
    $pen.StartCap = [System.Drawing.Drawing2D.LineCap]::Round
    $pen.EndCap = [System.Drawing.Drawing2D.LineCap]::Round
    Add-Chi $g $pen 0.0 0.0
    $pen.Dispose()

    $g.Dispose()
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp.Dispose()
}

$unsPath = Join-Path $outDir 'khicasio.png'
$selPath = Join-Path $outDir 'khicasio1.png'
New-Icon $unsPath $false
New-Icon $selPath $true
Write-Host "generated:"
Write-Host "  $unsPath  (unselected: white bg + orange chi + shadow)"
Write-Host "  $selPath  (selected: blue gradient + white chi + highlight)"
