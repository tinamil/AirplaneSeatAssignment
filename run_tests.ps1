$myLog = $PSScriptRoot + '/logs/'
$input = $PSScriptRoot + '/graphs/Delta/'

$stdErrLog = $myLog + 'stderr.log'
$stdOutLog = $myLog + 'stdout.log'

$values = (20, 40, 80, 150, 250)
$files = Get-ChildItem -Path $input -Force -Recurse
foreach($val in $values)
{
    foreach ($input_file in $files)
    {
    
        $output_file = $myLog + $input_file.Name + $val + ".txt"
        $error_file = $myLog + $input_file.Name + ".err"
        $input_file_name = -join ("`"", $input, $input_file.Name, "`"")
        Write-Output $output_file
        Start-Process -File x64\Release\AirplaneSeatAssignment.exe -ArgumentList '-n', $val, $input_file_name  -RedirectStandardOutput $output_file -RedirectStandardError $stdErrLog -wait
    }
}