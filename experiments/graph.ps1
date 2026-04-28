Add-Type -AssemblyName System.Windows.Forms.DataVisualization

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$csvPath = Join-Path $root "results.csv"
$outDir = $root

$widthOrder = @("Width 1", "Width 2", "Width 4", "Width 8")
$branchOrder = @("Static Not Taken", "Static Taken", "One Bit", "BTFNT", "Two Bit")
$vectorOrder = @("Baseline", "VLEN 128", "VLEN 256", "VLEN 512")

function Normalize-BenchmarkName {
    param([string]$Name)

    if ($Name.EndsWith("_vec")) {
        return $Name.Substring(0, $Name.Length - 4)
    }

    return $Name
}

function Get-GroupedSeries {
    param(
        [object[]]$Rows,
        [string[]]$ConfigOrder,
        [string]$Metric
    )

    $benchmarks = [System.Collections.Generic.List[string]]::new()
    $grouped = @{}

    foreach ($row in $Rows) {
        $config = [string]$row.Config
        if ($ConfigOrder -notcontains $config) {
            continue
        }

        $benchmark = Normalize-BenchmarkName ([string]$row.Benchmark)
        if (-not $benchmarks.Contains($benchmark)) {
            $benchmarks.Add($benchmark)
        }

        if (-not $grouped.ContainsKey($benchmark)) {
            $grouped[$benchmark] = @{}
        }

        $grouped[$benchmark][$config] = [double]$row.$Metric
    }

    $seriesConfigs = foreach ($config in $ConfigOrder) {
        foreach ($benchmark in $benchmarks) {
            if ($grouped[$benchmark].ContainsKey($config)) {
                $config
                break
            }
        }
    }

    return @{
        Benchmarks = $benchmarks
        Grouped = $grouped
        SeriesConfigs = @($seriesConfigs)
    }
}

function New-GroupedBarChart {
    param(
        [object[]]$Rows,
        [string[]]$ConfigOrder,
        [string]$Metric,
        [string]$Title,
        [string]$YAxisTitle,
        [string]$OutputPath,
        [switch]$LogYAxis
    )

    $data = Get-GroupedSeries -Rows $Rows -ConfigOrder $ConfigOrder -Metric $Metric

    $chart = New-Object System.Windows.Forms.DataVisualization.Charting.Chart
    $chart.Width = 1400
    $chart.Height = 800
    $chart.BackColor = [System.Drawing.Color]::White

    $chartArea = New-Object System.Windows.Forms.DataVisualization.Charting.ChartArea "Main"
    $chartArea.AxisX.Title = "Benchmark"
    $chartArea.AxisY.Title = $YAxisTitle
    $chartArea.AxisX.Interval = 1
    $chartArea.AxisX.MajorGrid.Enabled = $false
    $chartArea.AxisY.MajorGrid.LineDashStyle = [System.Windows.Forms.DataVisualization.Charting.ChartDashStyle]::Dash
    $chartArea.AxisY.MajorGrid.LineColor = [System.Drawing.Color]::LightGray
    $chartArea.AxisY.IsLogarithmic = $LogYAxis.IsPresent
    $chartArea.AxisX.LabelStyle.Angle = -20
    $chartArea.BackColor = [System.Drawing.Color]::White
    [void]$chart.ChartAreas.Add($chartArea)

    $legend = New-Object System.Windows.Forms.DataVisualization.Charting.Legend
    $legend.Docking = [System.Windows.Forms.DataVisualization.Charting.Docking]::Top
    [void]$chart.Legends.Add($legend)

    [void]$chart.Titles.Add($Title)

    foreach ($config in $data.SeriesConfigs) {
        $series = New-Object System.Windows.Forms.DataVisualization.Charting.Series $config
        $series.ChartType = [System.Windows.Forms.DataVisualization.Charting.SeriesChartType]::Column
        $series.ChartArea = "Main"
        $series.IsValueShownAsLabel = $false

        foreach ($benchmark in $data.Benchmarks) {
            $value = [double]::NaN
            if ($data.Grouped[$benchmark].ContainsKey($config)) {
                $value = [double]$data.Grouped[$benchmark][$config]
            }

            $pointIndex = $series.Points.AddY($value)
            $series.Points[$pointIndex].AxisLabel = $benchmark
        }

        [void]$chart.Series.Add($series)
    }

    $chart.SaveImage($OutputPath, [System.Windows.Forms.DataVisualization.Charting.ChartImageFormat]::Png)
    $chart.Dispose()
}

if (-not (Test-Path $outDir)) {
    [void](New-Item -ItemType Directory -Path $outDir)
}

$rows = Import-Csv $csvPath

$widthRows = $rows | Where-Object { $_.Config -like "Width *" }
New-GroupedBarChart `
    -Rows $widthRows `
    -ConfigOrder $widthOrder `
    -Metric "IPC" `
    -Title "IPC by Width and Benchmark" `
    -YAxisTitle "IPC" `
    -OutputPath (Join-Path $outDir "ipc.png")

$branchRows = $rows | Where-Object { $branchOrder -contains $_.Config }
New-GroupedBarChart `
    -Rows $branchRows `
    -ConfigOrder $branchOrder `
    -Metric "Prediction Rate (%)" `
    -Title "Branch Prediction Accuracy by Policy and Benchmark" `
    -YAxisTitle "Prediction Accuracy (%)" `
    -OutputPath (Join-Path $outDir "accuracy.png")

$vectorRows = $rows | Where-Object { $vectorOrder -contains $_.Config }
New-GroupedBarChart `
    -Rows $vectorRows `
    -ConfigOrder $vectorOrder `
    -Metric "Cycles" `
    -Title "Cycles for Baseline and Vector Lengths by Benchmark" `
    -YAxisTitle "Cycles" `
    -OutputPath (Join-Path $outDir "cycles.png") `
    -LogYAxis
