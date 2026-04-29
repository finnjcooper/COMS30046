Add-Type -AssemblyName System.Windows.Forms.DataVisualization

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$csvPath = Join-Path $root "results.csv"
$outDir = $root

$widthOrder = @("Width 1", "Width 2", "Width 4", "Width 8")
$branchOrder = @("Static Not Taken", "Static Taken", "One Bit", "BTFNT", "Two Bit")
$vectorOrder = @("Baseline", "VLEN 128", "VLEN 256", "VLEN 512")

function Normalize-BenchmarkName {
	param([string]$Name)

	if ($Name.EndsWith("v")) {
		return $Name.Substring(0, $Name.Length - 1)
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
	$chart.Width = 1080
	$chart.Height = 1080
	$chart.BackColor = [System.Drawing.Color]::FromArgb(0x18, 0x18, 0x25)

	$palette = @(
		# [System.Drawing.Color]::FromArgb(0x89, 0xB4, 0xFA),
		# [System.Drawing.Color]::FromArgb(0xA6, 0xE3, 0xA1),
		[System.Drawing.Color]::FromArgb(0xF9, 0xE2, 0xAF),
		[System.Drawing.Color]::FromArgb(0xFA, 0xB3, 0x87),
		[System.Drawing.Color]::FromArgb(0xF3, 0x8B, 0xA8),
		[System.Drawing.Color]::FromArgb(0xCB, 0xA6, 0xF7)
	)

	$chartArea = New-Object System.Windows.Forms.DataVisualization.Charting.ChartArea "Main"
	$chartArea.AxisX.Title = "Benchmark"
	$chartArea.AxisY.Title = $YAxisTitle
	$chartArea.AxisX.TitleFont = New-Object System.Drawing.Font("JetBrains Mono", 20, [System.Drawing.FontStyle]::Bold)
	$chartArea.AxisY.TitleFont = New-Object System.Drawing.Font("JetBrains Mono", 20, [System.Drawing.FontStyle]::Bold)
	$chartArea.AxisX.TitleForeColor = [System.Drawing.Color]::FromArgb(0xCD, 0xD6, 0xF4)
	$chartArea.AxisY.TitleForeColor = [System.Drawing.Color]::FromArgb(0xCD, 0xD6, 0xF4)
	$chartArea.AxisX.Interval = 1
	$chartArea.AxisX.MajorGrid.Enabled = $false
	$chartArea.AxisY.MajorGrid.LineDashStyle = [System.Windows.Forms.DataVisualization.Charting.ChartDashStyle]::Dash
	$chartArea.AxisY.MajorGrid.LineColor = [System.Drawing.Color]::LightGray
	$chartArea.AxisY.IsLogarithmic = $LogYAxis.IsPresent
	$chartArea.AxisX.LabelStyle.Angle = -20
	$chartArea.AxisX.IsLabelAutoFit = $false
	$chartArea.AxisY.IsLabelAutoFit = $false
	$chartArea.AxisX.LabelStyle.Font = New-Object System.Drawing.Font("JetBrains Mono", 14)
	$chartArea.AxisY.LabelStyle.Font = New-Object System.Drawing.Font("JetBrains Mono", 14)
	$chartArea.AxisX.LabelStyle.ForeColor = [System.Drawing.Color]::FromArgb(0xCD, 0xD6, 0xF4)
	$chartArea.AxisY.LabelStyle.ForeColor = [System.Drawing.Color]::FromArgb(0xCD, 0xD6, 0xF4)
	$chartArea.AxisX.LineColor = [System.Drawing.Color]::FromArgb(0xCD, 0xD6, 0xF4)
	$chartArea.AxisY.LineColor = [System.Drawing.Color]::FromArgb(0xCD, 0xD6, 0xF4)
	$chartArea.AxisX.MajorTickMark.LineColor = [System.Drawing.Color]::FromArgb(0xCD, 0xD6, 0xF4)
	$chartArea.AxisY.MajorTickMark.LineColor = [System.Drawing.Color]::FromArgb(0xCD, 0xD6, 0xF4)
	$chartArea.BackColor = [System.Drawing.Color]::FromArgb(0x18, 0x18, 0x25)
	[void]$chart.ChartAreas.Add($chartArea)

	$legend = New-Object System.Windows.Forms.DataVisualization.Charting.Legend
	$legend.Docking = [System.Windows.Forms.DataVisualization.Charting.Docking]::Top
	$legend.Font = New-Object System.Drawing.Font("JetBrains Mono", 14)
	$legend.BackColor = [System.Drawing.Color]::FromArgb(0x18, 0x18, 0x25)
	$legend.ForeColor = [System.Drawing.Color]::FromArgb(0xCD, 0xD6, 0xF4)
	[void]$chart.Legends.Add($legend)

	# [void]$chart.Titles.Add($Title)
	# $chart.Titles[0].Font = New-Object System.Drawing.Font("JetBrains Mono", 20, [System.Drawing.FontStyle]::Bold)
	# $chart.Titles[0].ForeColor = [System.Drawing.Color]::FromArgb(0xCD, 0xD6, 0xF4)

	$seriesIndex = 0
	foreach ($config in $data.SeriesConfigs) {
		$series = New-Object System.Windows.Forms.DataVisualization.Charting.Series $config
		$series.ChartType = [System.Windows.Forms.DataVisualization.Charting.SeriesChartType]::Column
		$series.ChartArea = "Main"
		$series.IsValueShownAsLabel = $false
		$series.Color = $palette[$seriesIndex % $palette.Count]
		$seriesIndex++

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
	-Title "Width Performance" `
	-YAxisTitle "IPC" `
	-OutputPath (Join-Path $outDir "ipc.png")

$branchRows = $rows | Where-Object { $branchOrder -contains $_.Config }
New-GroupedBarChart `
	-Rows $branchRows `
	-ConfigOrder $branchOrder `
	-Metric "Prediction Rate (%)" `
	-Title "Branch Prediction Performance" `
	-YAxisTitle "Prediction Accuracy (%)" `
	-OutputPath (Join-Path $outDir "accuracy.png")

$baselineCycles = @{}
foreach ($row in ($rows | Where-Object { $_.Config -eq "Baseline" })) {
	$bench = Normalize-BenchmarkName ([string]$row.Benchmark)
	$baselineCycles[$bench] = [double]$row.Cycles
}

$vectorRows = $rows | Where-Object { $vectorOrder -contains $_.Config -and $_.Config -ne "Baseline" } | ForEach-Object {
	$bench = Normalize-BenchmarkName ([string]$_.Benchmark)
	$baseline = $baselineCycles[$bench]
	$speedup = if ($baseline -and [double]$_.Cycles -gt 0) { $baseline / [double]$_.Cycles } else { [double]::NaN }
	$_ | Add-Member -NotePropertyName "Speedup" -NotePropertyValue $speedup -PassThru
}

New-GroupedBarChart `
	-Rows $vectorRows `
	-ConfigOrder $vectorOrder `
	-Metric "Speedup" `
	-Title "Vectorization Performance" `
	-YAxisTitle "Cycle Speedup vs Baseline" `
	-OutputPath (Join-Path $outDir "speedup.png")

$vectorRows = $rows | Where-Object { $vectorOrder -contains $_.Config }
New-GroupedBarChart `
	-Rows $vectorRows `
	-ConfigOrder $vectorOrder `
	-Metric "Cycles" `
	-Title "Vectorization Performance" `
	-YAxisTitle "Cycles" `
	-OutputPath (Join-Path $outDir "cycles.png") `
	-LogYAxis
