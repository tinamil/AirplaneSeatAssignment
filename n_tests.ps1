#1 for coughing, 2 for non-coughing
$risk_model = 2

$risk_label = "coughing"
if($risk_model -eq 2) {$risk_label = "non_coughing"}

#1 for vertex packing risk minimize (only works with VPR)
$model_types = 1
$model_label = "vertex_packing"

$myLog = $PSScriptRoot + '/logs_n1/' + $model_label + '/' + $risk_label + '/'
$input = $PSScriptRoot + '/graphs/N-test/'

$stdErrLog = $myLog + 'stderr.log'
$stdOutLog = $myLog + 'stdout.log'

$starting_file = 'Boeing 717-200.graph'
$val_range = (1..110)

foreach ($val in $val_range)
{
    $input_file = [string]($val - 1) + '_' + $starting_file
    $output_file = $myLog + $starting_file + $val + ".txt"
    $error_file = $myLog + $input_file + ".err"
    $input_file_name = -join ("`"", $input, $input_file, "`"")
    Write-Output $output_file
    Start-Process -FilePath "${PSScriptRoot}/x64/Release/AirplaneSeatAssignment.exe" -ArgumentList '-n', $val, '-m', $model_type, '-t', $risk_model, $input_file_name  -RedirectStandardOutput $output_file -RedirectStandardError $stdErrLog -wait -WindowStyle Hidden

    $found_graph = $false
    foreach($line in Get-Content $output_file) {
        if($found_graph){
            if($line.Length -gt 0){
                Add-Content -Path ($input + $val + '_' + $starting_file) -Value $line
            }
        }
        elseif($line.Contains('Optimal value:')){ 
            $found_graph = $true
            Remove-Item -Path ($input + $val + '_' + $starting_file) -ErrorAction Ignore
        }
    }
}