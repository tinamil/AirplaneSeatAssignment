#1 for vertex packing risk minimize
#2 for risk constrained vertex packing
$model_types = 1

$myLog = $PSScriptRoot + '/logs/graph_data/' 
$input = $PSScriptRoot + '/graphs/Delta/'

$stdErrLog = $myLog + 'stderr.log'
$stdOutLog = $myLog + 'stdout.log'

$files = ("Boeing 717-200(717).graph", "Airbus A330-300(333).graph")
foreach($model_type in $model_types)
{
    For ($val=1; $val -lt 250; ++$val)
    {
        foreach ($input_file in $files)
        {
    
            $output_file = $myLog + $input_file + $val + ".txt"
            $error_file = $myLog + $input_file + ".err"
            $input_file_name = -join ("`"", $input, $input_file, "`"")
            Write-Output $output_file
            Start-Process -File x64\Release\AirplaneSeatAssignment.exe -ArgumentList '-n', $val, '-m', $model_type, $input_file_name  -RedirectStandardOutput $output_file -RedirectStandardError $stdErrLog -wait
        }
    }
}