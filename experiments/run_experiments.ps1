param(
	[switch]$ConfigsOnly,
	[switch]$GraphsOnly,
	[int]$RunTimeoutSeconds = 30
)

$ErrorActionPreference = "Stop"

$Root = Resolve-Path (Join-Path $PSScriptRoot "..")
$Benchmarks = @("matmul", "conv2d", "saxpy", "dotproduct", "factorial", "quicksort")
$Widths = @(1, 2, 4, 8)
$BranchPolicies = @("static_taken", "static_not_taken", "one_bit", "two_bit")
$Colors = @("#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b")

$Baseline = [ordered]@{
	"pipe-width" = 2
	"rs-size" = 8
	"lsq-size" = 8
	"alu-count" = 4
	"mul-count" = 1
	"ctrl-count" = 1
	"fpu-count" = 1
	"vec-count" = 1
	"lsu-count" = 4
	"branch" = "two_bit"
}

function Copy-Config($Config) {
	$copy = [ordered]@{}
	foreach ($key in $Config.Keys) {
		$copy[$key] = $Config[$key]
	}
	return $copy
}

function Write-Config($Name, $Config) {
	$path = Join-Path $Root "experiments/configs/$Name.json"
	$Config | ConvertTo-Json | Set-Content -Encoding utf8 $path
	return $path
}

function Build-Benchmark($Name) {
	$source = Join-Path $Root "bench/src/$Name.c"
	$elf = Join-Path $Root "bench/build/$Name.elf"
	$asm = Join-Path $Root "bench/build/$Name.asm"

	& riscv64-unknown-elf-gcc -march=rv32imf -mabi=ilp32 -mno-relax -O2 -ffreestanding -fno-pic -fno-pie -fno-builtin -fno-stack-protector -nostdlib -static -no-pie -T (Join-Path $Root "src/test/startup/linker.ld") (Join-Path $Root "src/test/startup/start.s") $source -o $elf
	if ($LASTEXITCODE -ne 0) { throw "failed to build $Name" }

	& riscv64-unknown-elf-objdump -d $elf | Set-Content -Encoding ascii $asm
	if ($LASTEXITCODE -ne 0) { throw "failed to disassemble $Name" }
}

function Parse-SimulatorOutput($Output) {
	$instructions = [regex]::Match($Output, "Instructions executed:\s+(\d+)").Groups[1].Value
	$cycles = [regex]::Match($Output, "Cycles taken:\s+(\d+)").Groups[1].Value
	$ipc = [regex]::Match($Output, "IPC:\s+([0-9.]+)").Groups[1].Value
	$branches = [regex]::Match($Output, "Branches:\s+(\d+)").Groups[1].Value
	$mispred = [regex]::Match($Output, "Mispredictions:\s+(\d+)\s+\(([0-9.]+)%\)")

	if (-not $instructions -or -not $cycles -or -not $ipc -or -not $branches -or -not $mispred.Success) {
		throw "could not parse simulator output: $Output"
	}

	return @{
		"instructions" = [int]$instructions
		"cycles" = [int]$cycles
		"ipc" = [double]$ipc
		"branches" = [int]$branches
		"mispredictions" = [int]$mispred.Groups[1].Value
		"mispred_rate" = [double]$mispred.Groups[2].Value
	}
}

function Run-Simulator($Benchmark, $ConfigPath) {
	$exe = Join-Path $Root "build/main.exe"
	$elf = Join-Path $Root "bench/build/$Benchmark.elf"
	$configName = [System.IO.Path]::GetFileNameWithoutExtension($ConfigPath)
	Write-Host "Running $configName / $Benchmark"

	$job = Start-Job -ScriptBlock {
		param($Exe, $Elf, $ConfigPath)
		& $Exe --headless --elf $Elf --config $ConfigPath | Out-String
		if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
	} -ArgumentList $exe,$elf,$ConfigPath

	$done = Wait-Job $job -Timeout $RunTimeoutSeconds
	if (-not $done) {
		Stop-Job $job
		Remove-Job $job
		throw "simulator timed out after ${RunTimeoutSeconds}s for $Benchmark with $ConfigName"
	}

	$output = Receive-Job $job
	$state = $job.State
	Remove-Job $job
	if ($state -ne "Completed") { throw "simulator failed for $Benchmark with $ConfigName" }
	return Parse-SimulatorOutput $output
}

function New-Row($Experiment, $Benchmark, $Config, $Metrics) {
	$correctRate = 100.0 - $Metrics["mispred_rate"]
	if ($correctRate -lt 0.0) { $correctRate = 0.0 }
	if ($correctRate -gt 100.0) { $correctRate = 100.0 }

	return [pscustomobject]@{
		experiment = $Experiment
		benchmark = $Benchmark
		pipe_width = $Config["pipe-width"]
		rs_size = $Config["rs-size"]
		lsq_size = $Config["lsq-size"]
		alu_count = $Config["alu-count"]
		mul_count = $Config["mul-count"]
		ctrl_count = $Config["ctrl-count"]
		fpu_count = $Config["fpu-count"]
		vec_count = $Config["vec-count"]
		lsu_count = $Config["lsu-count"]
		branch = $Config["branch"]
		instructions = $Metrics["instructions"]
		cycles = $Metrics["cycles"]
		ipc = "{0:F3}" -f $Metrics["ipc"]
		branches = $Metrics["branches"]
		mispredictions = $Metrics["mispredictions"]
		mispred_rate = "{0:F2}" -f $Metrics["mispred_rate"]
		correct_prediction_rate = "{0:F2}" -f $correctRate
	}
}

function Get-NiceMax($Values) {
	$max = ($Values | Measure-Object -Maximum).Maximum
	if (-not $max -or $max -le 0) { return 1.0 }
	$magnitude = [math]::Pow(10, [math]::Floor([math]::Log10($max)))
	$scaled = $max / $magnitude
	if ($scaled -le 1) { return 1 * $magnitude }
	if ($scaled -le 2) { return 2 * $magnitude }
	if ($scaled -le 5) { return 5 * $magnitude }
	return 10 * $magnitude
}

function Svg-Text($X, $Y, $Text, $Size = 12, $Anchor = "middle", $Extra = "") {
	$escaped = [System.Security.SecurityElement]::Escape([string]$Text)
	return "<text x=`"$([math]::Round($X, 1))`" y=`"$([math]::Round($Y, 1))`" font-family=`"Arial, sans-serif`" font-size=`"$Size`" text-anchor=`"$Anchor`" $Extra>$escaped</text>"
}

function Ensure-CorrectPredictionRate($Rows) {
	foreach ($row in $Rows) {
		$correctRate = 100.0 - [double]$row.mispred_rate
		if ($correctRate -lt 0.0) { $correctRate = 0.0 }
		if ($correctRate -gt 100.0) { $correctRate = 100.0 }
		$row | Add-Member -NotePropertyName correct_prediction_rate -NotePropertyValue ("{0:F2}" -f $correctRate) -Force
	}
}

function Write-WidthGraph($Rows) {
	$widthRows = @($Rows | Where-Object { $_.experiment -eq "width" })
	$yMax = Get-NiceMax @($widthRows | ForEach-Object { [double]$_.ipc * 1.1 })
	$w = 1060; $h = 560; $left = 75; $right = 190; $top = 45; $bottom = 95
	$plotW = $w - $left - $right
	$plotH = $h - $top - $bottom
	$groupW = $plotW / $Benchmarks.Count
	$barGap = 4
	$barW = ($groupW * 0.78 - $barGap * ($Widths.Count - 1)) / $Widths.Count
	$svg = [System.Collections.Generic.List[string]]::new()

	$svg.Add("<svg xmlns=`"http://www.w3.org/2000/svg`" width=`"$w`" height=`"$h`" viewBox=`"0 0 $w $h`">")
	$svg.Add("<rect width=`"100%`" height=`"100%`" fill=`"white`"/>")
	$svg.Add((Svg-Text ($w / 2) 25 "Pipeline Width vs IPC" 18))

	for ($i = 0; $i -le 5; $i++) {
		$value = $yMax * $i / 5
		$y = $top + $plotH - ($value / $yMax) * $plotH
		$svg.Add("<line x1=`"$left`" y1=`"$([math]::Round($y, 1))`" x2=`"$($w - $right)`" y2=`"$([math]::Round($y, 1))`" stroke=`"#dddddd`"/>")
		$svg.Add((Svg-Text ($left - 12) ($y + 4) ("{0:F2}" -f $value) 11 "end"))
	}

	$svg.Add("<line x1=`"$left`" y1=`"$top`" x2=`"$left`" y2=`"$($top + $plotH)`" stroke=`"#222`"/>")
	$svg.Add("<line x1=`"$left`" y1=`"$($top + $plotH)`" x2=`"$($w - $right)`" y2=`"$($top + $plotH)`" stroke=`"#222`"/>")

	for ($b = 0; $b -lt $Benchmarks.Count; $b++) {
		$benchmark = $Benchmarks[$b]
		$groupX = $left + $b * $groupW + $groupW * 0.11
		$labelX = $left + $b * $groupW + $groupW / 2
		for ($i = 0; $i -lt $Widths.Count; $i++) {
			$pipeWidth = $Widths[$i]
			$color = $Colors[$i % $Colors.Count]
			$row = $widthRows | Where-Object { $_.benchmark -eq $benchmark -and [int]$_.pipe_width -eq $pipeWidth } | Select-Object -First 1
			$value = [double]$row.ipc
			$barH = ($value / $yMax) * $plotH
			$x = $groupX + $i * ($barW + $barGap)
			$y = $top + $plotH - $barH
			$svg.Add("<rect x=`"$([math]::Round($x, 1))`" y=`"$([math]::Round($y, 1))`" width=`"$([math]::Round($barW, 1))`" height=`"$([math]::Round($barH, 1))`" fill=`"$color`"/>")
		}
		$svg.Add((Svg-Text $labelX ($top + $plotH + 24) $benchmark 11))
	}

	for ($i = 0; $i -lt $Widths.Count; $i++) {
		$pipeWidth = $Widths[$i]
		$color = $Colors[$i % $Colors.Count]
		$legendY = $top + 20 + $i * 24
		$svg.Add("<rect x=`"$($w - $right + 35)`" y=`"$($legendY - 10)`" width=`"16`" height=`"16`" fill=`"$color`"/>")
		$svg.Add((Svg-Text ($w - $right + 60) ($legendY + 3) "width $pipeWidth" 12 "start"))
	}

	$svg.Add((Svg-Text ($left + $plotW / 2) ($h - 22) "Benchmark" 13))
	$svg.Add((Svg-Text 18 ($top + $plotH / 2) "IPC" 13 "middle" "transform=`"rotate(-90 18 280)`""))
	$svg.Add("</svg>")
	$svg | Set-Content -Encoding utf8 (Join-Path $Root "experiments/graphs/width_ipc.svg")
}

function Write-BarGraph($Rows, $Metric, $Title, $FileName, $YLabel) {
	$branchRows = @($Rows | Where-Object { $_.experiment -eq "branch" })
	if ($Metric -eq "correct_prediction_rate") {
		$yMax = 100.0
	} else {
		$yMax = Get-NiceMax @($branchRows | ForEach-Object { [double]$_.$Metric * 1.1 })
	}
	$w = 1060; $h = 560; $left = 75; $right = 190; $top = 45; $bottom = 95
	$plotW = $w - $left - $right
	$plotH = $h - $top - $bottom
	$groupW = $plotW / $Benchmarks.Count
	$barGap = 4
	$barW = ($groupW * 0.78 - $barGap * ($BranchPolicies.Count - 1)) / $BranchPolicies.Count
	$svg = [System.Collections.Generic.List[string]]::new()

	$svg.Add("<svg xmlns=`"http://www.w3.org/2000/svg`" width=`"$w`" height=`"$h`" viewBox=`"0 0 $w $h`">")
	$svg.Add("<rect width=`"100%`" height=`"100%`" fill=`"white`"/>")
	$svg.Add((Svg-Text ($w / 2) 25 $Title 18))

	for ($i = 0; $i -le 5; $i++) {
		$value = $yMax * $i / 5
		$y = $top + $plotH - ($value / $yMax) * $plotH
		$svg.Add("<line x1=`"$left`" y1=`"$([math]::Round($y, 1))`" x2=`"$($w - $right)`" y2=`"$([math]::Round($y, 1))`" stroke=`"#dddddd`"/>")
		$svg.Add((Svg-Text ($left - 12) ($y + 4) ("{0:F2}" -f $value) 11 "end"))
	}

	$svg.Add("<line x1=`"$left`" y1=`"$top`" x2=`"$left`" y2=`"$($top + $plotH)`" stroke=`"#222`"/>")
	$svg.Add("<line x1=`"$left`" y1=`"$($top + $plotH)`" x2=`"$($w - $right)`" y2=`"$($top + $plotH)`" stroke=`"#222`"/>")

	for ($b = 0; $b -lt $Benchmarks.Count; $b++) {
		$benchmark = $Benchmarks[$b]
		$groupX = $left + $b * $groupW + $groupW * 0.11
		$labelX = $left + $b * $groupW + $groupW / 2
		for ($p = 0; $p -lt $BranchPolicies.Count; $p++) {
			$policy = $BranchPolicies[$p]
			$color = $Colors[$p % $Colors.Count]
			$row = $branchRows | Where-Object { $_.benchmark -eq $benchmark -and $_.branch -eq $policy } | Select-Object -First 1
			$value = [double]$row.$Metric
			if ($Metric -eq "correct_prediction_rate" -and $value -gt 100.0) { $value = 100.0 }
			$barH = ($value / $yMax) * $plotH
			$x = $groupX + $p * ($barW + $barGap)
			$y = $top + $plotH - $barH
			$svg.Add("<rect x=`"$([math]::Round($x, 1))`" y=`"$([math]::Round($y, 1))`" width=`"$([math]::Round($barW, 1))`" height=`"$([math]::Round($barH, 1))`" fill=`"$color`"/>")
		}
		$svg.Add((Svg-Text $labelX ($top + $plotH + 24) $benchmark 11))
	}

	for ($p = 0; $p -lt $BranchPolicies.Count; $p++) {
		$policy = $BranchPolicies[$p]
		$color = $Colors[$p % $Colors.Count]
		$legendY = $top + 20 + $p * 24
		$svg.Add("<rect x=`"$($w - $right + 35)`" y=`"$($legendY - 10)`" width=`"16`" height=`"16`" fill=`"$color`"/>")
		$svg.Add((Svg-Text ($w - $right + 60) ($legendY + 3) $policy 12 "start"))
	}

	$svg.Add((Svg-Text ($left + $plotW / 2) ($h - 22) "Benchmark" 13))
	$svg.Add((Svg-Text 18 ($top + $plotH / 2) $YLabel 13 "middle" "transform=`"rotate(-90 18 280)`""))
	$svg.Add("</svg>")
	$svg | Set-Content -Encoding utf8 (Join-Path $Root "experiments/graphs/$FileName")
}

New-Item -ItemType Directory -Force -Path (Join-Path $Root "bench/build") | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $Root "experiments/configs") | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $Root "experiments/graphs") | Out-Null

if ($GraphsOnly) {
	$resultsPath = Join-Path $Root "experiments/results.csv"
	if (-not (Test-Path $resultsPath)) { throw "missing experiments/results.csv; run the experiments before -GraphsOnly" }
	$rows = @(Import-Csv $resultsPath)
	Ensure-CorrectPredictionRate $rows
	$rows | Export-Csv -NoTypeInformation -Encoding utf8 $resultsPath
	Remove-Item -ErrorAction SilentlyContinue (Join-Path $Root "experiments/graphs/branch_ipc.svg")
	Remove-Item -ErrorAction SilentlyContinue (Join-Path $Root "experiments/graphs/branch_mispred_rate.svg")
	Write-WidthGraph $rows
	Write-BarGraph $rows "correct_prediction_rate" "Branch Policy vs Correct Prediction Rate" "branch_correct_prediction_rate.svg" "Correct prediction rate (%)"
	Write-Host "Wrote experiments/graphs/width_ipc.svg"
	Write-Host "Wrote experiments/graphs/branch_correct_prediction_rate.svg"
	return
}

$widthConfigs = @{}
$widthConfigPaths = @{}
foreach ($width in $Widths) {
	$config = Copy-Config $Baseline
	$config["pipe-width"] = $width
	$config["rs-size"] = [math]::Max(8, 4 * $width)
	$config["lsq-size"] = [math]::Max(8, 4 * $width)
	foreach ($key in @("alu-count", "mul-count", "ctrl-count", "fpu-count", "vec-count", "lsu-count")) {
		$config[$key] = $width
	}
	$config["branch"] = "two_bit"
	$widthConfigs[$width] = $config
	$widthConfigPaths[$width] = Write-Config "width_$width" $config
}

$branchConfigs = @{}
$branchConfigPaths = @{}
foreach ($policy in $BranchPolicies) {
	$config = Copy-Config $Baseline
	$config["branch"] = $policy
	$branchConfigs[$policy] = $config
	$branchConfigPaths[$policy] = Write-Config "branch_$policy" $config
}

if ($ConfigsOnly) {
	Write-Host "Wrote experiment configs to experiments/configs"
	return
}

foreach ($benchmark in $Benchmarks) {
	Build-Benchmark $benchmark
}

$rows = @()

foreach ($width in $Widths) {
	$config = $widthConfigs[$width]
	$configPath = $widthConfigPaths[$width]

	foreach ($benchmark in $Benchmarks) {
		$metrics = Run-Simulator $benchmark $configPath
		$rows += New-Row "width" $benchmark $config $metrics
	}
}

foreach ($policy in $BranchPolicies) {
	$config = $branchConfigs[$policy]
	$configPath = $branchConfigPaths[$policy]

	foreach ($benchmark in $Benchmarks) {
		$metrics = Run-Simulator $benchmark $configPath
		$rows += New-Row "branch" $benchmark $config $metrics
	}
}

$resultsPath = Join-Path $Root "experiments/results.csv"
$rows | Export-Csv -NoTypeInformation -Encoding utf8 $resultsPath
Remove-Item -ErrorAction SilentlyContinue (Join-Path $Root "experiments/graphs/branch_ipc.svg")
Remove-Item -ErrorAction SilentlyContinue (Join-Path $Root "experiments/graphs/branch_mispred_rate.svg")
Write-WidthGraph $rows
Write-BarGraph $rows "correct_prediction_rate" "Branch Policy vs Correct Prediction Rate" "branch_correct_prediction_rate.svg" "Correct prediction rate (%)"

Write-Host "Wrote experiments/results.csv"
Write-Host "Wrote experiments/graphs/width_ipc.svg"
Write-Host "Wrote experiments/graphs/branch_correct_prediction_rate.svg"
