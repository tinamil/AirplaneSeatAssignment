#1 for coughing, 2 for non-coughing
$risk_model = 1

#1 for vertex packing risk minimize
#2 for risk constrained vertex packing
$model_types = 1

$myLog = $PSScriptRoot + '/logs/' + $model_type + '/'
$input = $PSScriptRoot + '/graphs/Delta/'

$stdErrLog = $myLog + 'stderr.log'
$stdOutLog = $myLog + 'stdout.log'

if($model_types -eq 1){
    $values = (20, 40, 80, 150, 250)
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
            Start-Process -FilePath "${PSScriptRoot}/x64/Release/AirplaneSeatAssignment.exe" -ArgumentList '-n', $val, '-m', $model_type, '-t', $risk_model, $input_file_name  -RedirectStandardOutput $output_file -RedirectStandardError $stdErrLog -wait
        }
    }
}