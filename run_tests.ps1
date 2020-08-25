#1 for coughing, 2 for non-coughing
$risk_model = 1

$risk_label = "coughing"
if($risk_model -eq 2) {$risk_label = "non_coughing"}

#1 for vertex packing risk minimize
#2 for risk constrained vertex packing
$model_types = 1

$model_label = "vertex_packing"
if($model_types -eq 2) {$model_label = "risk_minimization"}

$myLog = $PSScriptRoot + '/logs_all/' + $model_label + '/' + $risk_label + '/'
$input = $PSScriptRoot + '/graphs/Delta/'

$stdErrLog = $myLog + 'stderr.log'
$stdOutLog = $myLog + 'stdout.log'

if($model_types -eq 1){
    $values = (1..306)
}
elseif($model_types -eq 2){
    $values = (0, 100, 1000, 10000)
}
$files = Get-ChildItem -Path $input -Force -Recurse
foreach($model_type in $model_types)
{
    foreach($val in $values)
    {
        foreach ($input_file in $files)
        {
    
            $output_file = $myLog + $input_file.Name + $val + ".txt"
            $error_file = $myLog + $input_file.Name + ".err"
            $input_file_name = -join ("`"", $input, $input_file.Name, "`"")
            Write-Output $output_file
            Start-Process -FilePath "${PSScriptRoot}/x64/Release/AirplaneSeatAssignment.exe" -ArgumentList '-n', $val, '-m', $model_type, '-t', $risk_model, $input_file_name  -RedirectStandardOutput $output_file -RedirectStandardError $stdErrLog -wait -WindowStyle Minimized
        }
    }
}