#1 for vertex packing risk minimize
#2 for risk constrained vertex packing
$model_types = 2

$myLog = $PSScriptRoot + '/logs/' + $model_type + '/'
$input = $PSScriptRoot + '/graphs/Delta/'

$stdErrLog = $myLog + 'stderr.log'
$stdOutLog = $myLog + 'stdout.log'

#$values = (20, 40, 80, 150, 250)
$values = 100
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
            Start-Process -File x64\Release\AirplaneSeatAssignment.exe -ArgumentList '-n', $val, '-m', $model_type, $input_file_name  -RedirectStandardOutput $output_file -RedirectStandardError $stdErrLog -wait
        }
    }
}