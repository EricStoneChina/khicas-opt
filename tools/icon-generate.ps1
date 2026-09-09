# icon-generate.ps1 - KhiCAS icon in the fx-CG50 OS main-menu style
# Style (from official app icons): full-bleed saturated gradient tile (square
# corners), large white pictogram, small dark rounded badge with white glyph
# in the top-right corner. No white margin, no drop shadow.
Add-Type -AssemblyName System.Drawing

$W = 92; $H = 64
$outDir = 'D:\GitHub\khicas-opt\tools\out'
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

function New-Icon([string]$path, [bool]$selected) {
    $bmp = New-Object System.Drawing.Bitmap($W, $H, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit

    # ---- full-bleed tile: blue vertical gradient (square corners) ----
    $rect = New-Object System.Drawing.Rectangle(0, 0, $W, $H)
    $top = if ($selected) { [System.Drawing.Color]::FromArgb(105, 175, 250) } else { [System.Drawing.Color]::FromArgb(58, 125, 220) }
    $bot = if ($selected) { [System.Drawing.Color]::FromArgb(30, 95, 195) } else { [System.Drawing.Color]::FromArgb(18, 62, 150) }
    $grad = New-Object System.Drawing.Drawing2D.LinearGradientBrush($rect, $top, $bot, 90.0)
    $g.FillRectangle($grad, $rect)
    $grad.Dispose()

    # soft top sheen
    $sheen = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
        (New-Object System.Drawing.Rectangle(0, 0, $W, 18)),
        [System.Drawing.Color]::FromArgb(60, 255, 255, 255),
        [System.Drawing.Color]::FromArgb(0, 255, 255, 255),
        90.0)
    $g.FillRectangle($sheen, 0, 0, $W, 18)
    $sheen.Dispose()

    # ---- pictogram: white chi ----
    $cx = 44.0; $y0 = 16.0; $y1 = 48.0; $halfX = 21.0; $penW = 9.0
    $pen = New-Object System.Drawing.Pen([System.Drawing.Color]::White, $penW)
    $pen.StartCap = [System.Drawing.Drawing2D.LineCap]::Round
    $pen.EndCap = [System.Drawing.Drawing2D.LineCap]::Round
    $g.DrawLine($pen, $cx - $halfX, $y0, $cx + $halfX, $y1)
    $g.DrawLine($pen, $cx + $halfX, $y0, $cx - $halfX, $y1)
    $pen.Dispose()

    # ---- badge: dark rounded square, white "CAS" (like official corner badges) ----
    # rendered at 4x then downscaled -> crisp small text
    $font = New-Object System.Drawing.Font('Arial', 7, [System.Drawing.FontStyle]::Bold, [System.Drawing.GraphicsUnit]::Pixel)
    $sfTight = [System.Drawing.StringFormat]::GenericTypographic
    $sz = $g.MeasureString('CAS', $font, 1000, $sfTight)
    $bw = [int][Math]::Ceiling($sz.Width) + 6
    $bh = [int][Math]::Ceiling($sz.Height) + 3
    $bx = $W - $bw - 3; $by = 3
    $ss = 4
    $tmp = New-Object System.Drawing.Bitmap(($bw*$ss), ($bh*$ss), [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $gt = [System.Drawing.Graphics]::FromImage($tmp)
    $gt.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $gt.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAlias
    $gt.Clear([System.Drawing.Color]::Transparent)
    $badge = New-Object System.Drawing.Drawing2D.GraphicsPath
    $d = 4*$ss
    $badge.AddArc(0, 0, $d, $d, 180, 90)
    $badge.AddArc($bw*$ss - $d, 0, $d, $d, 270, 90)
    $badge.AddArc($bw*$ss - $d, $bh*$ss - $d, $d, $d, 0, 90)
    $badge.AddArc(0, $bh*$ss - $d, $d, $d, 90, 90)
    $badge.CloseFigure()
    $badgeBrush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(20, 24, 34))
    $gt.FillPath($badgeBrush, $badge)
    $badgeBrush.Dispose()
    $bigFont = New-Object System.Drawing.Font('Arial', (7*$ss), [System.Drawing.FontStyle]::Bold, [System.Drawing.GraphicsUnit]::Pixel)
    $tb = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::White)
    $sf = New-Object System.Drawing.StringFormat
    $sf.Alignment = [System.Drawing.StringAlignment]::Center
    $sf.LineAlignment = [System.Drawing.StringAlignment]::Center
    $rectF = New-Object System.Drawing.RectangleF(0, 0, ($bw*$ss), ($bh*$ss))
    $gt.DrawString('CAS', $bigFont, $tb, $rectF, $sf)
    $gt.Dispose()
    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
    $g.DrawImage($tmp, (New-Object System.Drawing.Rectangle($bx, $by, $bw, $bh)))
    $tmp.Dispose()
    $tb.Dispose(); $font.Dispose(); $bigFont.Dispose(); $sf.Dispose(); $sfTight.Dispose(); $badge.Dispose()

    $g.Dispose()
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp.Dispose()
}

New-Icon (Join-Path $outDir 'khicasio.png') $false
New-Icon (Join-Path $outDir 'khicasio1.png') $true

# previews: 4x each + side-by-side comparison
$u = [System.Drawing.Bitmap]::FromFile((Join-Path $outDir 'khicasio.png'))
$s = [System.Drawing.Bitmap]::FromFile((Join-Path $outDir 'khicasio1.png'))
$scale = 4
foreach ($pair in @(@($u,'khicasio_x4.png'), @($s,'khicasio1_x4.png'))) {
    $big = New-Object System.Drawing.Bitmap(($pair[0].Width*$scale), ($pair[0].Height*$scale))
    $gg = [System.Drawing.Graphics]::FromImage($big)
    $gg.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
    $gg.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
    $gg.DrawImage($pair[0], 0, 0, $big.Width, $big.Height)
    $gg.Dispose()
    $big.Save((Join-Path $outDir $pair[1]), [System.Drawing.Imaging.ImageFormat]::Png)
    $big.Dispose()
}
$cmp = New-Object System.Drawing.Bitmap(($u.Width*$scale*2 + 12), ($u.Height*$scale + 8))
$g2 = [System.Drawing.Graphics]::FromImage($cmp)
$g2.Clear([System.Drawing.Color]::FromArgb(45,47,55))
$g2.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
$g2.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
$g2.DrawImage($u, 4, 4, $u.Width*$scale, $u.Height*$scale)
$g2.DrawImage($s, ($u.Width*$scale + 8), 4, $s.Width*$scale, $s.Height*$scale)
$g2.Dispose()
$cmp.Save((Join-Path $outDir 'compare_x4.png'), [System.Drawing.Imaging.ImageFormat]::Png)
$u.Dispose(); $s.Dispose(); $cmp.Dispose()
Write-Host "generated: khicasio.png (unselected), khicasio1.png (selected), compare_x4.png"
