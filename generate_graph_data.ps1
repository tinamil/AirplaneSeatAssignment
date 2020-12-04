#1 for vertex packing risk minimize
#2 for risk constrained vertex packing
$model_types = 1

#1 for coughing, 2 for non-coughing
$risk_model = 2

$myLog = $PSScriptRoot + '/logs/graph_data/noncoughing/' 
$input = $PSScriptRoot + '/graphs/Delta/'

$stdErrLog = $myLog + 'stderr.log'
$stdOutLog = $myLog + 'stdout.log'

$files = ("Boeing 717-200(717).graph")
foreach($model_type in $model_types)
{
    For ($val=110; $val -lt 111; ++$val)
    {
        foreach ($input_file in $files)
        {
    
            $output_file = $myLog + $input_file + $val + "_v2.txt"
            $error_file = $myLog + $input_file + ".err"
            $input_file_name = -join ("`"", $input, $input_file, "`"")
            Write-Output $output_file
            Start-Process -File "${PSScriptRoot}/x64/Release/AirplaneSeatAssignment.exe" -ArgumentList '-n', $val, '-m', $model_type, '-t', $risk_model, $input_file_name  -RedirectStandardOutput $output_file -RedirectStandardError $stdErrLog -wait -WindowStyle Minimized
        }
    }
}